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
const auto to2f = fw::to2f;

bool QueryCallback::ReportFixture(b2Fixture* fixture) {
    fixtures.push_back(fixture);
    return true;
}

void Editor::init() {
    fw::Application::init(WINDOW_TITLE, WINDOW_WIDTH, WINDOW_HEIGHT, ANTIALIASING);
}

void Editor::load(const std::string& filename) {
    load_from_file(filename);
}

void Editor::setCameraPos(float x, float y) {
    viewCenterX = x;
    viewCenterY = y;
}

void Editor::setCameraZoom(float zoom) {
    zoomFactor = zoom;
}

GameObjectList& Editor::getObjectList() {
    return game_objects;
}

fw::WidgetList& Editor::getWidgetList() {
    return widgets;
}

void Editor::selectSingleObject(GameObject* object, bool with_children) {
    select_tool.selectSingleObject(object, with_children);
}

BoxObject* Editor::create_box(
    const std::string& name,
    const b2Vec2& pos,
    float angle,
    const b2Vec2& size,
    const sf::Color& color
) {
    b2BodyDef def;
    def.type = b2_dynamicBody;
    def.position = pos;
    def.angle = angle;
    std::unique_ptr<BoxObject> uptr = std::make_unique<BoxObject>(
        &game_objects, def, size, color
    );
    BoxObject* ptr = uptr.get();
    ptr->setName(name);
    game_objects.add(std::move(uptr), true);
    return ptr;
}

BallObject* Editor::create_ball(
    const std::string& name,
    const b2Vec2& pos,
    float radius,
    const sf::Color& color,
    const sf::Color& notch_color
) {
    b2BodyDef def;
    def.type = b2_dynamicBody;
    def.position = pos;
    std::unique_ptr<BallObject> uptr = std::make_unique<BallObject>(
        &game_objects, def, radius, color, notch_color
    );
    BallObject* ptr = uptr.get();
    ptr->setName(name);
    game_objects.add(std::move(uptr), true);
    return ptr;
}

PolygonObject* Editor::create_polygon(
    const std::string& name,
    const b2Vec2& pos,
    float angle,
    const std::vector<b2Vec2>& vertices,
    const sf::Color& color
) {
    b2BodyDef def;
    def.type = b2_dynamicBody;
    def.position = pos;
    def.angle = angle;
    std::unique_ptr<PolygonObject> uptr = std::make_unique<PolygonObject>(
        &game_objects, def, vertices, color
    );
    PolygonObject* ptr = uptr.get();
    ptr->setName(name);
    game_objects.add(std::move(uptr), true);
    return ptr;
}

PolygonObject* Editor::create_car(
    const std::string& name,
    const b2Vec2& pos,
    const std::vector<float>& lengths,
    const std::vector<float>& wheels,
    const sf::Color& color
) {
    b2BodyDef def;
    def.type = b2_dynamicBody;
    def.position = pos;
    std::vector<b2Vec2> vertices;
    for (size_t i = 0; i < lengths.size(); i++) {
        b2Vec2 pos = utils::get_pos<b2Vec2>(lengths, i);
        vertices.push_back(pos);
    }
    std::unique_ptr<PolygonObject> uptr = std::make_unique<PolygonObject>(
        &game_objects, def, vertices, color
    );
    PolygonObject* car_ptr = uptr.get();
    car_ptr->setName(name);
    game_objects.add(std::move(uptr), true);
    size_t wheel_count = 0;

    for (size_t i = 0; i < wheels.size(); i++) {
        if (wheels[i] == 0.0f) {
            continue;
        }
        b2Vec2 wheel_pos = utils::get_pos<b2Vec2>(lengths, i);
        b2Vec2 anchor_pos = wheel_pos;
        b2Vec2 anchor_pos_world = car_ptr->rigid_body->GetPosition() + anchor_pos;
        float radius = wheels[i];
        b2BodyDef wheel_body_def;
        {
            wheel_body_def.type = b2_dynamicBody;
            wheel_body_def.position = anchor_pos_world;
        }
        std::unique_ptr<BallObject> wheel = std::make_unique<BallObject>(
            &game_objects, wheel_body_def, radius, sf::Color(255, 255, 0), sf::Color(64, 64, 0)
        );
        BallObject* wheel_ptr = wheel.get();
        {
            wheel_ptr->setDensity(1.0f, false);
            wheel_ptr->setFriction(0.3f, false);
            wheel_ptr->setRestitution(0.5f, false);
            std::string wheel_name = car_ptr->getName() + " wheel" + std::to_string(wheel_count);
            wheel_ptr->setName(wheel_name);
            wheel_ptr->setParent(car_ptr);
            game_objects.add(std::move(wheel), true);
        }
        b2RevoluteJointDef wheel_joint_def;
        {
            wheel_joint_def.Initialize(car_ptr->rigid_body, wheel_ptr->rigid_body, anchor_pos_world);
            wheel_joint_def.maxMotorTorque = 30.0f;
            wheel_joint_def.motorSpeed = -10.0f;
            wheel_joint_def.enableMotor = true;
            std::unique_ptr<RevoluteJoint> joint = std::make_unique<RevoluteJoint>(
                wheel_joint_def, world.get(), car_ptr, wheel_ptr
            );
            game_objects.addJoint(std::move(joint));
        }
        wheel_count++;
    }

    return car_ptr;
}

ChainObject* Editor::create_chain(
    const std::string& name,
    const b2Vec2& pos,
    float angle,
    const std::vector<b2Vec2>& vertices,
    const sf::Color& color
) {
    b2BodyDef def;
    def.position = pos;
    def.angle = angle;
    std::unique_ptr<ChainObject> uptr = std::make_unique<ChainObject>(
        &game_objects, def, vertices, color
    );
    ChainObject* ptr = uptr.get();
    ptr->setName(name);
    game_objects.add(std::move(uptr), true);
    return ptr;
}

void Editor::onInit() {
    sf::ContextSettings cs_world;
    world_texture.create(WINDOW_WIDTH, WINDOW_HEIGHT, cs_world);
    sf::ContextSettings cs_ui;
    ui_texture.create(WINDOW_WIDTH, WINDOW_HEIGHT, cs_ui);
    sf::ContextSettings cs_mask;
    selection_mask.create(WINDOW_WIDTH, WINDOW_HEIGHT, cs_mask);
    window_view = sf::View(sf::FloatRect(0.0f, 0.0f, WINDOW_WIDTH, WINDOW_HEIGHT));
    world_view = sf::View(sf::FloatRect(0.0f, 0.0f, WINDOW_WIDTH, WINDOW_HEIGHT));
    ui_view = sf::View(sf::FloatRect(0.0f, 0.0f, WINDOW_WIDTH, WINDOW_HEIGHT));
    if (!desat_shader.loadFromFile("shaders/desat.frag", sf::Shader::Fragment)) {
        throw std::runtime_error("Shader loading error");
    }
    if (!selection_shader.loadFromFile("shaders/selection.frag", sf::Shader::Fragment)) {
        throw std::runtime_error("Shader loading error");
    }
    init_tools();
    init_world();
    init_ui();
    logger.OnLineWrite = [&](std::string line) { // should be after init_ui and preferably before any logging
        logger_text_widget->setString(line);
    };
    try_select_tool_by_index(0); // should be after init_tools and init_ui
    select_create_type(0);
    maximize_window();
    auto getter = [&]() { return serialize(); };
    auto setter = [&](std::string str) { deserialize(str, false); };
    history = History<std::string>("Editor", getter, setter);

    assert(tools.size() > 0);
    assert(selected_tool);
    for (size_t i = 0; i < tools_in_tool_panel.size(); i++) {
        assert(tools_in_tool_panel[i]->widget);
    }
    assert(game_objects.world);
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
    } else if (event.type == sf::Event::Resized) {
        sf::ContextSettings cs_world;
        world_texture.create(event.size.width, event.size.height, cs_world);
        sf::ContextSettings cs_ui;
        ui_texture.create(event.size.width, event.size.height, cs_ui);
        sf::ContextSettings cs_mask;
        selection_mask.create(event.size.width, event.size.height, cs_mask);
    }
}

void Editor::init_tools() {
    for (size_t i = 0; i < tools.size(); i++) {
        tools[i]->reset();
    }
    create_tool.OnSetSelected = [&](bool value) {
        create_tool.create_panel_widget->setVisible(value);
    };
    edit_tool.OnSetSelected = [&](bool value) {
        if (value) {
            assert(active_object);
            edit_tool.edit_window_widget->updateParameters();
        }
        edit_tool.edit_window_widget->setVisible(value);
    };
}

void Editor::init_world() {
    b2Vec2 gravity(0.0f, -9.8f);
    world = std::make_unique<b2World>(gravity);
    b2BodyDef mouse_body_def;
    drag_tool.mouse_body = world->CreateBody(&mouse_body_def);
    game_objects.world = world.get();
}

void Editor::init_ui() {
    load_font(ui_font, "fonts/STAN0757.TTF");
    load_font(fps_font, "fonts/fraps.ttf");
    load_font(console_font, "fonts/courbd.ttf");
    load_font(small_font, "fonts/HelvetiPixel.ttf");
    load_font(textbox_font, "fonts/verdana.ttf");

    arrow_cursor.loadFromSystem(sf::Cursor::Arrow);
    text_cursor.loadFromSystem(sf::Cursor::Text);
    window.setMouseCursor(arrow_cursor);

    vertex_text.setFont(ui_font);
    vertex_text.setCharacterSize(20);
    vertex_text.setFillColor(sf::Color::White);

    origin_shape.setRadius(3.0f);
    origin_shape.setOrigin(origin_shape.getRadius(), origin_shape.getRadius());
    origin_shape.setOutlineColor(sf::Color::Black);
    origin_shape.setOutlineThickness(1.0f);

    object_info_text.setFont(small_font);
    object_info_text.setCharacterSize(16);
    object_info_text.setFillColor(sf::Color::White);

    init_widgets();
}

void Editor::init_widgets() {
    toolbox_widget = widgets.createWidget<Toolbox>(*this);
    edit_tool.edit_window_widget = widgets.createWidget<EditWindow>(*this);
    create_tool.create_panel_widget = widgets.createWidget<CreatePanel>(*this);

    // pause widget
    paused_rect_widget = widgets.createWidget<fw::ContainerWidget>();
    paused_rect_widget->setFillColor(sf::Color(0, 0, 0, 128));
    paused_rect_widget->setOrigin(fw::Widget::TOP_LEFT);
    paused_rect_widget->setPadding(10.0f);
    paused_rect_widget->setName("paused rect");
    fw::TextWidget* paused_text_widget = widgets.createWidget<fw::TextWidget>();
    paused_text_widget->setFont(ui_font);
    paused_text_widget->setString("PAUSED");
    paused_text_widget->setCharacterSize(24);
    paused_text_widget->setFillColor(sf::Color::Yellow);
    paused_text_widget->setOrigin(fw::Widget::TOP_LEFT);
    paused_text_widget->setParent(paused_rect_widget);

    // fps
    fps_widget = widgets.createWidget<fw::ContainerWidget>();
    fps_widget->setFillColor(sf::Color::Yellow);
    fps_widget->setOrigin(fw::Widget::TOP_LEFT);
    fps_widget->setPosition(120.0f, 0.0f);
    fps_widget->setPadding(0.0f);
    fps_widget->setName("fps");
    fps_text_widget = widgets.createWidget<fw::TextWidget>();
    fps_text_widget->setFont(fps_font);
    fps_text_widget->setCharacterSize(32);
    fps_text_widget->setFillColor(sf::Color::Black);
    fps_text_widget->setOrigin(fw::Widget::TOP_LEFT);
    fps_text_widget->setAdjustLocalBounds(false);
    fps_text_widget->setParent(fps_widget);

    // logger
    logger_widget = widgets.createWidget<fw::RectangleWidget>();
    logger_widget->setFillColor(sf::Color(0, 0, 0));
    logger_widget->setSize(sf::Vector2f(500.0f, 20.0f));
    logger_widget->setOrigin(fw::Widget::BOTTOM_LEFT);
    logger_widget->setParentAnchor(fw::Widget::BOTTOM_LEFT);
    logger_widget->setClipChildren(true);
    logger_widget->setName("logger");
    logger_text_widget = widgets.createWidget<fw::TextWidget>();
    logger_text_widget->setFont(console_font);
    logger_text_widget->setCharacterSize(15);
    logger_text_widget->setFillColor(sf::Color::White);
    logger_text_widget->setOrigin(fw::Widget::TOP_LEFT);
    logger_text_widget->setParentAnchor(fw::Widget::TOP_LEFT);
    logger_text_widget->setString("Logger message");
    logger_text_widget->setParent(logger_widget);

    //RectangleWidget* rect = widgets.createWidget<RectangleWidget>();
    //rect->setFillColor(sf::Color::Black);
    //rect->setSize(sf::Vector2f(100.0f, 100.0f));
    //rect->setPosition(0.0f, 0.5f);
    //rect->setName("black rect");

    //TextWidget* text = widgets.createWidget<TextWidget>();
    //text->setCharacterSize(15);
    //text->setFont(textbox_font);
    //text->setString("Text");
    //text->setParentAnchor(Widget::CENTER_LEFT);
    //text->setOrigin(0.0f, 7.0f);
    //text->setAnchorOffset(2.0f, 0.0f);
    //text->setName("test text");
    ////text->setParent(rect);
    //Widget* textbox = widgets.find("textbox");
    //text->setParent(textbox);

}

void Editor::onProcessKeyboardEvent(const sf::Event& event) {
    if (event.type == sf::Event::KeyPressed) {
        switch (event.key.code) {
            case sf::Keyboard::Escape:
                if (selected_tool == &move_tool) {
                    for (GameObject* obj : move_tool.moving_objects) {
                        obj->setGlobalPosition(obj->orig_pos);
                        obj->setEnabled(obj->was_enabled, true);
                    }
                    try_select_tool(&select_tool);
                } else if (selected_tool == &rotate_tool) {
                    for (GameObject* obj : rotate_tool.rotating_objects) {
                        obj->setGlobalPosition(obj->orig_pos);
                        obj->setGlobalAngle(obj->orig_angle);
                        obj->setEnabled(obj->was_enabled, true);
                    }
                    try_select_tool(&select_tool);
                }
                break;
            case sf::Keyboard::Space: toggle_pause(); break;
            case sf::Keyboard::Num1: try_select_tool_by_index(0); break;
            case sf::Keyboard::Num2: try_select_tool_by_index(1); break;
            case sf::Keyboard::Num3: try_select_tool_by_index(2); break;
            case sf::Keyboard::Num4: try_select_tool_by_index(3); break;
            case sf::Keyboard::Num5: try_select_tool_by_index(4); break;
            case sf::Keyboard::Num6: try_select_tool_by_index(5); break;
            case sf::Keyboard::Num7: try_select_tool_by_index(6); break;
            case sf::Keyboard::Num8: try_select_tool_by_index(7); break;
            case sf::Keyboard::Num9: try_select_tool_by_index(8); break;
            case sf::Keyboard::Num0: try_select_tool_by_index(9); break;
            case sf::Keyboard::X:
                if (selected_tool == &select_tool) {
                    CompVector<GameObject*> selected_copy = select_tool.getSelectedObjects();
                    for (GameObject* obj : selected_copy | std::views::reverse) {
                        delete_object(obj, false);
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
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)) {
                    save_to_file("levels/level.txt");
                }
                break;
            case sf::Keyboard::Z:
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)) {
                    if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) {
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
                    if (sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt)) {
                        for (GameObject* obj : game_objects.getAllVector()) {
                            select_tool.deselectObject(obj);
                        }
                    } else {
                        for (GameObject* obj : game_objects.getAllVector()) {
                            select_tool.selectObject(obj);
                        }
                    }
                } else if (selected_tool == &edit_tool && active_object) {
                    if (sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt)) {
                        active_object->deselectAllVertices();
                    } else {
                        active_object->selectAllVertices();
                    }
                }
                break;
            case sf::Keyboard::Tab:
                if (selected_tool == &edit_tool) {
                    try_select_tool(&select_tool);
                } else if (selected_tool == &select_tool && active_object) {
                    try_select_tool(&edit_tool);
                }
                break;
            case sf::Keyboard::G:
                if (selected_tool == &select_tool && select_tool.selectedCount() > 0) {
                    try_select_tool(&move_tool);
                    grab_selected();
                    leftButtonProcessWidgetsOnPress = false;
                }
                break;
            case sf::Keyboard::R:
                if (selected_tool == &select_tool && select_tool.selectedCount() > 0) {
                    try_select_tool(&rotate_tool);
                    rotate_selected();
                }
                break;
            case sf::Keyboard::D:
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) {
                    if (selected_tool == &select_tool && select_tool.selectedCount() > 0) {
                        CompVector<GameObject*> old_objects = select_tool.getSelectedObjects();
                        CompVector<GameObject*> new_objects = game_objects.duplicate(old_objects);
                        select_tool.setSelected(new_objects);
                        try_select_tool(&move_tool);
                        grab_selected();
                        commit_action = true;
                        leftButtonProcessWidgetsOnPress = false;
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
    sfMousePosWorld = pixel_to_world(mousePos);
    b2MousePosWorld = tob2(sfMousePosWorld);
}

void Editor::onProcessMouseScroll(const sf::Event& event) {
    if (event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel) {
        zoomFactor *= pow(MOUSE_SCROLL_ZOOM, event.mouseWheelScroll.delta);
    }
}

void Editor::onProcessLeftClick() {
    if (selected_tool == &select_tool) {
        leftButtonProcessWidgetsOnRelease = false;
    } else if (selected_tool == &create_tool) {
        std::string id_string = std::to_string(game_objects.getMaxId());
        switch (create_tool.type) {
            case CreateTool::BOX:
                create_box(
                    "box" + id_string, b2MousePosWorld, 0.0f, NEW_BOX_SIZE, NEW_BOX_COLOR
                );
                commit_action = true;
                break;
            case CreateTool::BALL:
                create_ball(
                    "ball" + id_string, b2MousePosWorld, NEW_BALL_RADIUS, NEW_BALL_COLOR, NEW_BALL_NOTCH_COLOR
                );
                commit_action = true;
                break;
        }
    } else if (selected_tool == &drag_tool) {
        b2Fixture* grabbed_fixture = get_fixture_at(mousePosf);
        if (grabbed_fixture) {
            b2Body* grabbed_body = grabbed_fixture->GetBody();
            b2MouseJointDef mouse_joint_def;
            mouse_joint_def.bodyA = drag_tool.mouse_body;
            mouse_joint_def.bodyB = grabbed_body;
            mouse_joint_def.damping = 1.0f;
            mouse_joint_def.maxForce = 5000.0f * grabbed_body->GetMass();
            mouse_joint_def.stiffness = 50.0f;
            mouse_joint_def.target = b2MousePosWorld;
            drag_tool.mouse_joint = (b2MouseJoint*)world->CreateJoint(&mouse_joint_def);
            leftButtonProcessWidgetsOnRelease = false;
        }
    } else if (selected_tool == &move_tool) {
        for (GameObject* obj : move_tool.moving_objects) {
            //TODO: remember state for all children
            obj->setEnabled(obj->was_enabled, true);
            commit_action = true;
            leftButtonPressGesture = false;
        }
        try_select_tool(&select_tool);
    } else if (selected_tool == &rotate_tool) {
        for (GameObject* obj : rotate_tool.rotating_objects) {
            //TODO: remember state for all children
            obj->setEnabled(obj->was_enabled, true);
            commit_action = true;
            leftButtonPressGesture = false;
        }
        try_select_tool(&select_tool);
    } else if (selected_tool == &edit_tool && active_object) {
        if (edit_tool.mode == EditTool::HOVER) {
            if (edit_tool.highlighted_vertex != -1) {
                edit_tool.mode = EditTool::MOVE;
                edit_tool.grabbed_vertex = edit_tool.highlighted_vertex;
                const EditableVertex& vertex = active_object->getVertex(edit_tool.grabbed_vertex);
                edit_tool.grabbed_vertex_offset = vertex.pos - active_object->toLocal(b2MousePosWorld);
                bool shift = sf::Keyboard::isKeyPressed(sf::Keyboard::LShift);
                if (!vertex.selected && !shift) {
                    active_object->deselectAllVertices();
                } else if (shift) {
                    active_object->selectVertex(edit_tool.grabbed_vertex);
                }
            } else {
                edit_tool.mode = EditTool::SELECT;
                edit_tool.rectangle_select.active = true;
                edit_tool.rectangle_select.select_origin = sfMousePosWorld;
                if (!sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) {
                    active_object->deselectAllVertices();
                }
                leftButtonProcessWidgetsOnRelease = false;
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
        if (utils::length(mousePosf - mousePressPosf) < MOUSE_DRAG_THRESHOLD) {
            GameObject* object = get_object_at(mousePosf);
            setActiveObject(object);
            bool with_children = sf::Keyboard::isKeyPressed(sf::Keyboard::LControl);
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) {
                select_tool.toggleSelect(object, with_children);
            } else {
                select_tool.selectSingleObject(object, with_children);
            }
        }
    }
    if (select_tool.rectangle_select.active) {
        select_tool.rectangle_select.active = false;
        select_tool.applyRectSelection();
    }
    if (drag_tool.mouse_joint) {
        world->DestroyJoint(drag_tool.mouse_joint);
        drag_tool.mouse_joint = nullptr;
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
            GameObject* new_hover = get_object_at(mousePosf);
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
            edit_tool.highlighted_vertex = mouse_get_object_vertex();
        } else if (edit_tool.mode == EditTool::ADD) {
            edit_tool.edge_vertex = mouse_get_edge_vertex();
        } else if (edit_tool.mode == EditTool::INSERT) {
            edit_tool.highlighted_edge = mouse_get_object_edge();
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
            b2Vec2 new_pos = utils::rotate_point(obj->orig_pos, rotate_tool.pivot_pos, offset);
            obj->setGlobalPosition(new_pos);
            obj->setGlobalAngle(obj->orig_angle + offset);
        }
    }
    if (leftButtonPressed) {
        if (selected_tool == &select_tool) {
            if (select_tool.rectangle_select.active) {
                select_objects_in_rect(select_tool.rectangle_select);
            } else if (utils::length(mousePosf - mousePressPosf) >= MOUSE_DRAG_THRESHOLD) {
                select_tool.rectangle_select.active = true;
                select_tool.rectangle_select.select_origin = screen_to_world(mousePressPosf);
                if (!sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) {
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
                    select_vertices_in_rect(edit_tool.rectangle_select);
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
        sf::Vector2i mouseDelta = mousePrevPos - mousePos;
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
        load_from_file(load_request.filename);
        history.save("Load");
        load_request.requested = false;
    }
}

void Editor::onProcessWorld() {
    if (!paused) {
        world->Step(timeStep, VELOCITY_ITERATIONS, POSITION_ITERATIONS);
        game_objects.transformFromRigidbody();
    }
}

void Editor::onRender() {
    render_world();
    render_ui();
}

void Editor::render_world() {
    world_texture.clear(sf::Color(0, 0, 0));
    world_view.setCenter(viewCenterX, viewCenterY);
    world_view.setSize(world_texture.getSize().x / zoomFactor, -1.0f * world_texture.getSize().y / zoomFactor);
    world_texture.setView(world_view);

    selection_mask.clear();
    selection_mask.setView(world_view);

    for (size_t i = 0; i < game_objects.getTopSize(); i++) {
        GameObject* gameobject = game_objects.getFromTop(i);
        gameobject->draw_varray = selected_tool == &edit_tool && gameobject == active_object;
        gameobject->render(world_texture);
    }

    if (selected_tool != &edit_tool) {
        for (auto obj : select_tool.getSelectedObjects()) {
            obj->renderMask(selection_mask);
        }
    }

    world_texture.display();
    sf::Sprite world_sprite(world_texture.getTexture());
    desat_shader.setUniform("texture", sf::Shader::CurrentTexture);
    desat_shader.setUniform("saturation", WORLD_SATURATION);
    desat_shader.setUniform("vcenter", WORLD_COLOR_SCALE_CENTER);
    desat_shader.setUniform("vpercent", WORLD_COLOR_SCALE_PERCENT);
    window.draw(world_sprite, &desat_shader);

    selection_mask.display();
    sf::Sprite selection_sprite(selection_mask.getTexture());
    selection_shader.setUniform("selection_mask", sf::Shader::CurrentTexture);
    selection_shader.setUniform("outline_color", SELECTION_OUTLINE_COLOR);
    selection_shader.setUniform("offset", SELECTION_OUTLINE_THICKNESS);
    window.draw(selection_sprite, &selection_shader);
}

void Editor::render_ui() {
    ui_texture.clear(sf::Color(0, 0, 0, 0));
    ui_view.setCenter(ui_texture.getSize().x / 2.0f, ui_texture.getSize().y / 2.0f);
    ui_view.setSize(ui_texture.getSize().x, ui_texture.getSize().y);
    ui_texture.setView(ui_view);
    sf::RenderTarget& target = ui_texture;

    if (render_object_info) {
        // parent relation lines
        for (GameObject* object : game_objects.getAllVector()) {
            for (GameObject* child : object->getChildren()) {
                sf::Vector2f v1 = world_to_screen(object->getGlobalPosition());
                sf::Vector2f v2 = world_to_screen(child->getGlobalPosition());
                draw_line(target, v1, v2, sf::Color(128, 128, 128));
            }
        }
        // object origin circles
        for (size_t i = 0; i < game_objects.getAllSize(); i++) {
            GameObject* gameobject = game_objects.getFromAll(i);
            sf::Color circle_color = gameobject == active_object ? sf::Color(255, 255, 0) : sf::Color(255, 159, 44);
            origin_shape.setFillColor(circle_color);
            origin_shape.setPosition(world_to_screen(gameobject->getGlobalPosition()));
            target.draw(origin_shape);
        }
        // object info
        for (size_t i = 0; i < game_objects.getAllSize(); i++) {
            GameObject* gameobject = game_objects.getFromAll(i);
            size_t info_index = 0;
            auto render_info = [&](const std::string& str) {
                sf::Vector2f object_screen_pos = world_to_screen(gameobject->getGlobalPosition());
                float line_scaling = 3.0f / 4.0f;
                sf::Vector2f offset = sf::Vector2f(0.0f, info_index * object_info_text.getCharacterSize() * line_scaling);
                sf::Vector2f pos = object_screen_pos + offset;
                object_info_text.setPosition(utils::quantize(pos));
                object_info_text.setString(str);
                target.draw(object_info_text);
                info_index++;
            };
            render_info("id: " + std::to_string(gameobject->id));
            render_info(gameobject->getName());
            render_info(utils::body_type_to_str(gameobject->getType()));
        }
        // indices
        for (size_t i = 0; i < game_objects.getAllSize(); i++) {
            if (PolygonObject* polygon_object = dynamic_cast<PolygonObject*>(game_objects.getFromAll(i))) {
                if (polygon_object->draw_indices) {
                    // TODO: render indices
                }
            }
        }
    }

    if (selected_tool == &select_tool) {
        if (select_tool.rectangle_select.active) {
            render_rectangle_select(target, select_tool.rectangle_select);
        }
    } else if (selected_tool == &drag_tool) {
        if (drag_tool.mouse_joint) {
            sf::Vector2f grabbed_point = world_to_screen(drag_tool.mouse_joint->GetAnchorB());
            draw_line(target, grabbed_point, mousePosf, sf::Color::Yellow);
        }
    } else if (selected_tool == &rotate_tool) {
        for (size_t i = 0; i < rotate_tool.rotating_objects.size(); i++) {
            draw_line(target, world_to_screen(rotate_tool.pivot_pos), mousePosf, sf::Color::Yellow);
        }
    } else if (selected_tool == &edit_tool && active_object) {
        if (edit_tool.mode == EditTool::ADD && edit_tool.edge_vertex != -1) {
            // ghost edge
            sf::Vector2i v1 = world_to_pixel(active_object->getGlobalVertexPos(edit_tool.edge_vertex));
            sf::Vector2i v2 = mousePos;
            draw_line(target, to2f(v1), to2f(v2), sf::Color(255, 255, 255, 128));
            // ghost edge normal
            sf::Vector2f norm_v1, norm_v2;
            if (edit_tool.edge_vertex == 0) {
                get_screen_normal(v1, v2, norm_v1, norm_v2);
            } else {
                get_screen_normal(v2, v1, norm_v1, norm_v2);
            }
            draw_line(target, norm_v1, norm_v2, sf::Color(0, 255, 255, 128));
            // ghost vertex
            sf::Vector2f ghost_vertex_pos = to2f(mousePos);
            edit_tool.vertex_rect.setPosition(ghost_vertex_pos);
            edit_tool.vertex_rect.setFillColor(sf::Color(255, 0, 0, 128));
            target.draw(edit_tool.vertex_rect);
        } else if (edit_tool.mode == EditTool::INSERT && edit_tool.highlighted_edge != -1) {
            // edge highlight
            b2Vec2 v1 = active_object->getGlobalVertexPos(edit_tool.highlighted_edge);
            b2Vec2 v2 = active_object->getGlobalVertexPos(active_object->indexLoop(edit_tool.highlighted_edge + 1));
            sf::Vector2f v1_screen = world_to_screen(v1);
            sf::Vector2f v2_screen = world_to_screen(v2);
            sf::Vector2f vec = v2_screen - v1_screen;
            float angle = atan2(vec.y, vec.x);
            edit_tool.edge_highlight.setPosition(v1_screen);
            edit_tool.edge_highlight.setRotation(utils::to_degrees(angle));
            edit_tool.edge_highlight.setSize(sf::Vector2f(utils::length(vec), 3.0f));
            target.draw(edit_tool.edge_highlight);
            // ghost vertex on the edge
            sf::Vector2f ghost_vertex_pos = world_to_screen(edit_tool.insertVertexPos);
            edit_tool.vertex_rect.setFillColor(sf::Color(255, 0, 0, 128));
            edit_tool.vertex_rect.setPosition(ghost_vertex_pos);
            target.draw(edit_tool.vertex_rect);
        }
        // vertices
        for (size_t i = 0; i < active_object->getVertexCount(); i++) {
            edit_tool.vertex_rect.setPosition(world_to_screen(active_object->getGlobalVertexPos(i)));
            bool selected = active_object->isVertexSelected(i);
            sf::Color vertex_color = selected ? sf::Color(255, 255, 0) : sf::Color(255, 0, 0);
            edit_tool.vertex_rect.setFillColor(vertex_color);
            target.draw(edit_tool.vertex_rect);
        }
        // edge normals
        for (size_t i = 0; i < active_object->getEdgeCount(); i++) {
            sf::Vector2f norm_v1, norm_v2;
            get_screen_normal(
                active_object->getGlobalVertexPos(i),
                active_object->getGlobalVertexPos(active_object->indexLoop(i + 1)),
                norm_v1, 
                norm_v2
            );
            draw_line(target, norm_v1, norm_v2, sf::Color(0, 255, 255));
        }
        if (edit_tool.mode == EditTool::HOVER && edit_tool.highlighted_vertex != -1) {
            // highlighted vertex
            sf::Vector2f vertex_pos = world_to_screen(active_object->getGlobalVertexPos(edit_tool.highlighted_vertex));
            edit_tool.vertex_highlight_rect.setPosition(vertex_pos);
            target.draw(edit_tool.vertex_highlight_rect);
        } else if (edit_tool.mode == EditTool::SELECT && edit_tool.rectangle_select.active) {
            //selection box
            render_rectangle_select(target, edit_tool.rectangle_select);
        }
    }

    ui_texture.display();
    sf::Sprite ui_sprite(ui_texture.getTexture());
    window.draw(ui_sprite);
}

void Editor::maximize_window() const {
    sf::WindowHandle windowHandle = window.getSystemHandle();
    ShowWindow(windowHandle, SW_MAXIMIZE);
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
    size_t index = 0;
    std::function<void(GameObject*)> serialize_obj = [&](GameObject* obj) {
        logger << "Object: " << obj->id << "\n";
        if (index > 0) {
            tw << "\n\n";
        }
        obj->serialize(tw);
        index++;
    };
    std::function<void(GameObject*)> serialize_tree = [&](GameObject* obj) {
        serialize_obj(obj);
        LoggerIndent children_indent;
        for (size_t i = 0; i < obj->getChildren().size(); i++) {
            serialize_tree(obj->getChild(i));
        }
    };
    {
        logger << "Objects\n";
        LoggerIndent objects_indent;
        if (game_objects.getAllSize() == 0) {
            logger << "<empty>\n";
        }
        for (size_t i = 0; i < game_objects.getTopSize(); i++) {
            GameObject* gameobject = game_objects.getFromTop(i);
            serialize_tree(gameobject);
        }
    }
    {
        logger << "Joints\n";
        LoggerIndent joints_indent;
        if (game_objects.getJointsSize() == 0) {
            logger << "<empty>\n";
        } else {
            tw << "\n\n";
        }
        for (size_t i = 0; i < game_objects.getJointsSize(); i++) {
            if (i > 0) {
                tw << "\n\n";
            }
            Joint* joint = game_objects.getJoint(i);
            logger << "Joint: " << joint->object1->id << " " << joint->object2->id << "\n";
            game_objects.getJoint(i)->serialize(tw);
        }
    }
    return tw.toStr();
}

void Editor::deserialize(const std::string& str, bool set_camera) {
    ptrdiff_t active_object_id = -1;
    if (active_object) {
        active_object_id = active_object->id;
    }
    game_objects.clear();
    init_tools();
    init_world();
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
            } else if (entity == "object") {
                std::unique_ptr<GameObject> gameobject;
                std::string type = tr.readString();
                if (type == "box") {
                    gameobject = BoxObject::deserialize(tr, &game_objects);
                } else if (type == "ball") {
                    gameobject = BallObject::deserialize(tr, &game_objects);
                } else if (type == "polygon") {
                    gameobject = PolygonObject::deserialize(tr, &game_objects);
                } else if (type == "chain") {
                    gameobject = ChainObject::deserialize(tr, &game_objects);
                } else {
                    throw std::runtime_error("Unknown object type: " + type);
                }
                game_objects.add(std::move(gameobject), false);
            } else if (entity == "joint") {
                std::string type = tr.readString();
                if (type == "revolute") {
                    ptrdiff_t body_a_id, body_b_id;
                    b2RevoluteJointDef def = RevoluteJoint::deserialize(tr, body_a_id, body_b_id);
                    GameObject* object1 = game_objects.getById(body_a_id);
                    GameObject* object2 = game_objects.getById(body_b_id);
                    std::unique_ptr<RevoluteJoint> uptr = std::make_unique<RevoluteJoint>(def, world.get(), object1, object2);
                    game_objects.addJoint(std::move(uptr));
                } else {
                    throw std::runtime_error("Unknown joint type: " + type);
                }
            } else {
                throw std::runtime_error("Unknown entity type: " + entity);
            }
        }
        if (active_object_id >= 0) {
            GameObject* object = game_objects.getById(active_object_id);
            setActiveObject(object);
        }
    } catch (std::exception exc) {
        throw std::runtime_error("Line " + std::to_string(tr.getLine(-1)) + ": " + exc.what());
    }
}

void Editor::save_to_file(const std::string& filename) const {
    LoggerTag tag_saveload("saveload");
    std::string str = serialize();
    utils::str_to_file(str, filename);
    logger << "Saved to " << filename << "\n";
}

void Editor::load_action(const std::string& filename) {
    load_request.requested = true;
    load_request.filename = filename;
}

void Editor::load_from_file(const std::string& filename) {
    LoggerTag tag_saveload("saveload");
    try {
        std::string str = utils::file_to_str(filename);
        deserialize(str, true);
        logger << "Loaded from " << filename << "\n";
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
            try_select_tool(&select_tool);
        }
    }
}

Tool* Editor::try_select_tool_by_index(size_t index) {
    if (tools_in_tool_panel.size() > index) {
        Tool* tool = tools_in_tool_panel[index];
        try_select_tool(tool);
    }
    return nullptr;
}

Tool* Editor::try_select_tool(Tool* tool) {
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

void Editor::select_create_type(int type) {
    create_tool.type = static_cast<CreateTool::ObjectType>(type);
    for (size_t j = 0; j < create_tool.create_buttons.size(); j++) {
        create_tool.create_buttons[j]->setFillColor(sf::Color(128, 128, 128));
    }
    create_tool.create_buttons[type]->setFillColor(sf::Color(0, 175, 255));
}

void Editor::toggle_pause() {
    paused = !paused;
    paused_rect_widget->setVisible(paused);
}

void Editor::load_font(sf::Font& font, const std::string& filename, bool smooth) {
    if (!font.loadFromFile(filename)) {
        throw std::runtime_error("Font loading error (" + filename + ")");
    }
    font.setSmooth(smooth);
}

sf::Vector2f Editor::screen_to_world(const sf::Vector2f& screen_pos) const {
    sf::Transform combined = world_view.getInverseTransform() * ui_view.getTransform();
    sf::Vector2f result = combined.transformPoint(screen_pos);
    return result;
}

sf::Vector2f Editor::pixel_to_world(const sf::Vector2i& screen_pos) const {
    return screen_to_world(to2f(screen_pos) + sf::Vector2f(0.5f, 0.5f));
}

sf::Vector2f Editor::world_to_screen(const sf::Vector2f& world_pos) const {
    sf::Transform combined = ui_view.getInverseTransform() * world_view.getTransform();
    sf::Vector2f result = combined.transformPoint(world_pos);
    return result;
}

sf::Vector2f Editor::world_to_screen(const b2Vec2& world_pos) const {
    return world_to_screen(tosf(world_pos));
}

sf::Vector2i Editor::world_to_pixel(const sf::Vector2f& world_pos) const {
    return to2i(world_to_screen(world_pos));
}

sf::Vector2i Editor::world_to_pixel(const b2Vec2& world_pos) const {
    return to2i(world_to_screen(tosf(world_pos)));
}

sf::Vector2f Editor::world_dir_to_screenf(const b2Vec2& world_dir) const {
    //TODO: calculate new direction using an actual sf::View
    return sf::Vector2f(world_dir.x, -world_dir.y);
}

ptrdiff_t Editor::mouse_get_chain_edge(const b2Fixture* fixture) const {
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
        b2Vec2 b2_mouse_pos = tob2(to2f(mousePos));
        b2Vec2 b2_p1 = tob2(world_to_screen(p1));
        b2Vec2 b2_p2 = tob2(world_to_screen(p2));
        float dist = utils::distance_to_line(b2_mouse_pos, b2_p1, b2_p2);
        if (dist <= EditTool::EDGE_HIGHLIGHT_DISTANCE) {
            return i;
        }
    }
    return -1;
}

b2Fixture* Editor::get_fixture_at(const sf::Vector2f& screen_pos) const {
    b2Vec2 world_pos = tob2(screen_to_world(screen_pos));
    b2Vec2 world_pos_next = tob2(screen_to_world(screen_pos + sf::Vector2f(1.0f, 1.0f)));
    b2Vec2 midpoint = 0.5f * (world_pos + world_pos_next);
    QueryCallback callback;
    b2AABB aabb;
    aabb.upperBound = world_pos;
    aabb.lowerBound = world_pos_next;
    world->QueryAABB(&callback, aabb);
    for (size_t i = 0; i < callback.fixtures.size(); i++) {
        b2Fixture* fixture = callback.fixtures[i];
        if (fixture->GetShape()->m_type == b2Shape::e_chain) {
            if (mouse_get_chain_edge(fixture) >= 0) {
                return fixture;
            }
        } else if (fixture->TestPoint(midpoint)) {
            return fixture;
        }
    }
    return nullptr;
}

GameObject* Editor::get_object_at(const sf::Vector2f& screen_pos) const {
    GameObject* result = nullptr;
    b2Fixture* fixture = get_fixture_at(mousePosf);
    if (fixture) {
        b2Body* body = fixture->GetBody();
        result = GameObject::getGameobject(body);
    }
    return result;
}

ptrdiff_t Editor::mouse_get_object_vertex() const {
    if (!active_object) {
        return -1;
    }
    ptrdiff_t closest_vertex_i = 0;
    sf::Vector2i closest_vertex_pos = world_to_pixel(active_object->getGlobalVertexPos(0));
    int closest_vertex_offset = utils::get_max_offset(closest_vertex_pos, mousePos);
    for (size_t i = 1; i < active_object->getVertexCount(); i++) {
        sf::Vector2i vertex_pos = world_to_pixel(active_object->getGlobalVertexPos(i));
        float offset = utils::get_max_offset(vertex_pos, mousePos);
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

ptrdiff_t Editor::mouse_get_object_edge() const {
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
        b2Vec2 b2_mouse_pos = tob2(to2f(mousePos));
        b2Vec2 b2_p1 = tob2(world_to_screen(p1));
        b2Vec2 b2_p2 = tob2(world_to_screen(p2));
        float dist = utils::distance_to_line(b2_mouse_pos, b2_p1, b2_p2);
        if (dist <= EditTool::EDGE_HIGHLIGHT_DISTANCE) {
            return i;
        }
    }
    return -1;
}

ptrdiff_t Editor::mouse_get_edge_vertex() const {
    if (!active_object) {
        return -1;
    }
    if (active_object->isClosed()) {
        return -1;
    }
    ptrdiff_t v_start_i = 0;
    ptrdiff_t v_end_i = active_object->getEdgeCount();
    sf::Vector2f v_start = world_to_screen(active_object->getGlobalVertexPos(v_start_i));
    sf::Vector2f v_end = world_to_screen(active_object->getGlobalVertexPos(v_end_i));
    float v_start_dist = utils::length(mousePosf - v_start);
    float v_end_dist = utils::length(mousePosf - v_end);
    if (v_start_dist <= v_end_dist) {
        return v_start_i;
    } else {
        return v_end_i;
    }
}

void Editor::select_vertices_in_rect(const RectangleSelect& rectangle_select) {
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

void Editor::select_objects_in_rect(const RectangleSelect& rectangle_select) {
    select_tool.clearRectSelected();
    b2AABB aabb;
    float lower_x = std::min(rectangle_select.select_origin.x, sfMousePosWorld.x);
    float lower_y = std::min(rectangle_select.select_origin.y, sfMousePosWorld.y);
    float upper_x = std::max(rectangle_select.select_origin.x, sfMousePosWorld.x);
    float upper_y = std::max(rectangle_select.select_origin.y, sfMousePosWorld.y);
    aabb.lowerBound = b2Vec2(lower_x, lower_y);
    aabb.upperBound = b2Vec2(upper_x, upper_y);
    QueryCallback callback;
    world->QueryAABB(&callback, aabb);
    for (size_t i = 0; i < callback.fixtures.size(); i++) {
        if (utils::rect_fixture_intersect(aabb.lowerBound, aabb.upperBound, callback.fixtures[i])) {
            GameObject* gameobject = GameObject::getGameobject(callback.fixtures[i]->GetBody());
            select_tool.addToRectSelection(gameobject);
        }
    }
}

void Editor::render_rectangle_select(sf::RenderTarget& target, RectangleSelect& rectangle_select) {
    sf::Vector2f pos = world_to_screen(rectangle_select.select_origin);
    rectangle_select.select_rect.setPosition(pos);
    rectangle_select.select_rect.setSize(mousePosf - pos);
    target.draw(rectangle_select.select_rect);
}

void Editor::get_screen_normal(const b2Vec2& v1, const b2Vec2& v2, sf::Vector2f& norm_v1, sf::Vector2f& norm_v2) const {
    b2Vec2 midpoint = 0.5f * (v1 + v2);
    norm_v1 = world_to_screen(midpoint);
    sf::Vector2f edge_dir = utils::normalize(world_dir_to_screenf(v2 - v1));
    sf::Vector2f norm_dir = utils::rot90CCW(edge_dir);
    norm_v2 = norm_v1 + norm_dir * (float)EditTool::NORMAL_LENGTH;
}

void Editor::get_screen_normal(const sf::Vector2i& v1, const sf::Vector2i& v2, sf::Vector2f& norm_v1, sf::Vector2f& norm_v2) const {
    sf::Vector2f v1f = to2f(v1);
    sf::Vector2f v2f = to2f(v2);
    sf::Vector2f midpoint = (v1f + v2f) / 2.0f;
    norm_v1 = midpoint;
    sf::Vector2f edge_dir = utils::normalize(v2f - v1f);
    sf::Vector2f norm_dir = utils::rot90CW(edge_dir);
    norm_v2 = norm_v1 + norm_dir * (float)EditTool::NORMAL_LENGTH;
}

bool Editor::is_parent_selected(const GameObject* object) const {
    CompVector<GameObject*> parents = object->getParentChain();
    for (size_t i = 0; i < parents.size(); i++) {
        auto it = select_tool.getSelectedObjects().find(parents[i]);
        if (it != select_tool.getSelectedObjects().getSet().end()) {
            return true;
        }
    }
    return false;
}

void Editor::grab_selected() {
    move_tool.orig_cursor_pos = b2MousePosWorld;
    move_tool.moving_objects = CompVector<GameObject*>();
    for (GameObject* obj : select_tool.getSelectedObjects()) {
        if (is_parent_selected(obj)) {
            continue;
        }
        move_tool.moving_objects.add(obj);
        obj->orig_pos = obj->getGlobalPosition();
        obj->cursor_offset = obj->getGlobalPosition() - b2MousePosWorld;
        obj->was_enabled = obj->rigid_body->IsEnabled();
        obj->setEnabled(false, true);
        obj->setLinearVelocity(b2Vec2(0.0f, 0.0f), true);
        obj->setAngularVelocity(0.0f, true);
    }
}

void Editor::rotate_selected() {
    rotate_tool.orig_cursor_pos = b2MousePosWorld;
    rotate_tool.rotating_objects = CompVector<GameObject*>();
    for (GameObject* obj : select_tool.getSelectedObjects()) {
        if (is_parent_selected(obj)) {
            continue;
        }
        rotate_tool.rotating_objects.add(obj);
        obj->orig_pos = obj->getGlobalPosition();
        obj->orig_angle = obj->getGlobalRotation();
        obj->was_enabled = obj->rigid_body->IsEnabled();
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

void Editor::delete_object(GameObject* object, bool remove_children) {
    if (!object) {
        return;
    }
    if (object == active_object) {
        setActiveObject(nullptr);
    }
    select_tool.deselectObject(object);
    game_objects.remove(object, remove_children);
}

void Editor::check_debugbreak() {
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
            fps = 1.0 / average_frame_time;
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
