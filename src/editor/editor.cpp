#include "editor/editor.h"
#include "widgets/text_widget.h"
#include "widgets/canvas_widget.h"
#include "editor/UI/toolbox.h"
#include "editor/UI/create_panel.h"
#include "editor/UI/edit_window.h"
#include "editor/UI/outliner.h"
#include "editor/UI/menu.h"
#include "common/utils.h"
#include "common/filedialog.h"
#include <numbers>
#include <iostream>
#include <ranges>

const auto tob2 = utils::tob2;
const auto tosf = utils::tosf;

Logger& operator<<(Logger& lg, const b2Vec2& value) {
    return lg << "(" << value.x << " " << value.y << ")";
}

sf::Vector2f to2f(sf::Vector2i vec) {
    return fw::to2f(vec);
}

sf::Vector2f to2f(sf::Vector2u vec) {
    return fw::to2f(vec);
}

bool QueryCallback::ReportFixture(b2Fixture* fixture) {
    fixtures.push_back(fixture);
    return true;
}

Editor::Editor(bool maximized) {
    this->maximize_window = maximized;
}

Editor::Editor(sf::RenderWindow& window, bool maximized) : Application(window) {
    this->maximize_window = maximized;
}

void Editor::init(const std::string& title, bool vsync) {
    fw::Application::init(title, WINDOW_WIDTH, WINDOW_HEIGHT, ANTIALIASING, vsync);
}

void Editor::load(const std::string& filename) {
    loadFromFile(filename);
}

void Editor::setCameraPos(float x, float y) {
    camera.setPosition(x, y);
}

void Editor::setCameraPos(const b2Vec2& pos) {
    camera.setPosition(pos);
}

void Editor::setCameraZoom(float zoom) {
    camera.setZoom(zoom);
}

fw::WidgetList& Editor::getWidgetList() {
    return widgets;
}

void Editor::selectSingleObject(GameObject* object, bool with_children) {
    select_tool.selectSingleObject(object, with_children);
}

Simulation& Editor::getSimulation() {
    return simulation;
}

SelectTool& Editor::getSelectTool() {
    return select_tool;
}

void Editor::setActiveObject(GameObject* object) {
    active_object = object;
    if (selected_tool == &edit_tool) {
        if (object) {
            edit_tool.edit_window_widget->updateParameters();
        } else {
            trySelectTool(&select_tool);
        }
    }
}

BoxObject* Editor::createBox(
    const std::string& name,
    const b2Vec2& pos,
    float angle,
    const b2Vec2& size,
    const sf::Color& color
) {
    return simulation.createBox(name, pos, angle, size, color);
}

BallObject* Editor::createBall(
    const std::string& name,
    const b2Vec2& pos,
    float radius,
    const sf::Color& color,
    const sf::Color& notch_color
) {
    return simulation.createBall(name, pos, radius, color, notch_color);
}

PolygonObject* Editor::createPolygon(
    const std::string& name,
    const b2Vec2& pos,
    float angle,
    const std::vector<b2Vec2>& vertices,
    const sf::Color& color
) {
    return simulation.createPolygon(name, pos, angle, vertices, color);
}

PolygonObject* Editor::createCar(
    const std::string& name,
    const b2Vec2& pos,
    const std::vector<float>& lengths,
    const std::vector<float>& wheels,
    const sf::Color& color
) {
    return simulation.createCar(name, pos, lengths, wheels, color);
}

ChainObject* Editor::createChain(
    const std::string& name,
    const b2Vec2& pos,
    float angle,
    const std::vector<b2Vec2>& vertices,
    const sf::Color& color
) {
    return simulation.createChain(name, pos, angle, vertices, color);
}

void Editor::onInit() {
    sf::ContextSettings cs_mask;
    window_view = sf::View(sf::FloatRect(0.0f, 0.0f, WINDOW_WIDTH, WINDOW_HEIGHT));
    if (!desat_shader.loadFromFile("shaders/desat.frag", sf::Shader::Fragment)) {
        throw std::runtime_error("Shader loading error");
    }
    if (!selection_shader.loadFromFile("shaders/selection.frag", sf::Shader::Fragment)) {
        throw std::runtime_error("Shader loading error");
    }
    initTools();
    initUi();
    editor_logger.OnLineWrite = [&](std::string line) { // should be after initUi and preferably before any logging
        logger_text_widget->setString(line);
    };
    trySelectToolByIndex(0); // should be after initTools and initUi
    selectCreateType(0);
    if (maximize_window) {
        maximizeWindow();
    }
    auto getter = [&]() { return serialize(); };
    auto setter = [&](std::string str) { deserialize(str, false); };
    history = History<std::string>("Editor", getter, setter);

    assert(tools.size() > 0);
    assert(selected_tool);
    for (size_t i = 0; i < tools_in_tool_panel.size(); i++) {
        assert(tools_in_tool_panel[i]->widget);
    }
}

void Editor::onStart() {
    history.clear();
    history.save("Base");
    fps_counter.init();
}

void Editor::onFrameBegin() {
    fps_counter.frameBegin();
}

void Editor::onFrameEnd() {
    int fps = fps_counter.frameEnd();
    fps_text_widget->setString(std::to_string(fps));
}

void Editor::onProcessWidgets() {
    if (edit_tool.edit_window_widget->isVisible() && active_object) {
        edit_tool.edit_window_widget->updateParameters();
    }
}

void Editor::onProcessWindowEvent(const sf::Event& event) {
    if (event.type == sf::Event::Closed) {
        window.close();
    }
}

void Editor::initTools() {
    for (size_t i = 0; i < tools.size(); i++) {
        tools[i]->reset();
    }
    create_tool.OnSetSelected += [&](bool value) {
        create_tool.create_panel_widget->setVisible(value);
    };
    edit_tool.OnSetSelectedWithButton += [&](bool value) {
        edit_tool.selected_tool = selected_tool;
    };
    edit_tool.OnSetSelected += [&](bool value) {
        if (value) {
            if (active_object) {
                edit_tool.edit_window_widget->updateParameters();
                edit_tool.edit_window_widget->setVisible(true);
            } else {
                edit_tool.edit_window_widget->setVisible(false);
            }
        } else {
            edit_tool.edit_window_widget->setVisible(false);
        }
    };
}

void Editor::initUi() {
    ui_font = fw::Font("fonts/STAN0757.TTF");
    fps_font = fw::Font("fonts/fraps.ttf");
    console_font = fw::Font("fonts/courbd.ttf");
    small_font = fw::Font("fonts/HelvetiPixel.ttf");
    textbox_font = fw::Font("fonts/verdana.ttf", true);
    setDefaultFont(textbox_font);

    arrow_cursor.loadFromSystem(sf::Cursor::Arrow);
    text_cursor.loadFromSystem(sf::Cursor::Text);
    size_top_left_cursor.loadFromSystem(sf::Cursor::SizeTopLeft);
    size_top_cursor.loadFromSystem(sf::Cursor::SizeTop);
    size_top_right_cursor.loadFromSystem(sf::Cursor::SizeTopRight);
    size_left_cursor.loadFromSystem(sf::Cursor::SizeLeft);
    size_right_cursor.loadFromSystem(sf::Cursor::SizeRight);
    size_bottom_left_cursor.loadFromSystem(sf::Cursor::SizeBottomLeft);
    size_bottom_cursor.loadFromSystem(sf::Cursor::SizeBottom);
    size_bottom_right_cursor.loadFromSystem(sf::Cursor::SizeBottomRight);
    window.setMouseCursor(arrow_cursor);

    vertex_text.setFont(ui_font.getSfmlFont());
    vertex_text.setCharacterSize(20);
    vertex_text.setFillColor(sf::Color::White);

    origin_shape.setRadius(3.0f);
    origin_shape.setOrigin(origin_shape.getRadius(), origin_shape.getRadius());
    origin_shape.setOutlineColor(sf::Color::Black);
    origin_shape.setOutlineThickness(1.0f);

    object_info_text.setFont(small_font.getSfmlFont());
    object_info_text.setCharacterSize(16);
    object_info_text.setFillColor(sf::Color::White);

    setBackgroundColor(sf::Color(25, 25, 25));

    initWidgets();
}

void Editor::initWidgets() {
    // canvas widgets
    world_widget = widgets.createWidget<fw::CanvasWidget>(
        (float)WINDOW_WIDTH, (float)WINDOW_HEIGHT, WINDOW_WIDTH, WINDOW_HEIGHT
    );
    world_widget->setName("world_canvas");
    world_widget->setShader(&desat_shader);
    world_widget->setClickThrough(false);
    world_widget->OnBeforeRender += [&](sf::RenderTarget& target) {
        desat_shader.setUniform("texture", sf::Shader::CurrentTexture);
        desat_shader.setUniform("saturation", WORLD_SATURATION);
        desat_shader.setUniform("vcenter", WORLD_COLOR_SCALE_CENTER);
        desat_shader.setUniform("vpercent", WORLD_COLOR_SCALE_PERCENT);
    };
    world_widget->OnPreUpdate += [&]() {
        world_widget->setSize((float)window.getSize().x, (float)window.getSize().y);
        world_widget->setTextureSize(window.getSize().x, window.getSize().y);
    };
    world_widget->OnLeftPress += [&](const sf::Vector2f& pos) {
        processLeftPress(pos);
    };
    world_widget->OnGlobalLeftRelease += [&](const sf::Vector2f& pos) {
        processGlobalLeftRelease(pos);
    };
    world_widget->OnBlockableLeftRelease += [&](const sf::Vector2f& pos) {
        processBlockableLeftRelease(pos);
    };
    world_widget->OnProcessMouse += [&](const sf::Vector2f& pos) {
        processMouse(pos);
    };
    world_widget->OnProcessDragGesture += [&](sf::Mouse::Button button, const sf::Vector2f& pos) {
        if (button == sf::Mouse::Left) {
            processDragGestureLeft(pos);
        } else if (button == sf::Mouse::Right) {
            processDragGestureRight(pos);
        }
    };
    world_widget->OnScrollY += [&](const sf::Vector2f& pos, float delta) {
        processMouseScrollY(delta);
    };

    selection_mask_widget = widgets.createWidget<fw::CanvasWidget>(
        (float)WINDOW_WIDTH, (float)WINDOW_HEIGHT, WINDOW_WIDTH, WINDOW_HEIGHT
    );
    selection_mask_widget->setName("selection_mask_canvas");
    selection_mask_widget->setShader(&selection_shader);
    selection_mask_widget->OnBeforeRender += [&](sf::RenderTarget& target) {
        selection_shader.setUniform("selection_mask", sf::Shader::CurrentTexture);
        selection_shader.setUniform("outline_color", SELECTION_OUTLINE_COLOR);
        selection_shader.setUniform("offset", SELECTION_OUTLINE_THICKNESS);
    };
    selection_mask_widget->OnPreUpdate += [&]() {
        selection_mask_widget->setSize((float)window.getSize().x, (float)window.getSize().y);
        selection_mask_widget->setTextureSize(window.getSize().x, window.getSize().y);
    };

    ui_widget = widgets.createWidget<fw::CanvasWidget>(
        (float)WINDOW_WIDTH, (float)WINDOW_HEIGHT, WINDOW_WIDTH, WINDOW_HEIGHT
    );
    ui_widget->setName("ui_canvas");
    ui_widget->OnPreUpdate += [&]() {
        ui_widget->setSize((float)window.getSize().x, (float)window.getSize().y);
        ui_widget->setTextureSize(window.getSize().x, window.getSize().y);
    };

    // ui widgets that have separate classes
    toolbox_widget = widgets.createWidget<Toolbox>(*this);
    edit_tool.edit_window_widget = widgets.createWidget<EditWindow>(300.0f, 400.0f, *this);
    create_tool.create_panel_widget = widgets.createWidget<CreatePanel>(*this);
    outliner_widget = widgets.createWidget<Outliner>(300.0f, 200.0f, *this);
    menu_widget = widgets.createWidget<Menu>(*this);
    menu_widget->setAnchorOffsetX(200.0f);

    // step text
    step_widget = widgets.createWidget<fw::TextWidget>();
    step_widget->setFont(ui_font);
    step_widget->setString(std::to_string(simulation.getStep()));
    step_widget->setCharacterSize(10);
    step_widget->setFillColor(sf::Color::White);
    step_widget->setOrigin(fw::Widget::Anchor::TOP_RIGHT);
    step_widget->setParentAnchor(fw::Widget::Anchor::TOP_RIGHT);
    step_widget->setName("step text");

    // pause widget
    paused_rect_widget = widgets.createWidget<fw::ContainerWidget>(20.0f, 20.0f);
    paused_rect_widget->setFillColor(sf::Color(0, 0, 0, 128));
    paused_rect_widget->setOrigin(fw::Widget::Anchor::TOP_LEFT);
    paused_rect_widget->setPadding(10.0f);
    paused_rect_widget->setName("paused rect");
    fw::TextWidget* paused_text_widget = widgets.createWidget<fw::TextWidget>();
    paused_text_widget->setFont(ui_font);
    paused_text_widget->setString("PAUSED");
    paused_text_widget->setCharacterSize(24);
    paused_text_widget->setFillColor(sf::Color::Yellow);
    paused_text_widget->setOrigin(fw::Widget::Anchor::TOP_LEFT);
    paused_text_widget->setParent(paused_rect_widget);

    // fps
    fps_widget = widgets.createWidget<fw::ContainerWidget>(20.0f, 20.0f);
    fps_widget->setFillColor(sf::Color::Yellow);
    fps_widget->setOrigin(fw::Widget::Anchor::TOP_LEFT);
    fps_widget->setPosition(120.0f, 0.0f);
    fps_widget->setPadding(0.0f);
    fps_widget->setName("fps");
    fps_text_widget = widgets.createWidget<fw::TextWidget>();
    fps_text_widget->setFont(fps_font);
    fps_text_widget->setCharacterSize(32);
    fps_text_widget->setFillColor(sf::Color::Black);
    fps_text_widget->setOrigin(fw::Widget::Anchor::TOP_LEFT);
    fps_text_widget->setAdjustLocalBounds(false);
    fps_text_widget->setParent(fps_widget);

    // logger
    logger_widget = widgets.createWidget<fw::RectangleWidget>(500.0f, 20.0f);
    logger_widget->setFillColor(sf::Color(0, 0, 0));
    logger_widget->setOrigin(fw::Widget::Anchor::BOTTOM_LEFT);
    logger_widget->setParentAnchor(fw::Widget::Anchor::BOTTOM_LEFT);
    logger_widget->setClipChildren(true);
    logger_widget->setName("logger");
    logger_text_widget = widgets.createWidget<fw::TextWidget>();
    logger_text_widget->setFont(console_font);
    logger_text_widget->setCharacterSize(15);
    logger_text_widget->setFillColor(sf::Color::White);
    logger_text_widget->setOrigin(fw::Widget::Anchor::TOP_LEFT);
    logger_text_widget->setParentAnchor(fw::Widget::Anchor::TOP_LEFT);
    logger_text_widget->setString("Logger message");
    logger_text_widget->setParent(logger_widget);

    edit_tool.edit_window_widget->moveToTop();
}

void Editor::onProcessKeyboardEvent(const sf::Event& event) {
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Escape) {
            if (selected_tool == &move_tool) {
                endMove(false);
                trySelectTool(move_tool.selected_tool);
                endGestureLeft();
            } else if (selected_tool == &rotate_tool) {
                endRotate(false);
                trySelectTool(rotate_tool.selected_tool);
                endGestureLeft();
            }
        } else if (event.key.code == sf::Keyboard::Space) {
            togglePause();
        } else if (event.key.code == sf::Keyboard::Num1) {
            trySelectToolByIndex(0);
        } else if (event.key.code == sf::Keyboard::Num2) {
            trySelectToolByIndex(1);
        } else if (event.key.code == sf::Keyboard::Num3) {
            trySelectToolByIndex(2);
        } else if (event.key.code == sf::Keyboard::Num4) {
            trySelectToolByIndex(3);
        } else if (event.key.code == sf::Keyboard::Num5) {
            trySelectToolByIndex(4);
        } else if (event.key.code == sf::Keyboard::Num6) {
            trySelectToolByIndex(5);
        } else if (event.key.code == sf::Keyboard::Num7) {
            trySelectToolByIndex(6);
        } else if (event.key.code == sf::Keyboard::Num8) {
            trySelectToolByIndex(7);
        } else if (event.key.code == sf::Keyboard::Num9) {
            trySelectToolByIndex(8);
        } else if (event.key.code == sf::Keyboard::Num0) {
            trySelectToolByIndex(9);
        } else if (event.key.code == sf::Keyboard::X) {
            if (selected_tool == &select_tool) {
                CompVector<GameObject*> selected_copy = select_tool.getSelectedObjects();
                for (GameObject* obj : selected_copy | std::views::reverse) {
                    deleteObject(obj, false);
                    commit_action = true;
                }
            } else if (selected_tool == &edit_tool && active_object) {
                if (active_object->tryDeleteVertex(edit_tool.highlighted_vertex)) {
                    commit_action = true;
                }
            }
        } else if (event.key.code == sf::Keyboard::LControl) {
            edit_tool.mode = EditTool::ADD;
        } else if (event.key.code == sf::Keyboard::LAlt) {
            edit_tool.mode = EditTool::INSERT;
        } else if (event.key.code == sf::Keyboard::S) {
            if (isLCtrlPressed()) {
                saveToFile("levels/level.txt");
            }
        } else if (event.key.code == sf::Keyboard::Z) {
            if (isLCtrlPressed()) {
                if (isLShiftPressed()) {
                    history.redo();
                } else {
                    history.undo();
                }
            }
        } else if (event.key.code == sf::Keyboard::Z) {
            quicksave();
        } else if (event.key.code == sf::Keyboard::W) {
            quickload_requested = true;
        } else if (event.key.code == sf::Keyboard::A) {
            if (selected_tool == &select_tool) {
                if (isLAltPressed()) {
                    for (GameObject* obj : simulation.getAllObjects()) {
                        select_tool.deselectObject(obj);
                    }
                } else {
                    for (GameObject* obj : simulation.getAllObjects()) {
                        select_tool.selectObject(obj);
                    }
                }
            } else if (selected_tool == &edit_tool && active_object) {
                if (isLAltPressed()) {
                    active_object->deselectAllVertices();
                } else {
                    active_object->selectAllVertices();
                }
            }
        } else if (event.key.code == sf::Keyboard::Tab) {
            if (selected_tool == &edit_tool) {
                trySelectTool(edit_tool.selected_tool);
            } else if (active_object) {
                edit_tool.selected_tool = selected_tool;
                trySelectTool(&edit_tool);
            }
        } else if (event.key.code == sf::Keyboard::G) {
            if (select_tool.selectedCount() > 0) {
                Tool* s_tool = selected_tool;
                trySelectTool(&move_tool);
                grabSelected(s_tool);
                startMoveGesture(world_widget);
            }
        } else if (event.key.code == sf::Keyboard::R) {
            if (select_tool.selectedCount() > 0) {
                Tool* s_tool = selected_tool;
                trySelectTool(&rotate_tool);
                rotateSelected(s_tool);
                startMoveGesture(world_widget);
            }
        } else if (event.key.code == sf::Keyboard::D) {
            if (isLShiftPressed()) {
                if (selected_tool == &select_tool && select_tool.selectedCount() > 0) {
                    CompVector<GameObject*> old_objects = select_tool.getSelectedObjects();
                    CompVector<GameObject*> new_objects = simulation.duplicate(old_objects);
                    select_tool.setSelected(new_objects);
                    Tool* s_tool = selected_tool;
                    trySelectTool(&move_tool);
                    grabSelected(s_tool);
                    commit_action = true;
                    startMoveGesture(world_widget);
                }
            } else {
                widgets.debug_render = !widgets.debug_render;
            }
        } else if (event.key.code == sf::Keyboard::I) {
            render_object_info = !render_object_info;
        } else if (event.key.code == sf::Keyboard::B) {
            debug_break = true;
        } else if (event.key.code == sf::Keyboard::E) {
            simulation.advance(timeStep);
        } else if (event.key.code == sf::Keyboard::Slash) {
            viewSelectedObjects();
        } else if (event.key.code == sf::Keyboard::F) {
            if (follow_object) {
                follow_object = nullptr;
            } else {
                follow_object = active_object;
            }
        }
    }
    if (event.type == sf::Event::KeyReleased) {
        if (event.key.code == sf::Keyboard::LControl) {
            if (edit_tool.mode == EditTool::ADD) {
                edit_tool.mode = EditTool::HOVER;
            }
        } else if (event.key.code == sf::Keyboard::LAlt) {
            if (edit_tool.mode == EditTool::INSERT) {
                edit_tool.mode = EditTool::HOVER;
            }
        }
    }
}

void Editor::processLeftPress(const sf::Vector2f& pos) {
    if (selected_tool == &create_tool) {
        std::string id_string = std::to_string(simulation.getMaxId() + 1);
        switch (create_tool.type) {
            case CreateTool::BOX:
                simulation.createBox(
                    "box" + id_string, getMouseWorldPosb2(), 0.0f, NEW_BOX_SIZE, NEW_BOX_COLOR
                );
                commit_action = true;
                break;
            case CreateTool::BALL:
                simulation.createBall(
                    "ball" + id_string, getMouseWorldPosb2(), NEW_BALL_RADIUS, NEW_BALL_COLOR, NEW_BALL_NOTCH_COLOR
                );
                commit_action = true;
                break;
        }
    } else if (selected_tool == &drag_tool) {
        b2Fixture* grabbed_fixture = getFixtureAt(getMousePosf());
        if (grabbed_fixture) {
            b2BodyDef mouse_body_def;
            b2Body* mouse_body = simulation.getWorld()->CreateBody(&mouse_body_def);
            drag_tool.mouse_body = std::unique_ptr<b2Body, std::function<void(b2Body*)>>(
                mouse_body,
                [&](b2Body* ptr) {
                    simulation.getWorld()->DestroyBody(ptr);
                }
            );
            b2Body* grabbed_body = grabbed_fixture->GetBody();
            b2MouseJointDef mouse_joint_def;
            mouse_joint_def.bodyA = mouse_body;
            mouse_joint_def.bodyB = grabbed_body;
            mouse_joint_def.damping = 1.0f;
            mouse_joint_def.maxForce = 5000.0f * grabbed_body->GetMass();
            mouse_joint_def.stiffness = 50.0f;
            mouse_joint_def.target = getMouseWorldPosb2();
            b2MouseJoint* mouse_joint = (b2MouseJoint*)simulation.getWorld()->CreateJoint(&mouse_joint_def);
            drag_tool.mouse_joint = std::unique_ptr<b2MouseJoint, std::function<void(b2MouseJoint*)>>(
                mouse_joint,
                [&](b2MouseJoint* ptr) {
                    simulation.getWorld()->DestroyJoint(ptr);
                });
            };
    } else if (selected_tool == &move_tool) {
        if (move_tool.moving_objects.size() > 0) {
            endMove(true);
            trySelectTool(move_tool.selected_tool);
        } else {
            grabSelected(selected_tool);
        }
    } else if (selected_tool == &rotate_tool) {
        if (rotate_tool.rotating_objects.size() > 0) {
            endRotate(true);
            trySelectTool(rotate_tool.selected_tool);
        } else {
            rotateSelected(selected_tool);
        }
    } else if (selected_tool == &edit_tool && active_object) {
        if (edit_tool.mode == EditTool::HOVER) {
            if (edit_tool.highlighted_vertex != -1) {
                edit_tool.mode = EditTool::MOVE;
                edit_tool.grabbed_vertex = edit_tool.highlighted_vertex;
                const EditableVertex& vertex = active_object->getVertex(edit_tool.grabbed_vertex);
                edit_tool.grabbed_vertex_offset = vertex.pos - active_object->toLocal(getMouseWorldPosb2());
                bool shift = isLShiftPressed();
                if (!vertex.selected && !shift) {
                    active_object->deselectAllVertices();
                } else if (shift) {
                    active_object->selectVertex(edit_tool.grabbed_vertex);
                }
            } else {
                edit_tool.mode = EditTool::SELECT;
                edit_tool.rectangle_select.active = true;
                edit_tool.rectangle_select.select_origin = getMouseWorldPos();
                if (!isLShiftPressed()) {
                    active_object->deselectAllVertices();
                }
            }
        } else if (edit_tool.mode == EditTool::ADD && edit_tool.edge_vertex != -1) {
            if (edit_tool.edge_vertex == 0) {
                active_object->addVertexGlobal(0, getMouseWorldPosb2());
            } else if (edit_tool.edge_vertex > 0) {
                active_object->addVertexGlobal(edit_tool.edge_vertex + 1, getMouseWorldPosb2());
            }
            commit_action = true;
        } else if (edit_tool.mode == EditTool::INSERT && edit_tool.highlighted_edge != -1) {
            active_object->addVertexGlobal(edit_tool.highlighted_edge + 1, edit_tool.insertVertexPos);
            commit_action = true;
        }
    }
}

void Editor::processGlobalLeftRelease(const sf::Vector2f& pos) {
    if (selected_tool == &move_tool) {
        endMove(true);
    } else if (selected_tool == &rotate_tool) {
        endRotate(true);
    }
    if (select_tool.rectangle_select.active) {
        select_tool.rectangle_select.active = false;
        select_tool.applyRectSelection();
    }
    if (drag_tool.mouse_joint) {
        // delete joint before deleting body
        drag_tool.mouse_joint.reset();
    }
    if (drag_tool.mouse_body) {
        drag_tool.mouse_body.reset();
    }
    if (edit_tool.grabbed_vertex != -1) {
        edit_tool.grabbed_vertex = -1;
        active_object->saveOffsets();
        commit_action = true;
        edit_tool.mode = EditTool::HOVER;
    }
    if (edit_tool.rectangle_select.active) {
        edit_tool.rectangle_select.active = false;
        edit_tool.mode = EditTool::HOVER;
    }
}

void Editor::processBlockableLeftRelease(const sf::Vector2f& pos) {
    if (selected_tool == &select_tool) {
        if (utils::length(getMousePosf() - getMousePressPosf()) < MOUSE_DRAG_THRESHOLD) {
            GameObject* object = getObjectAt(getMousePosf());
            setActiveObject(object);
            bool with_children = isLCtrlPressed();
            if (isLShiftPressed()) {
                select_tool.toggleSelect(object, with_children);
            } else {
                select_tool.selectSingleObject(object, with_children);
            }
        }
    }
}

void Editor::processMouseScrollY(float delta) {
    camera.setZoom(camera.getZoom() * pow(MOUSE_SCROLL_ZOOM, delta));
}

void Editor::processMouse(const sf::Vector2f& pos) {
    if (selected_tool == &select_tool) {
        {
            GameObject* old_hover = select_tool.hover_object;
            GameObject* new_hover = getObjectAt(getMousePosf());
            if (old_hover) {
                old_hover->hover = false;
            }
            if (new_hover) {
                new_hover->hover = true;
            }
            select_tool.hover_object = new_hover;
        }
    } else if (selected_tool == &edit_tool) {
        if (edit_tool.mode == EditTool::HOVER) {
            edit_tool.highlighted_vertex = mouseGetObjectVertex();
        } else if (edit_tool.mode == EditTool::ADD) {
            edit_tool.edge_vertex = mouseGetEdgeVertex();
        } else if (edit_tool.mode == EditTool::INSERT) {
            edit_tool.highlighted_edge = mouseGetObjectEdge();
            if (edit_tool.highlighted_edge != -1) {
                b2Vec2 v1 = active_object->getGlobalVertexPos(edit_tool.highlighted_edge);
                b2Vec2 v2 = active_object->getGlobalVertexPos(active_object->indexLoop(edit_tool.highlighted_edge + 1));
                edit_tool.insertVertexPos = utils::line_project(getMouseWorldPosb2(), v1, v2);
            }
        }
    } else if (selected_tool == &rotate_tool) {
        for (size_t i = 0; i < rotate_tool.rotating_objects.size(); i++) {
            GameObject* obj = rotate_tool.rotating_objects[i];
            b2Vec2 mouse_vector = getMouseWorldPosb2() - rotate_tool.pivot_pos;
            float current_mouse_angle = atan2(mouse_vector.y, mouse_vector.x);
            float offset = current_mouse_angle - rotate_tool.orig_mouse_angle;
            if (isLCtrlPressed()) {
                float offset_deg = utils::to_degrees(offset);
                float quantized_offset = ((int)(offset_deg / ROTATE_ANGLE_STEP)) * ROTATE_ANGLE_STEP;
                offset = utils::to_radians(quantized_offset);
            }
            b2Vec2 new_pos = utils::rotate_point(obj->orig_pos, rotate_tool.pivot_pos, offset);
            obj->setGlobalPosition(new_pos);
            obj->setGlobalAngle(obj->orig_angle + offset);
        }
    }
}

void Editor::processDragGestureLeft(const sf::Vector2f& pos) {
    if (selected_tool == &select_tool) {
        if (select_tool.rectangle_select.active) {
            selectObjectsInRect(select_tool.rectangle_select);
        } else if (utils::length(getMousePosf() - getMousePressPosf()) >= MOUSE_DRAG_THRESHOLD) {
            select_tool.rectangle_select.active = true;
            select_tool.rectangle_select.select_origin = screenToWorld(getMousePressPosf());
            if (!isLShiftPressed()) {
                select_tool.clearSelected();
            }
        }
    } else if (selected_tool == &drag_tool) {
        if (drag_tool.mouse_joint) {
            drag_tool.mouse_joint->SetTarget(getMouseWorldPosb2());
        }
    } else if (selected_tool == &edit_tool) {
        if (edit_tool.mode == EditTool::SELECT) {
            if (edit_tool.rectangle_select.active) {
                selectVerticesInRect(edit_tool.rectangle_select);
            }
        } else if (edit_tool.mode == EditTool::MOVE) {
            if (edit_tool.grabbed_vertex != -1) {
                ptrdiff_t index = edit_tool.grabbed_vertex;
                const EditableVertex& vertex = active_object->getVertex(index);
                b2Vec2 offset = active_object->toLocal(getMouseWorldPosb2()) + edit_tool.grabbed_vertex_offset - vertex.orig_pos;
                active_object->offsetVertex(index, offset, false);
                active_object->offsetSelected(offset, false);
                active_object->syncVertices();
            }
        }
    }
}

void Editor::processDragGestureRight(const sf::Vector2f& pos) {
    sf::Vector2i mouseDelta = mousePrevPos - getMousePos();
    float x_offset = mouseDelta.x / camera.getZoom();
    float y_offset = -mouseDelta.y / camera.getZoom();
    camera.move(x_offset, y_offset);
    if (mouseDelta.x != 0 || mouseDelta.y != 0) {
        follow_object = nullptr;
    }
    
}

void Editor::onAfterProcessInput() {
    // must be here to avoid a bug in situaltion when
    // the user is using move tool and moving camera at the same time
    if (selected_tool == &move_tool) {
        for (GameObject* obj : move_tool.moving_objects) {
            b2Vec2 mouse_pos = getMouseWorldPosb2();
            b2Vec2 cursor_offset = obj->cursor_offset;
            b2Vec2 new_pos = mouse_pos + cursor_offset;
            obj->setGlobalPosition(new_pos);
        }
    }
    if (commit_action) {
        history.save("Normal");
        commit_action = false;
    }
    if (quickload_requested) {
        quickload();
        if (history.getCurrent().tag != "Quickload") {
            history.save("Quickload");
        }
        quickload_requested = false;
    }
    if (load_request.requested) {
        loadFromFile(load_request.path);
        history.save("Load");
        load_request.requested = false;
    }
}

void Editor::onProcessWorld() {
    if (!paused) {
        simulation.advance(timeStep);
    }
    step_widget->setString(std::to_string(simulation.getStep()));
}

void Editor::onRender() {
    if (follow_object) {
        setCameraPos(follow_object->getGlobalPosition());
    }
    renderWorld();
    renderUi();
}

void Editor::renderWorld() {
    world_widget->clear(sf::Color::Transparent);
    world_widget->setViewCenter(tosf(camera.getPosition()));
    world_widget->setViewSize(world_widget->getSize().x / camera.getZoom(), -1.0f * world_widget->getSize().y / camera.getZoom());
    for (size_t i = 0; i < simulation.getTopSize(); i++) {
        GameObject* gameobject = simulation.getFromTop(i);
        gameobject->setDrawVarray(selected_tool == &edit_tool && gameobject == active_object);
        std::function<void(GameObject*)> render_object = [&](GameObject* object) {
            for (size_t i = 0; i < object->getChildren().size(); i++) {
                render_object(object->getChild(i));
            }
            object->updateVisual();
            canvasDraw(world_widget, *object->getDrawable());
        };
        render_object(gameobject);
    }
    world_widget->display();

    selection_mask_widget->clear();
    selection_mask_widget->setView(world_widget->getView());
    auto render_func = [&](const sf::Drawable& drawable) {
        canvasDraw(selection_mask_widget, drawable);
    };
    if (selected_tool != &edit_tool) {
        for (auto obj : select_tool.getSelectedObjects()) {
            obj->renderMask(render_func);
        }
    }
    selection_mask_widget->display();
}

void Editor::renderUi() {
    ui_widget->clear(sf::Color::Transparent);
    ui_widget->resetView();

    if (render_object_info) {
        // parent relation lines
        for (GameObject* object : simulation.getAllObjects()) {
            for (GameObject* child : object->getChildren()) {
                sf::Vector2f v1 = worldToScreen(object->getGlobalPosition());
                sf::Vector2f v2 = worldToScreen(child->getGlobalPosition());
                fw::draw_line(ui_widget, v1, v2, sf::Color(128, 128, 128));
            }
        }
        // object origin circles
        for (size_t i = 0; i < simulation.getAllSize(); i++) {
            GameObject* gameobject = simulation.getFromAll(i);
            sf::Color circle_color = gameobject == active_object ? sf::Color(255, 255, 0) : sf::Color(255, 159, 44);
            origin_shape.setFillColor(circle_color);
            origin_shape.setPosition(worldToScreen(gameobject->getGlobalPosition()));
            ui_widget->draw(origin_shape);
        }
        // object info
        for (size_t i = 0; i < simulation.getAllSize(); i++) {
            GameObject* gameobject = simulation.getFromAll(i);
            size_t info_index = 0;
            auto render_info = [&](const std::string& str) {
                sf::Vector2f object_screen_pos = worldToScreen(gameobject->getGlobalPosition());
                float line_scaling = 3.0f / 4.0f;
                sf::Vector2f offset = sf::Vector2f(0.0f, info_index * object_info_text.getCharacterSize() * line_scaling);
                sf::Vector2f pos = object_screen_pos + offset;
                object_info_text.setPosition(utils::quantize(pos));
                object_info_text.setString(str);
                ui_widget->draw(object_info_text);
                info_index++;
            };
            render_info("id: " + std::to_string(gameobject->getId()));
            render_info(gameobject->getName());
            render_info(utils::body_type_to_str(gameobject->getBodyType()));
        }
        // indices
        for (size_t i = 0; i < simulation.getAllSize(); i++) {
            if (PolygonObject* polygon_object = dynamic_cast<PolygonObject*>(simulation.getFromAll(i))) {
                if (polygon_object->draw_indices) {
                    // TODO: render indices
                }
            }
        }
    }

    if (selected_tool == &select_tool) {
        if (select_tool.rectangle_select.active) {
            renderRectangleSelect(ui_widget, select_tool.rectangle_select);
        }
    } else if (selected_tool == &drag_tool) {
        if (drag_tool.mouse_joint) {
            sf::Vector2f grabbed_point = worldToScreen(drag_tool.mouse_joint->GetAnchorB());
            fw::draw_line(ui_widget, grabbed_point, getMousePosf(), sf::Color::Yellow);
        }
    } else if (selected_tool == &rotate_tool) {
        for (size_t i = 0; i < rotate_tool.rotating_objects.size(); i++) {
            fw::draw_line(ui_widget, worldToScreen(rotate_tool.pivot_pos), getMousePosf(), sf::Color::Yellow);
        }
    } else if (selected_tool == &edit_tool && active_object) {
        if (edit_tool.mode == EditTool::ADD && edit_tool.edge_vertex != -1) {
            // ghost edge
            sf::Vector2i v1 = worldToPixel(active_object->getGlobalVertexPos(edit_tool.edge_vertex));
            sf::Vector2i v2 = getMousePos();
            fw::draw_line(ui_widget, to2f(v1), to2f(v2), sf::Color(255, 0, 0, 128));
            // ghost edge normal
            sf::Vector2f norm_v1, norm_v2;
            if (edit_tool.edge_vertex == 0) {
                getScreenNormal(v1, v2, norm_v1, norm_v2);
            } else {
                getScreenNormal(v2, v1, norm_v1, norm_v2);
            }
            fw::draw_line(ui_widget, norm_v1, norm_v2, sf::Color(0, 255, 255, 128));
            // ghost vertex
            sf::Vector2f ghost_vertex_pos = to2f(getMousePos());
            edit_tool.vertex_rect.setPosition(ghost_vertex_pos);
            edit_tool.vertex_rect.setFillColor(sf::Color(255, 0, 0, 128));
            ui_widget->draw(edit_tool.vertex_rect);
        } else if (edit_tool.mode == EditTool::INSERT && edit_tool.highlighted_edge != -1) {
            // edge highlight
            b2Vec2 v1 = active_object->getGlobalVertexPos(edit_tool.highlighted_edge);
            b2Vec2 v2 = active_object->getGlobalVertexPos(active_object->indexLoop(edit_tool.highlighted_edge + 1));
            sf::Vector2f v1_screen = worldToScreen(v1);
            sf::Vector2f v2_screen = worldToScreen(v2);
            sf::Vector2f vec = v2_screen - v1_screen;
            float angle = atan2(vec.y, vec.x);
            edit_tool.edge_highlight.setPosition(v1_screen);
            edit_tool.edge_highlight.setRotation(utils::to_degrees(angle));
            edit_tool.edge_highlight.setSize(sf::Vector2f(utils::length(vec), 3.0f));
            ui_widget->draw(edit_tool.edge_highlight);
            // ghost vertex on the edge
            sf::Vector2f ghost_vertex_pos = worldToScreen(edit_tool.insertVertexPos);
            edit_tool.vertex_rect.setFillColor(sf::Color(255, 0, 0, 128));
            edit_tool.vertex_rect.setPosition(ghost_vertex_pos);
            ui_widget->draw(edit_tool.vertex_rect);
        }
        // vertices
        for (size_t i = 0; i < active_object->getVertexCount(); i++) {
            edit_tool.vertex_rect.setPosition(worldToScreen(active_object->getGlobalVertexPos(i)));
            bool selected = active_object->isVertexSelected(i);
            sf::Color vertex_color = selected ? sf::Color(255, 255, 0) : sf::Color(255, 0, 0);
            edit_tool.vertex_rect.setFillColor(vertex_color);
            ui_widget->draw(edit_tool.vertex_rect);
        }
        // edge normals
        for (size_t i = 0; i < active_object->getEdgeCount(); i++) {
            sf::Vector2f norm_v1, norm_v2;
            getScreenNormal(
                active_object->getGlobalVertexPos(i),
                active_object->getGlobalVertexPos(active_object->indexLoop(i + 1)),
                norm_v1, 
                norm_v2
            );
            fw::draw_line(ui_widget, norm_v1, norm_v2, sf::Color(0, 255, 255));
        }
        if (edit_tool.mode == EditTool::HOVER && edit_tool.highlighted_vertex != -1) {
            // highlighted vertex
            sf::Vector2f vertex_pos = worldToScreen(active_object->getGlobalVertexPos(edit_tool.highlighted_vertex));
            edit_tool.vertex_highlight_rect.setPosition(vertex_pos);
            ui_widget->draw(edit_tool.vertex_highlight_rect);
        } else if (edit_tool.mode == EditTool::SELECT && edit_tool.rectangle_select.active) {
            //selection box
            renderRectangleSelect(ui_widget, edit_tool.rectangle_select);
        }
    }

    ui_widget->display();
}

std::string Editor::serialize() const {
    LoggerTag tag_serialize("serialize");
    editor_logger << __FUNCTION__"\n";
    LoggerIndent serialize_indent;
    TokenWriter tw;
    tw << "camera\n";
    {
        TokenWriterIndent camera_indent(tw);
        tw << "center" << camera.getPosition() << "\n";
        tw << "zoom" << camera.getZoom() << "\n";
    }
    tw << "/camera";
    tw << "\n\n";
    simulation.serialize(tw);
    return tw.toStr();
}

void Editor::deserialize(const std::string& str, bool set_camera) {
    ptrdiff_t active_object_id = -1;
    ptrdiff_t follow_object_id = -1;
    if (active_object) {
        active_object_id = active_object->getId();
    }
    if (follow_object) {
        follow_object_id = follow_object->getId();
    }
    initTools();
    TokenReader tr(str);
    try {
        while (tr.validRange()) {
            std::string entity = tr.readString();
            if (entity == "camera") {
                struct CameraParams {
                    float x = 0.0f, y = 0.0f, zoom = 30.0f;
                };
                CameraParams params;
                while (tr.validRange()) {
                    std::string pname = tr.readString();
                    if (pname == "center") {
                        params.x = tr.readFloat();
                        params.y = tr.readFloat();
                    } else if (pname == "zoom") {
                        params.zoom = tr.readFloat();
                    } else if (pname == "/camera") {
                        break;
                    } else {
                        throw std::runtime_error("Unknown camera parameter: " + pname);
                    }
                }
                if (set_camera) {
                    camera.setPosition(params.x, params.y);
                    camera.setZoom(params.zoom);
                }
            } else if (entity == "simulation") {
                simulation.deserialize(tr);
            } else {
                throw std::runtime_error("Unknown entity type: " + entity);
            }
        }
        if (active_object_id >= 0) {
            GameObject* object = simulation.getById(active_object_id);
            setActiveObject(object);
        }
        if (follow_object_id >= 0) {
            GameObject* object = simulation.getById(follow_object_id);
            follow_object = object;
        }
    } catch (std::exception exc) {
        throw std::runtime_error(__FUNCTION__": Line " + std::to_string(tr.getLine(-1)) + ": " + exc.what());
    }
}

void Editor::save() {
    if (save_file_location.empty()) {
        showSaveFileMenu();
    } else {
        saveToFile(save_file_location);
    }
}

void Editor::saveToFile(const std::filesystem::path& path) {
    LoggerTag tag_saveload("saveload");
    std::string str = serialize();
    utils::str_to_file(str, path);
    save_file_location = path;
    editor_logger << "Saved to " << path << "\n";
}

void Editor::loadAction(const std::filesystem::path& path) {
    load_request.requested = true;
    load_request.path = path;
}

void Editor::loadFromFile(const std::filesystem::path& path) {
    LoggerTag tag_saveload("saveload");
    try {
        std::string str = utils::file_to_str(path);
        deserialize(str, true);
        editor_logger << "Editor loaded from " << path << "\n";
    } catch (std::exception exc) {
        throw std::runtime_error(__FUNCTION__": " + path.string() + ": " + std::string(exc.what()));
    }
}

void Editor::quicksave() {
    LoggerTag tag_saveload("saveload");
    quicksave_str = serialize();
    editor_logger << "Quicksave\n";
}

void Editor::quickload() {
    LoggerTag tag_saveload("saveload");
    if (quicksave_str.size() > 0) {
        deserialize(quicksave_str, true);
        editor_logger << "Quickload\n";
    } else {
        editor_logger << "Can't quickload\n";
    }
}

void Editor::showOpenFileMenu() {
    std::wstring filename = open_file_dialog("levels/");
    endGestureLeft(); // dialog prevents mouse button release
    if (filename.empty()) {
        return;
    }
    loadFromFile(filename);
}

void Editor::showSaveFileMenu() {
    std::wstring filename = save_file_dialog("levels/");
    endGestureLeft();
    if (filename.empty()) {
        return;
    }
    saveToFile(filename);
}

Tool* Editor::trySelectToolByIndex(size_t index) {
    if (tools_in_tool_panel.size() > index) {
        Tool* tool = tools_in_tool_panel[index];
        trySelectTool(tool);
    }
    return nullptr;
}

Tool* Editor::trySelectTool(Tool* tool) {
    if (tool == selected_tool) {
        return tool;
    }
    if (selected_tool) {
        selected_tool->OnSetSelected(false);
    }
    selected_tool = tool;
    tool->OnSetSelected(true);
    for (size_t i = 0; i < tools_in_tool_panel.size(); i++) {
        tools_in_tool_panel[i]->widget->setFillColor(sf::Color(128, 128, 128));
    }
    if (tool->widget) {
        tool->widget->setFillColor(sf::Color::Yellow);
    }
    return tool;
}

void Editor::selectCreateType(size_t type) {
    create_tool.type = static_cast<CreateTool::ObjectType>(type);
    for (size_t j = 0; j < create_tool.create_buttons.size(); j++) {
        create_tool.create_buttons[j]->setFillColor(sf::Color(128, 128, 128));
    }
    create_tool.create_buttons[type]->setFillColor(sf::Color(0, 175, 255));
}

void Editor::togglePause() {
    paused = !paused;
    paused_rect_widget->setVisible(paused);
}

sf::Vector2f Editor::screenToWorld(const sf::Vector2f& screen_pos) const {
    sf::Transform combined = world_widget->getView().getInverseTransform() * ui_widget->getView().getTransform();
    sf::Vector2f result = combined.transformPoint(screen_pos);
    return result;
}

sf::Vector2f Editor::pixelToWorld(const sf::Vector2i& screen_pos) const {
    return screenToWorld(to2f(screen_pos) + sf::Vector2f(0.5f, 0.5f));
}

sf::Vector2f Editor::worldToScreen(const sf::Vector2f& world_pos) const {
    sf::Transform combined = ui_widget->getView().getInverseTransform() * world_widget->getView().getTransform();
    sf::Vector2f result = combined.transformPoint(world_pos);
    return result;
}

sf::Vector2f Editor::worldToScreen(const b2Vec2& world_pos) const {
    return worldToScreen(tosf(world_pos));
}

sf::Vector2i Editor::worldToPixel(const sf::Vector2f& world_pos) const {
    return fw::to2i(worldToScreen(world_pos));
}

sf::Vector2i Editor::worldToPixel(const b2Vec2& world_pos) const {
    return fw::to2i(worldToScreen(tosf(world_pos)));
}

sf::Vector2f Editor::worldDirToScreenf(const b2Vec2& world_dir) const {
    //TODO: calculate new direction using an actual sf::View
    return sf::Vector2f(world_dir.x, -world_dir.y);
}

sf::Vector2f Editor::getMouseWorldPos() const {
    return pixelToWorld(getMousePos());
}

b2Vec2 Editor::getMouseWorldPosb2() const {
    return tob2(getMouseWorldPos());
}

ptrdiff_t Editor::mouseGetChainEdge(const b2Fixture* fixture) const {
    const b2ChainShape* shape = dynamic_cast<const b2ChainShape*>(fixture->GetShape());
    const b2Body* body = fixture->GetBody();
    for (size_t i = 0; i < shape->m_count - 1; i++) {
        b2Vec2 p1 = body->GetWorldPoint(shape->m_vertices[i]);
        b2Vec2 p2 = body->GetWorldPoint(shape->m_vertices[i + 1]);
        b2Vec2 dir = p2 - p1;
        b2Vec2 side1_dir = utils::rot90CW(dir);
        b2Vec2 side2_dir = utils::rot90CCW(dir);
        b2Vec2 side1_p = p1 + side1_dir;
        b2Vec2 side2_p = p2 + side2_dir;
        bool inside =
            utils::left_side(getMouseWorldPosb2(), p1, side1_p)
            && utils::left_side(getMouseWorldPosb2(), p2, side2_p);
        if (!inside) {
            continue;
        }
        b2Vec2 b2_mouse_pos = tob2(to2f(getMousePos()));
        b2Vec2 b2_p1 = tob2(worldToScreen(p1));
        b2Vec2 b2_p2 = tob2(worldToScreen(p2));
        float dist = utils::distance_to_line(b2_mouse_pos, b2_p1, b2_p2);
        if (dist <= EditTool::EDGE_HIGHLIGHT_DISTANCE) {
            return i;
        }
    }
    return -1;
}

b2Fixture* Editor::getFixtureAt(const sf::Vector2f& screen_pos) const {
    b2Vec2 world_pos = tob2(screenToWorld(screen_pos));
    b2Vec2 world_pos_next = tob2(screenToWorld(screen_pos + sf::Vector2f(1.0f, 1.0f)));
    b2Vec2 midpoint = 0.5f * (world_pos + world_pos_next);
    QueryCallback callback;
    b2AABB aabb;
    aabb.upperBound = world_pos;
    aabb.lowerBound = world_pos_next;
    simulation.getWorld()->QueryAABB(&callback, aabb);
    for (size_t i = 0; i < callback.fixtures.size(); i++) {
        b2Fixture* fixture = callback.fixtures[i];
        if (fixture->GetShape()->m_type == b2Shape::e_chain) {
            if (mouseGetChainEdge(fixture) >= 0) {
                return fixture;
            }
        } else if (fixture->TestPoint(midpoint)) {
            return fixture;
        }
    }
    return nullptr;
}

GameObject* Editor::getObjectAt(const sf::Vector2f& screen_pos) const {
    GameObject* result = nullptr;
    b2Fixture* fixture = getFixtureAt(getMousePosf());
    if (fixture) {
        b2Body* body = fixture->GetBody();
        result = GameObject::getGameobject(body);
    }
    return result;
}

sf::Vector2f Editor::getObjectScreenPos(GameObject* object) const {
    return worldToScreen(object->getGlobalPosition());
}

b2AABB Editor::getObjectsAABB(const CompVector<GameObject*>& objects) const {
    b2AABB result;
    result.lowerBound = b2Vec2_zero;
    result.upperBound = b2Vec2_zero;
    if (objects.empty()) {
        return result;
    }
    result = objects.front()->getExactAABB();
    for (size_t i = 1; i < objects.size(); i++) {
        GameObject* object = objects[i];
        b2AABB aabb = object->getExactAABB();
        result.Combine(aabb);
    }
    return result;
}

ptrdiff_t Editor::mouseGetObjectVertex() const {
    if (!active_object) {
        return -1;
    }
    ptrdiff_t closest_vertex_i = 0;
    sf::Vector2i closest_vertex_pos = worldToPixel(active_object->getGlobalVertexPos(0));
    float closest_vertex_offset = utils::get_max_offset(closest_vertex_pos, getMousePos());
    for (size_t i = 1; i < active_object->getVertexCount(); i++) {
        sf::Vector2i vertex_pos = worldToPixel(active_object->getGlobalVertexPos(i));
        float offset = (float)utils::get_max_offset(vertex_pos, getMousePos());
        if (offset < closest_vertex_offset) {
            closest_vertex_i = i;
            closest_vertex_pos = vertex_pos;
            closest_vertex_offset = offset;
        }
    }
    if (closest_vertex_offset <= EditTool::VERTEX_HIGHLIGHT_DISTANCE) {
        return closest_vertex_i;
    }
    return -1;
}

ptrdiff_t Editor::mouseGetObjectEdge() const {
    if (!active_object) {
        return -1;
    }
    for (size_t i = 0; i < active_object->getEdgeCount(); i++) {
        b2Vec2 p1 = active_object->getGlobalVertexPos(i);
        b2Vec2 p2 = active_object->getGlobalVertexPos(active_object->indexLoop(i + 1));
        b2Vec2 dir = p2 - p1;
        b2Vec2 side1_dir = utils::rot90CW(dir);
        b2Vec2 side2_dir = utils::rot90CCW(dir);
        b2Vec2 side1_p = p1 + side1_dir;
        b2Vec2 side2_p = p2 + side2_dir;
        bool inside = 
            utils::left_side(getMouseWorldPosb2(), p1, side1_p)
            && utils::left_side(getMouseWorldPosb2(), p2, side2_p);
        if (!inside) {
            continue;
        }
        b2Vec2 b2_mouse_pos = tob2(to2f(getMousePos()));
        b2Vec2 b2_p1 = tob2(worldToScreen(p1));
        b2Vec2 b2_p2 = tob2(worldToScreen(p2));
        float dist = utils::distance_to_line(b2_mouse_pos, b2_p1, b2_p2);
        if (dist <= EditTool::EDGE_HIGHLIGHT_DISTANCE) {
            return i;
        }
    }
    return -1;
}

ptrdiff_t Editor::mouseGetEdgeVertex() const {
    if (!active_object) {
        return -1;
    }
    if (active_object->isClosed()) {
        return -1;
    }
    ptrdiff_t v_start_i = 0;
    ptrdiff_t v_end_i = active_object->getEdgeCount();
    sf::Vector2f v_start = worldToScreen(active_object->getGlobalVertexPos(v_start_i));
    sf::Vector2f v_end = worldToScreen(active_object->getGlobalVertexPos(v_end_i));
    float v_start_dist = utils::length(getMousePosf() - v_start);
    float v_end_dist = utils::length(getMousePosf() - v_end);
    if (v_start_dist <= v_end_dist) {
        return v_start_i;
    } else {
        return v_end_i;
    }
}

void Editor::selectVerticesInRect(const RectangleSelect& rectangle_select) {
    sf::Vector2f mpos = getMouseWorldPos();
    sf::Vector2f origin = rectangle_select.select_origin;
    float left = std::min(mpos.x, origin.x);
    float top = std::min(mpos.y, origin.y);
    float right = std::max(mpos.x, origin.x);
    float bottom = std::max(mpos.y, origin.y);
    float width = right - left;
    float height = bottom - top;
    sf::FloatRect rect(left, top, width, height);
    for (size_t i = 0; i < active_object->getVertexCount(); i++) {
        if (utils::contains_point(rect, tosf(active_object->getGlobalVertexPos(i)))) {
            active_object->selectVertex(i);
        }
    }
}

void Editor::selectObjectsInRect(const RectangleSelect& rectangle_select) {
    select_tool.clearRectSelected();
    b2AABB aabb;
    float lower_x = std::min(rectangle_select.select_origin.x, getMouseWorldPos().x);
    float lower_y = std::min(rectangle_select.select_origin.y, getMouseWorldPos().y);
    float upper_x = std::max(rectangle_select.select_origin.x, getMouseWorldPos().x);
    float upper_y = std::max(rectangle_select.select_origin.y, getMouseWorldPos().y);
    aabb.lowerBound = b2Vec2(lower_x, lower_y);
    aabb.upperBound = b2Vec2(upper_x, upper_y);
    QueryCallback callback;
    simulation.getWorld()->QueryAABB(&callback, aabb);
    for (size_t i = 0; i < callback.fixtures.size(); i++) {
        if (utils::rect_fixture_intersect(aabb.lowerBound, aabb.upperBound, callback.fixtures[i])) {
            GameObject* gameobject = GameObject::getGameobject(callback.fixtures[i]->GetBody());
            select_tool.addToRectSelection(gameobject);
        }
    }
}

void Editor::renderRectangleSelect(sf::RenderTarget& target, RectangleSelect& rectangle_select) {
    sf::Vector2f pos = worldToScreen(rectangle_select.select_origin);
    rectangle_select.select_rect.setPosition(pos);
    rectangle_select.select_rect.setSize(getMousePosf() - pos);
    target.draw(rectangle_select.select_rect);
}

void Editor::renderRectangleSelect(fw::CanvasWidget* canvas, RectangleSelect& rectangle_select) {
    sf::Vector2f pos = worldToScreen(rectangle_select.select_origin);
    rectangle_select.select_rect.setPosition(pos);
    rectangle_select.select_rect.setSize(getMousePosf() - pos);
    canvas->draw(rectangle_select.select_rect);
}

void Editor::getScreenNormal(const b2Vec2& v1, const b2Vec2& v2, sf::Vector2f& norm_v1, sf::Vector2f& norm_v2) const {
    b2Vec2 midpoint = 0.5f * (v1 + v2);
    norm_v1 = worldToScreen(midpoint);
    sf::Vector2f edge_dir = utils::normalize(worldDirToScreenf(v2 - v1));
    sf::Vector2f norm_dir = utils::rot90CCW(edge_dir);
    norm_v2 = norm_v1 + norm_dir * (float)EditTool::NORMAL_LENGTH;
}

void Editor::getScreenNormal(const sf::Vector2i& v1, const sf::Vector2i& v2, sf::Vector2f& norm_v1, sf::Vector2f& norm_v2) const {
    sf::Vector2f v1f = to2f(v1);
    sf::Vector2f v2f = to2f(v2);
    sf::Vector2f midpoint = (v1f + v2f) / 2.0f;
    norm_v1 = midpoint;
    sf::Vector2f edge_dir = utils::normalize(v2f - v1f);
    sf::Vector2f norm_dir = utils::rot90CW(edge_dir);
    norm_v2 = norm_v1 + norm_dir * (float)EditTool::NORMAL_LENGTH;
}

bool Editor::isParentSelected(const GameObject* object) const {
    CompVector<GameObject*> parents = object->getParentChain();
    for (size_t i = 0; i < parents.size(); i++) {
        auto it = select_tool.getSelectedObjects().find(parents[i]);
        if (it != select_tool.getSelectedObjects().getSet().end()) {
            return true;
        }
    }
    return false;
}

void Editor::grabSelected(Tool* selected_tool) {
    move_tool.orig_cursor_pos = getMouseWorldPosb2();
    move_tool.moving_objects = CompVector<GameObject*>();
    move_tool.selected_tool = selected_tool;
    for (GameObject* obj : select_tool.getSelectedObjects()) {
        if (isParentSelected(obj)) {
            continue;
        }
        move_tool.moving_objects.add(obj);
        obj->orig_pos = obj->getGlobalPosition();
        obj->cursor_offset = obj->getGlobalPosition() - getMouseWorldPosb2();
        obj->was_enabled = obj->getRigidBody()->IsEnabled();
        obj->setEnabled(false, true);
        obj->setLinearVelocity(b2Vec2(0.0f, 0.0f), true);
        obj->setAngularVelocity(0.0f, true);
    }
}

void Editor::rotateSelected(Tool* selected_tool) {
    rotate_tool.orig_cursor_pos = getMouseWorldPosb2();
    rotate_tool.rotating_objects = CompVector<GameObject*>();
    rotate_tool.selected_tool = selected_tool;
    for (GameObject* obj : select_tool.getSelectedObjects()) {
        if (isParentSelected(obj)) {
            continue;
        }
        rotate_tool.rotating_objects.add(obj);
        obj->orig_pos = obj->getGlobalPosition();
        obj->orig_angle = obj->getGlobalRotation();
        obj->was_enabled = obj->getRigidBody()->IsEnabled();
        obj->setEnabled(false, true);
        obj->setAngularVelocity(0.0f, true);
    }
    b2Vec2 sum = b2Vec2_zero;
    for (size_t i = 0; i < rotate_tool.rotating_objects.size(); i++) {
        sum += rotate_tool.rotating_objects[i]->getGlobalPosition();
    }
    b2Vec2 avg = 1.0f / rotate_tool.rotating_objects.size() * sum;
    rotate_tool.pivot_pos = avg;
    b2Vec2 mouse_vector = getMouseWorldPosb2() - avg;
    rotate_tool.orig_mouse_angle = atan2(mouse_vector.y, mouse_vector.x);
}

void Editor::endMove(bool confirm) {
    for (GameObject* obj : move_tool.moving_objects) {
        if (confirm) {
            commit_action = true;
        } else {
            obj->setGlobalPosition(obj->orig_pos);
        }
        //TODO: remember state for all children
        obj->setEnabled(obj->was_enabled, true);
    }
    move_tool.moving_objects.clear();
}

void Editor::endRotate(bool confirm) {
    for (GameObject* obj : rotate_tool.rotating_objects) {
        if (confirm) {
            commit_action = true;
        } else {
            obj->setGlobalPosition(obj->orig_pos);
            obj->setGlobalAngle(obj->orig_angle);
        }
        //TODO: remember state for all children
        obj->setEnabled(obj->was_enabled, true);
    }
    rotate_tool.rotating_objects.clear();
}

void Editor::deleteObject(GameObject* object, bool remove_children) {
    if (!object) {
        return;
    }
    if (object == active_object) {
        setActiveObject(nullptr);
    }
    if (object == follow_object) {
        follow_object = nullptr;
    }
    select_tool.deselectObject(object);
    simulation.remove(object, remove_children);
}

void Editor::viewSelectedObjects() {
    if (select_tool.selectedCount() == 0) {
        return;
    }
    b2AABB aabb = getObjectsAABB(select_tool.getSelectedObjects());
    float sizeX = aabb.upperBound.x - aabb.lowerBound.x;
    float sizeY = aabb.upperBound.y - aabb.lowerBound.y;
    float zoomX = window.getSize().x / sizeX;
    float zoomY = window.getSize().y / sizeY;
    float zoom = 1.0f;
    if (sizeX == 0.0f && sizeY > 0.0f) {
        zoom = zoomY;
    } else if (sizeX > 0.0f && sizeY == 0.0f) {
        zoom = zoomX;
    } else if (sizeX > 0.0f && sizeY > 0.0f) {
        zoom = std::min(zoomX, zoomY);
    } else {
        zoom = camera.getZoom();
    }
    b2Vec2 aabb_center = aabb.GetCenter();
    setCameraPos(aabb_center);
    setCameraZoom(zoom);
}

void Editor::checkDebugbreak() {
    if (debug_break) {
        __debugbreak();
        debug_break = false;
    }
}

void Editor::canvasDraw(fw::CanvasWidget* canvas, const sf::Drawable& drawable, const sf::RenderStates& states) {
    if (const sf::Shape* shape = dynamic_cast<const sf::Shape*>(&drawable)) {
        canvas->draw(*shape);
    } else if (dynamic_cast<const LineStripShape*>(&drawable)) {
        canvas->draw(drawable, fw::ColorType::VERTEX);
    } else if (dynamic_cast<const CircleNotchShape*>(&drawable)) {
        canvas->draw(drawable, fw::ColorType::VERTEX);
    } else if (dynamic_cast<const SplittablePolygon*>(&drawable)) {
        canvas->draw(drawable, fw::ColorType::VERTEX);
    } else {
        mAssert(false, "This Drawable is not implemented yet");
    }
}

void FpsCounter::init() {
    last_fps_time = std::chrono::high_resolution_clock::now();
}

void FpsCounter::frameBegin() {
    t1 = std::chrono::high_resolution_clock::now();
}

int FpsCounter::frameEnd() {
    t2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> frame_time = t2 - t1;
    std::chrono::duration<double> fps_time = t2 - last_fps_time;
    if (fps_time.count() >= 1.0) {
        if (frame_times.size() > 0) {
            double average_frame_time = utils::average(frame_times);
            fps = (int)(1.0 / average_frame_time);
            frame_times.clear();
        }
        last_fps_time = t2;
    }
    frame_times.push_back(frame_time.count());
    return fps;
}

int FpsCounter::getFps() const {
    return fps;
}

Editor::Camera::Camera(Editor& editor) : editor(editor) { }

const b2Vec2& Editor::Camera::getPosition() const {
    return pos;
}

float Editor::Camera::getZoom() const {
    return zoom;
}

void Editor::Camera::setPosition(float x, float y) {
    pos.x = x;
    pos.y = y;
    editor.world_widget->setViewCenter(x, y);
}

void Editor::Camera::setPosition(const b2Vec2& p_pos) {
    setPosition(p_pos.x, p_pos.y);;
}

void Editor::Camera::move(float x, float y) {
    setPosition(pos.x + x, pos.y + y);
}

void Editor::Camera::move(const b2Vec2& offset) {
    move(offset.x, offset.y);
}

void Editor::Camera::setZoom(float zoom) {
    this->zoom = zoom;
    editor.world_widget->setViewSize(
        editor.world_widget->getSize().x / zoom,
        -1.0f * editor.world_widget->getSize().y / zoom
    );
}
