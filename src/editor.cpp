#include "editor.h"
#include "utils.h"
#include <numbers>
#include <iostream>
#include <ranges>
#include "UI/toolbox.h"
#include "UI/create_panel.h"

const auto tob2 = utils::tob2;
const auto tosf = utils::tosf;
const auto to2i = fw::to2i;

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

void Editor::init() {
    fw::Application::init(WINDOW_TITLE, WINDOW_WIDTH, WINDOW_HEIGHT, ANTIALIASING, true);
}

void Editor::load(const std::string& filename) {
    loadFromFile(filename);
}

void Editor::setCameraPos(float x, float y) {
    viewCenterX = x;
    viewCenterY = y;
}

void Editor::setCameraZoom(float zoom) {
    zoomFactor = zoom;
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
    logger.OnLineWrite = [&](std::string line) { // should be after initUi and preferably before any logging
        logger_text_widget->setString(line);
    };
    trySelectToolByIndex(0); // should be after initTools and initUi
    selectCreateType(0);
    maximizeWindow();
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
    create_tool.OnSetSelected = [&](bool value) {
        create_tool.create_panel_widget->setVisible(value);
    };
    edit_tool.OnSetSelectedWithButton = [&](bool value) {
        edit_tool.selected_tool = selected_tool;
    };
    edit_tool.OnSetSelected = [&](bool value) {
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

    world_widget = widgets.createWidget<fw::CanvasWidget>(
        (float)WINDOW_WIDTH, (float)WINDOW_HEIGHT, WINDOW_WIDTH, WINDOW_HEIGHT
    );
    world_widget->setName("world_canvas");
    world_widget->setShader(&desat_shader);
    world_widget->OnBeforeRender = [&](sf::RenderTarget& target) {
        desat_shader.setUniform("texture", sf::Shader::CurrentTexture);
        desat_shader.setUniform("saturation", WORLD_SATURATION);
        desat_shader.setUniform("vcenter", WORLD_COLOR_SCALE_CENTER);
        desat_shader.setUniform("vpercent", WORLD_COLOR_SCALE_PERCENT);
    };
    world_widget->OnUpdate = [&]() {
        world_widget->setSize((float)window.getSize().x, (float)window.getSize().y);
        world_widget->setTextureSize(window.getSize().x, window.getSize().y);
    };

    selection_mask_widget = widgets.createWidget<fw::CanvasWidget>(
        (float)WINDOW_WIDTH, (float)WINDOW_HEIGHT, WINDOW_WIDTH, WINDOW_HEIGHT
    );
    selection_mask_widget->setName("selection_mask_canvas");
    selection_mask_widget->setShader(&selection_shader);
    selection_mask_widget->OnBeforeRender = [&](sf::RenderTarget& target) {
        selection_shader.setUniform("selection_mask", sf::Shader::CurrentTexture);
        selection_shader.setUniform("outline_color", SELECTION_OUTLINE_COLOR);
        selection_shader.setUniform("offset", SELECTION_OUTLINE_THICKNESS);
    };
    selection_mask_widget->OnUpdate = [&]() {
        selection_mask_widget->setSize((float)window.getSize().x, (float)window.getSize().y);
        selection_mask_widget->setTextureSize(window.getSize().x, window.getSize().y);
    };

    ui_widget = widgets.createWidget<fw::CanvasWidget>(
        (float)WINDOW_WIDTH, (float)WINDOW_HEIGHT, WINDOW_WIDTH, WINDOW_HEIGHT
    );
    ui_widget->setName("ui_canvas");
    ui_widget->OnUpdate = [&]() {
        ui_widget->setSize((float)window.getSize().x, (float)window.getSize().y);
        ui_widget->setTextureSize(window.getSize().x, window.getSize().y);
    };

    toolbox_widget = widgets.createWidget<Toolbox>(*this);
    edit_tool.edit_window_widget = widgets.createWidget<EditWindow>(300.0f, 400.0f, *this);
    create_tool.create_panel_widget = widgets.createWidget<CreatePanel>(*this);

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

    fw::ScrollAreaWidget* scroll_area_widget = widgets.createWidget<fw::ScrollAreaWidget>(300.0f, 200.0f);
    fw::ContainerWidget* container = widgets.createWidget<fw::ContainerWidget>(100.0f, 100.0f);
    fw::RectangleWidget* red_rect = widgets.createWidget<fw::RectangleWidget>(50.0f, 75.0f);
    fw::RectangleWidget* green_rect = widgets.createWidget<fw::RectangleWidget>(50.0f, 50.0f);
    fw::RectangleWidget* blue_rect = widgets.createWidget<fw::RectangleWidget>(600.0f, 50.0f);
    scroll_area_widget->setPosition(400.0f, 300.0f);
    scroll_area_widget->setWidget(container);
    container->setFillColor(sf::Color::White);
    container->setHorizontal(false);
    container->setPadding(10.0f);
    red_rect->setFillColor(sf::Color::Red);
    red_rect->setParent(container);
    green_rect->setFillColor(sf::Color::Green);
    green_rect->setParent(container);
    blue_rect->setFillColor(sf::Color::Blue);
    blue_rect->setParent(container);

    edit_tool.edit_window_widget->moveToTop();
}

void Editor::onProcessKeyboardEvent(const sf::Event& event) {
    if (event.type == sf::Event::KeyPressed) {
        switch (event.key.code) {
            case sf::Keyboard::Escape:
                if (selected_tool == &move_tool) {
                    endMove(false);
                    trySelectTool(move_tool.selected_tool);
                    endGesture();
                } else if (selected_tool == &rotate_tool) {
                    endRotate(false);
                    trySelectTool(rotate_tool.selected_tool);
                    endGesture();
                }
                break;
            case sf::Keyboard::Space: togglePause(); break;
            case sf::Keyboard::Num1: trySelectToolByIndex(0); break;
            case sf::Keyboard::Num2: trySelectToolByIndex(1); break;
            case sf::Keyboard::Num3: trySelectToolByIndex(2); break;
            case sf::Keyboard::Num4: trySelectToolByIndex(3); break;
            case sf::Keyboard::Num5: trySelectToolByIndex(4); break;
            case sf::Keyboard::Num6: trySelectToolByIndex(5); break;
            case sf::Keyboard::Num7: trySelectToolByIndex(6); break;
            case sf::Keyboard::Num8: trySelectToolByIndex(7); break;
            case sf::Keyboard::Num9: trySelectToolByIndex(8); break;
            case sf::Keyboard::Num0: trySelectToolByIndex(9); break;
            case sf::Keyboard::X:
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
                break;
            case sf::Keyboard::LControl: edit_tool.mode = EditTool::ADD; break;
            case sf::Keyboard::LAlt: edit_tool.mode = EditTool::INSERT; break;
            case sf::Keyboard::S:
                if (isLCtrlPressed()) {
                    saveToFile("levels/level.txt");
                }
                break;
            case sf::Keyboard::Z:
                if (isLCtrlPressed()) {
                    if (isLShiftPressed()) {
                        history.redo();
                    } else {
                        history.undo();
                    }
                }
                break;
            case sf::Keyboard::Q:
                quicksave();
                break;
            case sf::Keyboard::W:
                quickload_requested = true;
                break;
            case sf::Keyboard::A:
                if (selected_tool == &select_tool) {
                    if (isLAltPressed()) {
                        for (GameObject* obj : simulation.getAllVector()) {
                            select_tool.deselectObject(obj);
                        }
                    } else {
                        for (GameObject* obj : simulation.getAllVector()) {
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
                break;
            case sf::Keyboard::Tab:
                if (selected_tool == &edit_tool) {
                    trySelectTool(edit_tool.selected_tool);
                } else if (active_object) {
                    edit_tool.selected_tool = selected_tool;
                    trySelectTool(&edit_tool);
                }
                break;
            case sf::Keyboard::G:
                if (select_tool.selectedCount() > 0) {
                    Tool* s_tool = selected_tool;
                    trySelectTool(&move_tool);
                    grabSelected(s_tool);
                    startMoveGesture();
                }
                break;
            case sf::Keyboard::R:
                if (select_tool.selectedCount() > 0) {
                    Tool* s_tool = selected_tool;
                    trySelectTool(&rotate_tool);
                    rotateSelected(s_tool);
                    startMoveGesture();
                }
                break;
            case sf::Keyboard::D:
                if (isLShiftPressed()) {
                    if (selected_tool == &select_tool && select_tool.selectedCount() > 0) {
                        CompVector<GameObject*> old_objects = select_tool.getSelectedObjects();
                        CompVector<GameObject*> new_objects = simulation.duplicate(old_objects);
                        select_tool.setSelected(new_objects);
                        Tool* s_tool = selected_tool;
                        trySelectTool(&move_tool);
                        grabSelected(s_tool);
                        commit_action = true;
                        startMoveGesture();
                    }
                } else {
                    widgets.debug_render = !widgets.debug_render;
                }
                break;
            case sf::Keyboard::I:
                render_object_info = !render_object_info;
                break;
            case sf::Keyboard::B:
                debug_break = true;
                break;
            case sf::Keyboard::E:
                simulation.advance(timeStep);
                break;
        }
    }
    if (event.type == sf::Event::KeyReleased) {
        switch (event.key.code) {
            case sf::Keyboard::LControl:
                if (edit_tool.mode == EditTool::ADD) {
                    edit_tool.mode = EditTool::HOVER;
                }
                break;
            case sf::Keyboard::LAlt:
                if (edit_tool.mode == EditTool::INSERT) {
                    edit_tool.mode = EditTool::HOVER;
                }
                break;
        }
    }
}

void Editor::beforeProcessMouseEvent(const sf::Event& event) {
    sfMousePosWorld = pixelToWorld(getMousePos());
    b2MousePosWorld = tob2(sfMousePosWorld);
}

void Editor::onProcessMouseScroll(const sf::Event& event) {
    if (event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel) {
        zoomFactor *= pow(MOUSE_SCROLL_ZOOM, event.mouseWheelScroll.delta);
    }
}

void Editor::onProcessLeftPress() {
    if (selected_tool == &create_tool) {
        std::string id_string = std::to_string(simulation.getMaxId());
        switch (create_tool.type) {
            case CreateTool::BOX:
                simulation.createBox(
                    "box" + id_string, b2MousePosWorld, 0.0f, NEW_BOX_SIZE, NEW_BOX_COLOR
                );
                commit_action = true;
                break;
            case CreateTool::BALL:
                simulation.createBall(
                    "ball" + id_string, b2MousePosWorld, NEW_BALL_RADIUS, NEW_BALL_COLOR, NEW_BALL_NOTCH_COLOR
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
            mouse_joint_def.target = b2MousePosWorld;
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
                edit_tool.grabbed_vertex_offset = vertex.pos - active_object->toLocal(b2MousePosWorld);
                bool shift = isLShiftPressed();
                if (!vertex.selected && !shift) {
                    active_object->deselectAllVertices();
                } else if (shift) {
                    active_object->selectVertex(edit_tool.grabbed_vertex);
                }
            } else {
                edit_tool.mode = EditTool::SELECT;
                edit_tool.rectangle_select.active = true;
                edit_tool.rectangle_select.select_origin = sfMousePosWorld;
                if (!isLShiftPressed()) {
                    active_object->deselectAllVertices();
                }
            }
        } else if (edit_tool.mode == EditTool::ADD && edit_tool.edge_vertex != -1) {
            if (edit_tool.edge_vertex == 0) {
                active_object->addVertexGlobal(0, b2MousePosWorld);
            } else if (edit_tool.edge_vertex > 0) {
                active_object->addVertexGlobal(edit_tool.edge_vertex + 1, b2MousePosWorld);
            }
            commit_action = true;
        } else if (edit_tool.mode == EditTool::INSERT && edit_tool.highlighted_edge != -1) {
            active_object->addVertexGlobal(edit_tool.highlighted_edge + 1, edit_tool.insertVertexPos);
            commit_action = true;
        }
    }
}

void Editor::onProcessLeftRelease() {
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
    } else if (selected_tool == &move_tool) {
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

void Editor::onProcessMouse() {
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
                edit_tool.insertVertexPos = utils::line_project(b2MousePosWorld, v1, v2);
            }
        }
    } else if (selected_tool == &move_tool) {
        for (GameObject* obj : move_tool.moving_objects) {
            obj->setGlobalPosition(b2MousePosWorld + obj->cursor_offset);
        }
    } else if (selected_tool == &rotate_tool) {
        for (size_t i = 0; i < rotate_tool.rotating_objects.size(); i++) {
            GameObject* obj = rotate_tool.rotating_objects[i];
            b2Vec2 mouse_vector = b2MousePosWorld - rotate_tool.pivot_pos;
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
    if (leftButtonPressed) {
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
                drag_tool.mouse_joint->SetTarget(b2MousePosWorld);
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
                    b2Vec2 offset = active_object->toLocal(b2MousePosWorld) + edit_tool.grabbed_vertex_offset - vertex.orig_pos;
                    active_object->offsetVertex(index, offset, false);
                    active_object->offsetSelected(offset, false);
                    active_object->syncVertices();
                }
            }
        }
    }
    if (rightButtonPressed) {
        sf::Vector2i mouseDelta = mousePrevPos - getMousePos();
        viewCenterX += mouseDelta.x / zoomFactor;
        viewCenterY += -mouseDelta.y / zoomFactor;
    }
}

void Editor::afterProcessInput() {
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
        loadFromFile(load_request.filename);
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
    renderWorld();
    renderUi();
}

void Editor::renderWorld() {
    world_widget->clear(sf::Color::Transparent);
    world_widget->setViewCenter(viewCenterX, viewCenterY);
    world_widget->setViewSize(world_widget->getSize().x / zoomFactor, -1.0f * world_widget->getSize().y / zoomFactor);
    for (size_t i = 0; i < simulation.getTopSize(); i++) {
        GameObject* gameobject = simulation.getFromTop(i);
        gameobject->draw_varray = selected_tool == &edit_tool && gameobject == active_object;
        gameobject->render(world_widget->getRenderTexture());
    }
    world_widget->display();

    selection_mask_widget->clear();
    selection_mask_widget->setView(world_widget->getView());
    if (selected_tool != &edit_tool) {
        for (auto obj : select_tool.getSelectedObjects()) {
            obj->renderMask(selection_mask_widget->getRenderTexture());
        }
    }
    selection_mask_widget->display();
}

void Editor::renderUi() {
    ui_widget->clear(sf::Color::Transparent);
    ui_widget->resetView();
    sf::RenderTarget& target = ui_widget->getRenderTexture();

    if (render_object_info) {
        // parent relation lines
        for (GameObject* object : simulation.getAllVector()) {
            for (GameObject* child : object->getChildren()) {
                sf::Vector2f v1 = worldToScreen(object->getGlobalPosition());
                sf::Vector2f v2 = worldToScreen(child->getGlobalPosition());
                fw::draw_line(target, v1, v2, sf::Color(128, 128, 128));
            }
        }
        // object origin circles
        for (size_t i = 0; i < simulation.getAllSize(); i++) {
            GameObject* gameobject = simulation.getFromAll(i);
            sf::Color circle_color = gameobject == active_object ? sf::Color(255, 255, 0) : sf::Color(255, 159, 44);
            origin_shape.setFillColor(circle_color);
            origin_shape.setPosition(worldToScreen(gameobject->getGlobalPosition()));
            target.draw(origin_shape);
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
                target.draw(object_info_text);
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
            renderRectangleSelect(target, select_tool.rectangle_select);
        }
    } else if (selected_tool == &drag_tool) {
        if (drag_tool.mouse_joint) {
            sf::Vector2f grabbed_point = worldToScreen(drag_tool.mouse_joint->GetAnchorB());
            fw::draw_line(target, grabbed_point, getMousePosf(), sf::Color::Yellow);
        }
    } else if (selected_tool == &rotate_tool) {
        for (size_t i = 0; i < rotate_tool.rotating_objects.size(); i++) {
            fw::draw_line(target, worldToScreen(rotate_tool.pivot_pos), getMousePosf(), sf::Color::Yellow);
        }
    } else if (selected_tool == &edit_tool && active_object) {
        if (edit_tool.mode == EditTool::ADD && edit_tool.edge_vertex != -1) {
            // ghost edge
            sf::Vector2i v1 = worldToPixel(active_object->getGlobalVertexPos(edit_tool.edge_vertex));
            sf::Vector2i v2 = getMousePos();
            fw::draw_line(target, to2f(v1), to2f(v2), sf::Color(255, 255, 255, 128));
            // ghost edge normal
            sf::Vector2f norm_v1, norm_v2;
            if (edit_tool.edge_vertex == 0) {
                getScreenNormal(v1, v2, norm_v1, norm_v2);
            } else {
                getScreenNormal(v2, v1, norm_v1, norm_v2);
            }
            fw::draw_line(target, norm_v1, norm_v2, sf::Color(0, 255, 255, 128));
            // ghost vertex
            sf::Vector2f ghost_vertex_pos = to2f(getMousePos());
            edit_tool.vertex_rect.setPosition(ghost_vertex_pos);
            edit_tool.vertex_rect.setFillColor(sf::Color(255, 0, 0, 128));
            target.draw(edit_tool.vertex_rect);
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
            target.draw(edit_tool.edge_highlight);
            // ghost vertex on the edge
            sf::Vector2f ghost_vertex_pos = worldToScreen(edit_tool.insertVertexPos);
            edit_tool.vertex_rect.setFillColor(sf::Color(255, 0, 0, 128));
            edit_tool.vertex_rect.setPosition(ghost_vertex_pos);
            target.draw(edit_tool.vertex_rect);
        }
        // vertices
        for (size_t i = 0; i < active_object->getVertexCount(); i++) {
            edit_tool.vertex_rect.setPosition(worldToScreen(active_object->getGlobalVertexPos(i)));
            bool selected = active_object->isVertexSelected(i);
            sf::Color vertex_color = selected ? sf::Color(255, 255, 0) : sf::Color(255, 0, 0);
            edit_tool.vertex_rect.setFillColor(vertex_color);
            target.draw(edit_tool.vertex_rect);
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
            fw::draw_line(target, norm_v1, norm_v2, sf::Color(0, 255, 255));
        }
        if (edit_tool.mode == EditTool::HOVER && edit_tool.highlighted_vertex != -1) {
            // highlighted vertex
            sf::Vector2f vertex_pos = worldToScreen(active_object->getGlobalVertexPos(edit_tool.highlighted_vertex));
            edit_tool.vertex_highlight_rect.setPosition(vertex_pos);
            target.draw(edit_tool.vertex_highlight_rect);
        } else if (edit_tool.mode == EditTool::SELECT && edit_tool.rectangle_select.active) {
            //selection box
            renderRectangleSelect(target, edit_tool.rectangle_select);
        }
    }

    ui_widget->display();
}

std::string Editor::serialize() const {
    LoggerTag tag_serialize("serialize");
    logger << __FUNCTION__"\n";
    LoggerIndent serialize_indent;
    TokenWriter tw;
    tw << "camera\n";
    {
        TokenWriterIndent camera_indent(tw);
        tw << "center" << viewCenterX << viewCenterY << "\n";
        tw << "zoom" << zoomFactor << "\n";
    }
    tw << "/camera";
    tw << "\n\n";
    simulation.serialize(tw);
    return tw.toStr();
}

void Editor::deserialize(const std::string& str, bool set_camera) {
    ptrdiff_t active_object_id = -1;
    if (active_object) {
        active_object_id = active_object->getId();
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
                    viewCenterX = params.x;
                    viewCenterY = params.y;
                    zoomFactor = params.zoom;
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
    } catch (std::exception exc) {
        throw std::runtime_error(__FUNCTION__": Line " + std::to_string(tr.getLine(-1)) + ": " + exc.what());
    }
}

void Editor::saveToFile(const std::string& filename) const {
    LoggerTag tag_saveload("saveload");
    std::string str = serialize();
    utils::str_to_file(str, filename);
    logger << "Saved to " << filename << "\n";
}

void Editor::loadAction(const std::string& filename) {
    load_request.requested = true;
    load_request.filename = filename;
}

void Editor::loadFromFile(const std::string& filename) {
    LoggerTag tag_saveload("saveload");
    try {
        std::string str = utils::file_to_str(filename);
        deserialize(str, true);
        logger << "Editor loaded from " << filename << "\n";
    } catch (std::exception exc) {
        throw std::runtime_error(__FUNCTION__": " + filename + ": " + std::string(exc.what()));
    }
}

void Editor::quicksave() {
    LoggerTag tag_saveload("saveload");
    quicksave_str = serialize();
    logger << "Quicksave\n";
}

void Editor::quickload() {
    LoggerTag tag_saveload("saveload");
    if (quicksave_str.size() > 0) {
        deserialize(quicksave_str, true);
        logger << "Quickload\n";
    } else {
        logger << "Can't quickload\n";
    }
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
    return to2i(worldToScreen(world_pos));
}

sf::Vector2i Editor::worldToPixel(const b2Vec2& world_pos) const {
    return to2i(worldToScreen(tosf(world_pos)));
}

sf::Vector2f Editor::worldDirToScreenf(const b2Vec2& world_dir) const {
    //TODO: calculate new direction using an actual sf::View
    return sf::Vector2f(world_dir.x, -world_dir.y);
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
            utils::left_side(b2MousePosWorld, p1, side1_p)
            && utils::left_side(b2MousePosWorld, p2, side2_p);
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
            utils::left_side(b2MousePosWorld, p1, side1_p)
            && utils::left_side(b2MousePosWorld, p2, side2_p);
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
    sf::Vector2f mpos = sfMousePosWorld;
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
    float lower_x = std::min(rectangle_select.select_origin.x, sfMousePosWorld.x);
    float lower_y = std::min(rectangle_select.select_origin.y, sfMousePosWorld.y);
    float upper_x = std::max(rectangle_select.select_origin.x, sfMousePosWorld.x);
    float upper_y = std::max(rectangle_select.select_origin.y, sfMousePosWorld.y);
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
    move_tool.orig_cursor_pos = b2MousePosWorld;
    move_tool.moving_objects = CompVector<GameObject*>();
    move_tool.selected_tool = selected_tool;
    for (GameObject* obj : select_tool.getSelectedObjects()) {
        if (isParentSelected(obj)) {
            continue;
        }
        move_tool.moving_objects.add(obj);
        obj->orig_pos = obj->getGlobalPosition();
        obj->cursor_offset = obj->getGlobalPosition() - b2MousePosWorld;
        obj->was_enabled = obj->getRigidBody()->IsEnabled();
        obj->setEnabled(false, true);
        obj->setLinearVelocity(b2Vec2(0.0f, 0.0f), true);
        obj->setAngularVelocity(0.0f, true);
    }
}

void Editor::rotateSelected(Tool* selected_tool) {
    rotate_tool.orig_cursor_pos = b2MousePosWorld;
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
    b2Vec2 mouse_vector = b2MousePosWorld - avg;
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
    select_tool.deselectObject(object);
    simulation.remove(object, remove_children);
}

void Editor::checkDebugbreak() {
    if (debug_break) {
        __debugbreak();
        debug_break = false;
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
