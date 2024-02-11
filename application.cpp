#include "application.h"
#include "utils.h"
#include <numbers>
#include <iostream>

const auto tob2 = utils::tob2;
const auto tosf = utils::tosf;
const auto to2i = utils::to2i;
const auto to2f = utils::to2f;

Logger& operator<<(Logger& lg, const b2Vec2& value) {
    return lg << "(" << value.x << " " << value.y << ")";
}

Logger& operator<<(Logger& lg, const sf::Vector2f& value) {
    return lg << "(" << value.x << " " << value.y << ")";
}

bool QueryCallback::ReportFixture(b2Fixture* fixture) {
    fixtures.push_back(fixture);
    return true;
}

void Application::init() {
    sf::ContextSettings cs_window;
    cs_window.antialiasingLevel = ANTIALIASING;
    window.create(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "SFML", sf::Style::Default, cs_window);
    window.setVerticalSyncEnabled(true);
    sf::ContextSettings cs_world;
    world_texture.create(WINDOW_WIDTH, WINDOW_HEIGHT, cs_world);
    sf::ContextSettings cs_ui;
    ui_texture.create(WINDOW_WIDTH, WINDOW_HEIGHT, cs_ui);
    sf::ContextSettings cs_mask;
    selection_mask.create(WINDOW_WIDTH, WINDOW_HEIGHT, cs_mask);
    window_view = sf::View(sf::FloatRect(0.0f, 0.0f, WINDOW_WIDTH, WINDOW_HEIGHT));
    world_view = sf::View(sf::FloatRect(0.0f, 0.0f, WINDOW_WIDTH, WINDOW_HEIGHT));
    ui_view = sf::View(sf::FloatRect(0.0f, 0.0f, WINDOW_WIDTH, WINDOW_HEIGHT));
    if (!desat_shader.loadFromFile("desat.frag", sf::Shader::Fragment)) {
        throw std::runtime_error("Shader loading error");
    }
    if (!selection_shader.loadFromFile("selection.frag", sf::Shader::Fragment)) {
        throw std::runtime_error("Shader loading error");
    }
    init_tools();
    init_world();
    init_ui();
    logger.OnLineWrite = [&](std::string line) { // should be after init_ui and preferably before any logging
        logger_text_widget->setString(line);
    };
    init_objects();
    try_select_tool(0); // should be after init_tools and init_ui
    select_create_type(0);
    maximize_window();
    load("level.txt");
    auto getter = [&]() { return serialize(); };
    auto setter = [&](std::string str) { deserialize(str, false); };
    history = History(getter, setter);
    history.save(HistoryEntry::BASE);

    assert(tools.size() > 0);
    assert(selected_tool);
    for (size_t i = 0; i < tools.size(); i++) {
        assert(tools[i]->widget);
    }
    assert(history.size() == 1);
}

void Application::start() {
    main_loop();
}

void Application::init_tools() {
    for (size_t i = 0; i < tools.size(); i++) {
        tools[i]->reset();
    }
}

void Application::init_world() {
    b2Vec2 gravity(0.0f, -9.8f);
    world = std::make_unique<b2World>(gravity);
    b2BodyDef mouse_body_def;
    drag_tool.mouse_body = world->CreateBody(&mouse_body_def);
}

void Application::init_ui() {
    std::string ui_font_filename = "STAN0757.TTF";
    if (!ui_font.loadFromFile(ui_font_filename)) {
        throw std::runtime_error("Font loading error (" + ui_font_filename + ")");
    }
    std::string fps_font_filename = "fraps.ttf";;
    if (!fps_font.loadFromFile(fps_font_filename)) {
        throw std::runtime_error("Font loading error (" + fps_font_filename + ")");
    }
    root_widget.setSize(sf::Vector2f(window.getSize().x, window.getSize().y));
    root_widget.setFillColor(sf::Color::Transparent);
    {
        std::unique_ptr<ContainerWidget> toolbox_widget_uptr = std::make_unique<ContainerWidget>();
        toolbox_widget = toolbox_widget_uptr.get();
        toolbox_widget->setFillColor(sf::Color(255, 0, 0, 0));
        toolbox_widget->setOrigin(Widget::TOP_CENTER);
        toolbox_widget->setParentAnchor(Widget::TOP_CENTER);
        toolbox_widget->setPadding(TOOLBOX_PADDING);
        toolbox_widget->setClickThrough(false);
        for (size_t i = 0; i < tools.size(); i++) {
            std::unique_ptr<RectangleWidget> tool_widget_uptr = std::make_unique<RectangleWidget>();
            RectangleWidget* tool_widget = tool_widget_uptr.get();
            tool_widget->setSize(sf::Vector2f(TOOL_RECT_WIDTH, TOOL_RECT_HEIGHT));
            tool_widget->setFillColor(sf::Color(128, 128, 128));
            tool_widget->setOutlineColor(sf::Color::Yellow);
            tool_widget->OnClick = [=](sf::Vector2f pos) {
                try_select_tool(i);
            };
            tool_widget->OnMouseEnter = [=]() {
                tool_widget->setOutlineThickness(-1.0f);
            };
            tool_widget->OnMouseExit = [=]() {
                tool_widget->setOutlineThickness(0.0f);
            };
            {
                std::unique_ptr<TextWidget> text_widget_uptr = std::make_unique<TextWidget>();
                TextWidget* text_widget = text_widget_uptr.get();
                text_widget->setFont(ui_font);
                text_widget->setCharacterSize(TOOL_TEXT_SIZE);
                text_widget->setString(tools[i]->name);
                text_widget->setFillColor(sf::Color::Black);
                text_widget->setOriginToTextCenter();
                text_widget->setParentAnchor(Widget::CENTER);
                tool_widget->addChild(std::move(text_widget_uptr));
            }
            tools[i]->widget = tool_widget;
            toolbox_widget->addChild(std::move(tool_widget_uptr));
        }
        root_widget.addChild(std::move(toolbox_widget_uptr));
    }
    {
        std::unique_ptr<ContainerWidget> edit_window_widget_uptr = std::make_unique<ContainerWidget>();
        ContainerWidget* edit_window_widget = edit_window_widget_uptr.get();
        edit_window_widget->setVisible(false);
        edit_window_widget->setSize(sf::Vector2f(100.0f, 200.0f));
        edit_window_widget->setFillColor(sf::Color(255, 0, 0));
        edit_window_widget->setOrigin(Widget::TOP_RIGHT);
        edit_window_widget->setParentAnchor(Widget::TOP_RIGHT);
        edit_window_widget->setAnchorOffset(-20.0f, 20.0f);
        edit_window_widget->setPadding(TOOLBOX_PADDING);
        edit_window_widget->setClickThrough(false);
        edit_window_widget->setAutoResize(false);
        edit_tool.edit_window_widget = edit_window_widget;
        root_widget.addChild(std::move(edit_window_widget_uptr));
    }
    {
        std::unique_ptr<ContainerWidget> create_panel_widget_uptr = std::make_unique<ContainerWidget>();
        ContainerWidget* create_panel_widget = create_panel_widget_uptr.get();
        create_panel_widget->setFillColor(sf::Color(255, 0, 0, 0));
        create_panel_widget->setOrigin(Widget::CENTER_LEFT);
        create_panel_widget->setParentAnchor(Widget::CENTER_LEFT);
        create_panel_widget->setHorizontal(false);
        create_panel_widget->setPadding(CREATE_PANEL_PADDING);
        create_panel_widget->setClickThrough(false);
        for (int i = 0; i < CreateTool::mode_count; i++) {
            std::unique_ptr<RectangleWidget> button_widget_uptr = std::make_unique<RectangleWidget>();
            RectangleWidget* button_widget = button_widget_uptr.get();
            button_widget->setSize(sf::Vector2f(CREATE_RECT_WIDTH, CREATE_RECT_HEIGHT));
            button_widget->setFillColor(sf::Color(128, 128, 128));
            button_widget->setOutlineColor(sf::Color(0, 175, 255));
            button_widget->OnClick = [=](sf::Vector2f pos) {
                select_create_type(i);
            };
            button_widget->OnMouseEnter = [=]() {
                button_widget->setOutlineThickness(-1.0f);
            };
            button_widget->OnMouseExit = [=]() {
                button_widget->setOutlineThickness(0.0f);
            };
            {
                std::unique_ptr<TextWidget> text_widget_uptr = std::make_unique<TextWidget>();
                TextWidget* text_widget = text_widget_uptr.get();
                text_widget->setFont(ui_font);
                text_widget->setCharacterSize(TOOL_TEXT_SIZE);
                text_widget->setString(CreateTool::create_type_name(static_cast<CreateTool::ObjectType>(i)));
                text_widget->setFillColor(sf::Color::Black);
                text_widget->setOriginToTextCenter();
                text_widget->setParentAnchor(Widget::CENTER);
                button_widget->addChild(std::move(text_widget_uptr));
            }
            create_tool.create_buttons.push_back(button_widget);
            create_tool.create_panel_widget = create_panel_widget;
            create_panel_widget->addChild(std::move(button_widget_uptr));
        }
        root_widget.addChild(std::move(create_panel_widget_uptr));
    }
    {
        std::unique_ptr<ContainerWidget> paused_rect_uptr = std::make_unique<ContainerWidget>();
        paused_rect_widget = paused_rect_uptr.get();
        paused_rect_widget->setFillColor(sf::Color(0, 0, 0, 128));
        paused_rect_widget->setOrigin(Widget::TOP_LEFT);
        paused_rect_widget->setPadding(10.0f);
        {
            std::unique_ptr<TextWidget> paused_text_uptr = std::make_unique<TextWidget>();
            TextWidget* paused_text_widget = paused_text_uptr.get();
            paused_text_widget->setFont(ui_font);
            paused_text_widget->setString("PAUSED");
            paused_text_widget->setCharacterSize(24);
            paused_text_widget->setFillColor(sf::Color::Yellow);
            paused_text_widget->setOrigin(Widget::TOP_LEFT);
            paused_rect_widget->addChild(std::move(paused_text_uptr));
        }
        root_widget.addChild(std::move(paused_rect_uptr));
    }
    {
        std::unique_ptr<ContainerWidget> fps_uptr = std::make_unique<ContainerWidget>();
        fps_widget = fps_uptr.get();
        fps_widget->setFillColor(sf::Color::Yellow);
        fps_widget->setOrigin(Widget::TOP_LEFT);
        fps_widget->setPosition(120.0f, 0.0f);
        fps_widget->setPadding(0.0f);
        {
            std::unique_ptr<TextWidget> fps_text_uptr = std::make_unique<TextWidget>();
            fps_text_widget = fps_text_uptr.get();
            fps_text_widget->setFont(fps_font);
            fps_text_widget->setCharacterSize(32);
            fps_text_widget->setFillColor(sf::Color::Black);
            fps_text_widget->setOrigin(Widget::TOP_LEFT);
            fps_widget->addChild(std::move(fps_text_uptr));
        }
        root_widget.addChild(std::move(fps_uptr));
    }
    {
        std::unique_ptr<RectangleWidget> logger_uptr = std::make_unique<RectangleWidget>();
        logger_widget = logger_uptr.get();
        logger_widget->setFillColor(sf::Color(0, 0, 0));
        logger_widget->setSize(sf::Vector2f(500.0f, 20.0f));
        logger_widget->setOrigin(Widget::BOTTOM_LEFT);
        logger_widget->setParentAnchor(Widget::BOTTOM_LEFT);
        {
            std::unique_ptr<TextWidget> logger_text_uptr = std::make_unique<TextWidget>();
            logger_text_widget = logger_text_uptr.get();
            logger_text_widget->setFont(ui_font);
            logger_text_widget->setCharacterSize(15);
            logger_text_widget->setFillColor(sf::Color::White);
            logger_text_widget->setOrigin(Widget::TOP_LEFT);
            logger_text_widget->setString("Logger message");
            logger_widget->addChild(std::move(logger_text_uptr));
        }
        root_widget.addChild(std::move(logger_uptr));
    }
}

void Application::init_objects() {
    std::vector<b2Vec2> ground_vertices = {
        b2Vec2(25.0f, 8.0f),
        b2Vec2(15.0f, 2.0f),
        b2Vec2(5.0f, 0.0f),
        b2Vec2(-5.0f, 0.0f),
        b2Vec2(-15.0f, 2.0f),
        b2Vec2(-25.0f, 8.0f),
    };
    ground = create_ground(b2Vec2(0.0f, 0.0f), ground_vertices, sf::Color(255, 255, 255));

    GameObject* box0 = create_box(b2Vec2(0.0f, 1.0f), utils::to_radians(0.0f), b2Vec2(1.0f, 1.0f), sf::Color(0, 255, 0));
    GameObject* box1 = create_box(b2Vec2(0.1f, 2.0f), utils::to_radians(0.0f), b2Vec2(1.0f, 1.0f), sf::Color(0, 255, 0));
    GameObject* box2 = create_box(b2Vec2(0.2f, 3.0f), utils::to_radians(0.0f), b2Vec2(1.0f, 1.0f), sf::Color(0, 255, 0));
    GameObject* ball = create_ball(b2Vec2(0.0f, 5.0f), 0.5f, sf::Color(0, 255, 0), sf::Color(0, 64, 0));
    std::vector<GameObject*> dynamic_objects = { box0, box1, box2, ball };
    for (int i = 0; i < dynamic_objects.size(); i++) {
        GameObject* box = dynamic_objects[i];
        box->setType(b2_dynamicBody, false);
        box->setDensity(1.0f, false);
        box->setFriction(0.3f, false);
        box->setRestitution(0.5f, false);
    }

    std::vector<float> lengths = { 5.0f, 1.0f, 5.0f, 1.0f, 5.0f, 1.0f };
    std::vector<float> wheels = { 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f };
    GameObject* car = create_car(b2Vec2(0.0f, 10.0f), lengths, wheels, sf::Color(255, 0, 0));
    car->setType(b2_dynamicBody, false);
    car->setDensity(1.0f, false);
    car->setFriction(0.3f, false);
    car->setRestitution(0.5f, false);
}

void Application::main_loop() {
    fps_counter.init();
    while (window.isOpen()) {
        fps_counter.frameBegin();
        process_widgets();
        process_input();
        process_world();
        render();
        int fps = fps_counter.frameEnd();
        fps_text_widget->setString(std::to_string(fps));
    }
}

void Application::resetView() {
    viewCenterX = 0.0f;
    viewCenterY = 0.0f;
    zoomFactor = 1.0f;
}

void Application::process_widgets() {
    root_widget.setSize(sf::Vector2f(ui_texture.getSize().x, ui_texture.getSize().y));
    root_widget.updateMouseState();
    Widget::click_blocked = false;
}

void Application::process_input() {
    sf::Event event;
    while (window.pollEvent(event)) {
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
        process_keyboard_event(event);
        process_mouse_event(event);
    }
    process_keyboard();
    process_mouse();
    if (commit_action) {
        history.save(HistoryEntry::NORMAL);
        commit_action = false;
    }
    if (quickload_requested) {
        quickload();
        quickload_requested = false;
    }
}

void Application::process_keyboard_event(sf::Event event) {
    if (event.type == sf::Event::KeyPressed) {
        switch (event.key.code) {
            case sf::Keyboard::R: resetView(); break;
            case sf::Keyboard::Space: toggle_pause(); break;
            case sf::Keyboard::Num1: try_select_tool(0); break;
            case sf::Keyboard::Num2: try_select_tool(1); break;
            case sf::Keyboard::Num3: try_select_tool(2); break;
            case sf::Keyboard::Num4: try_select_tool(3); break;
            case sf::Keyboard::Num5: try_select_tool(4); break;
            case sf::Keyboard::Num6: try_select_tool(5); break;
            case sf::Keyboard::Num7: try_select_tool(6); break;
            case sf::Keyboard::Num8: try_select_tool(7); break;
            case sf::Keyboard::Num9: try_select_tool(8); break;
            case sf::Keyboard::Num0: try_select_tool(9); break;
            case sf::Keyboard::X:
                if (ground->tryDeleteVertex(edit_tool.highlighted_vertex)) {
                    commit_action = true;
                }
                break;
            case sf::Keyboard::LControl: edit_tool.mode = EditTool::ADD; break;
            case sf::Keyboard::LAlt: edit_tool.mode = EditTool::INSERT; break;
            case sf::Keyboard::S:
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)) {
                    save("level.txt");
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
                if (selected_tool == &edit_tool) {
                    if (sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt)) {
                        ground->deselectAllVertices();
                    } else {
                        ground->selectAllVertices();
                    }
                }
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

void Application::process_mouse_event(sf::Event event) {
    mousePos = sf::Mouse::getPosition(window);
    mousePosf = to2f(mousePos);
    sfMousePosWorld = sf_screen_to_world(mousePos);
    b2MousePosWorld = tob2(sfMousePosWorld);
    if (event.type == sf::Event::MouseButtonPressed) {
        switch (event.mouseButton.button) {
            case sf::Mouse::Left:
                process_left_click();
                break;
            case sf::Mouse::Right:
                mousePrevPos = sf::Vector2i(event.mouseButton.x, event.mouseButton.y);
                break;
        }
    }
    if (event.type == sf::Event::MouseButtonReleased) {
        switch (event.mouseButton.button) {
            case sf::Mouse::Left:
                if (select_tool.rectangle_select.active) {
                    select_tool.rectangle_select.active = false;
                }
                if (drag_tool.mouse_joint) {
                    world->DestroyJoint(drag_tool.mouse_joint);
                    drag_tool.mouse_joint = nullptr;
                }
                if (move_tool.object) {
                    //TODO: remember state for all children
                    move_tool.object->setEnabled(move_tool.object_was_enabled, true);
                    move_tool.object = nullptr;
                    commit_action = true;
                }
                if (rotate_tool.object) {
                    rotate_tool.object->setEnabled(rotate_tool.object_was_enabled, true);
                    rotate_tool.object = nullptr;
                    commit_action = true;
                }
                if (edit_tool.grabbed_vertex != -1) {
                    edit_tool.grabbed_vertex = -1;
                    ground->saveOffsets();
                    commit_action = true;
                    edit_tool.mode = EditTool::HOVER;
                }
                if (edit_tool.rectangle_select.active) {
                    edit_tool.rectangle_select.active = false;
                    edit_tool.mode = EditTool::HOVER;
                }
                break;
        }
    }
    if (event.type == sf::Event::MouseWheelScrolled) {
        if (event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel) {
            zoomFactor *= pow(MOUSE_SCROLL_ZOOM, event.mouseWheelScroll.delta);
        }
    }
}

void Application::process_keyboard() {
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
        window.close();
    }
}

void Application::process_mouse() {
    if (selected_tool == &select_tool) {
        GameObject* old_hover = select_tool.hover_object;
        GameObject* new_hover = get_object_at(mousePos);
        if (old_hover) {
            old_hover->hover = false;
        }
        if (new_hover) {
            new_hover->hover = true;
        }
        select_tool.hover_object = new_hover;
    } else if (selected_tool == &edit_tool) {
        if (edit_tool.mode == EditTool::HOVER) {
            edit_tool.highlighted_vertex = mouse_get_ground_vertex();
        } else if (edit_tool.mode == EditTool::ADD) {
            edit_tool.edge_vertex = mouse_get_edge_vertex();
        } else if (edit_tool.mode == EditTool::INSERT) {
            edit_tool.highlighted_edge = mouse_get_ground_edge();
            if (edit_tool.highlighted_edge != -1) {
                b2Vec2 v1 = ground->getVertexPos(edit_tool.highlighted_edge);
                b2Vec2 v2 = ground->getVertexPos(edit_tool.highlighted_edge + 1);
                edit_tool.insertVertexPos = utils::line_project(b2MousePosWorld, v1, v2);
            }
        }
    }
    if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
        if (selected_tool == &select_tool) {
            if (select_tool.rectangle_select.active) {
                select_objects_in_rect(select_tool.rectangle_select);
            }
        } else if (selected_tool == &drag_tool) {
            if (drag_tool.mouse_joint) {
                drag_tool.mouse_joint->SetTarget(b2MousePosWorld);
            }
        } else if (selected_tool == &move_tool) {
            if (move_tool.object) {
                move_tool.object->setPosition(b2MousePosWorld - move_tool.offset, true);
            }
        } else if (selected_tool == &rotate_tool) {
            if (rotate_tool.object) {
                b2Vec2 mouse_vector = b2MousePosWorld - rotate_tool.object->rigid_body->GetPosition();
                float angle = atan2(mouse_vector.y, mouse_vector.x);
                rotate_tool.object->setAngle(angle - rotate_tool.angle_offset, true);
            }
        } else if (selected_tool == &edit_tool) {
            if (edit_tool.mode == EditTool::SELECT) {
                if (edit_tool.rectangle_select.active) {
                    select_vertices_in_rect(edit_tool.rectangle_select);
                }
            } else if (edit_tool.mode == EditTool::MOVE) {
                if (edit_tool.grabbed_vertex != -1) {
                    ptrdiff_t index = edit_tool.grabbed_vertex;
                    const GroundVertex& vertex = ground->getVertex(index);
                    b2Vec2 offset = b2MousePosWorld + edit_tool.grabbed_vertex_offset - vertex.orig_pos;
                    ground->offsetVertex(index, offset, false);
                    ground->offsetSelected(offset, false);
                    ground->syncVertices();
                }
            }
        }
    }
    if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
        sf::Vector2i mouseDelta = mousePrevPos - mousePos;
        viewCenterX += mouseDelta.x / zoomFactor;
        viewCenterY += -mouseDelta.y / zoomFactor;
    }
    mousePrevPos = mousePos;
}

void Application::process_left_click() {
    root_widget.processClick(mousePosf);
    if (Widget::click_blocked) {
        return;
    }
    if (selected_tool == &select_tool) {
        GameObject* object = get_object_at(mousePos);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) {
            select_tool.toggleSelect(object);
        } else {
            select_tool.selectSingleObject(object);
        }
        select_tool.rectangle_select.active = true;
        select_tool.rectangle_select.select_origin = sfMousePosWorld;
    } else if (selected_tool == &create_tool) {
        switch (create_tool.type) {
            case CreateTool::BOX:
                create_box(
                    b2MousePosWorld, 0.0f, NEW_BOX_SIZE, NEW_BOX_COLOR
                );
                commit_action = true;
                break;
            case CreateTool::BALL:
                create_ball(
                    b2MousePosWorld, NEW_BALL_RADIUS, NEW_BALL_COLOR, NEW_BALL_NOTCH_COLOR
                );
                commit_action = true;
                break;
        }
    } else if (selected_tool == &drag_tool) {
        b2Fixture* grabbed_fixture = get_fixture_at(mousePos);
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
        }
    } else if (selected_tool == &move_tool) {
        GameObject* gameobject = get_object_at(mousePos);
        if (gameobject) {
            b2Body* body = gameobject->rigid_body;
            move_tool.object_was_enabled = body->IsEnabled();
            move_tool.offset = b2MousePosWorld - body->GetPosition();
            gameobject->setEnabled(false, true);
            gameobject->setLinearVelocity(b2Vec2(0.0f, 0.0f), true);
            gameobject->setAngularVelocity(0.0f, true);
            move_tool.object = gameobject;
        }
    } else if (selected_tool == &rotate_tool) {
        GameObject* gameobject = get_object_at(mousePos);
        if (gameobject) {
            b2Body* body = gameobject->rigid_body;
            rotate_tool.object_was_enabled = body->IsEnabled();
            b2Vec2 mouse_vector = b2MousePosWorld - body->GetPosition();
            float mouse_angle = atan2(mouse_vector.y, mouse_vector.x);
            rotate_tool.angle_offset = mouse_angle - body->GetAngle();
            gameobject->setEnabled(false, true);
            gameobject->setAngularVelocity(0.0f, true);
            rotate_tool.object = gameobject;
        }
    } else if (selected_tool == &edit_tool) {
        if (edit_tool.mode == EditTool::HOVER) {
            if (edit_tool.highlighted_vertex != -1) {
                edit_tool.mode = EditTool::MOVE;
                edit_tool.grabbed_vertex = edit_tool.highlighted_vertex;
                const GroundVertex& vertex = ground->getVertex(edit_tool.grabbed_vertex);
                edit_tool.grabbed_vertex_offset = vertex.pos - b2MousePosWorld;
                bool shift = sf::Keyboard::isKeyPressed(sf::Keyboard::LShift);
                if (!vertex.selected && !shift) {
                    ground->deselectAllVertices();
                } else if (shift) {
                    ground->selectVertex(edit_tool.grabbed_vertex);
                }
            } else {
                edit_tool.mode = EditTool::SELECT;
                edit_tool.rectangle_select.active = true;
                edit_tool.rectangle_select.select_origin = sfMousePosWorld;
                if (!sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) {
                    ground->deselectAllVertices();
                }
            }
        } else if (edit_tool.mode == EditTool::ADD && edit_tool.edge_vertex != -1) {
            if (edit_tool.edge_vertex == 0) {
                ground->addVertex(0, b2MousePosWorld);
            } else if (edit_tool.edge_vertex > 0) {
                ground->addVertex(edit_tool.edge_vertex + 1, b2MousePosWorld);
            }
            commit_action = true;
        } else if (edit_tool.mode == EditTool::INSERT && edit_tool.highlighted_edge != -1) {
            ground->addVertex(edit_tool.highlighted_edge + 1, edit_tool.insertVertexPos);
            commit_action = true;
        }
    }
}

void Application::process_world() {
    if (!paused) {
        world->Step(timeStep, VELOCITY_ITERATIONS, POSITION_ITERATIONS);
    }
}

void Application::render() {
    window.clear(sf::Color(0, 0, 0));
    window_view.setCenter(window.getSize().x / 2.0f, window.getSize().y / 2.0f);
    window_view.setSize(window.getSize().x, window.getSize().y);
    window.setView(window_view);
    render_world();
    render_ui();
    window.display();
}

void Application::render_world() {
    world_texture.clear(sf::Color(0, 0, 0));
    world_view.setCenter(viewCenterX, viewCenterY);
    world_view.setSize(world_texture.getSize().x / zoomFactor, -1.0f * world_texture.getSize().y / zoomFactor);
    world_texture.setView(world_view);

    selection_mask.clear();
    selection_mask.setView(world_view);

    for (size_t i = 0; i < game_objects.size(); i++) {
        GameObject* gameobject = game_objects[i].get();
        gameobject->render(world_texture);
    }
    //if (select_tool.hover_object) {
    //    select_tool.hover_object->renderMask(selection_mask, false);
    //}
    for (size_t i = 0; i < select_tool.selected_objects.size(); i++) {
        select_tool.selected_objects[i]->renderMask(selection_mask, false);
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

void Application::render_ui() {
    ui_texture.clear(sf::Color(0, 0, 0, 0));
    ui_view.setCenter(ui_texture.getSize().x / 2.0f, ui_texture.getSize().y / 2.0f);
    ui_view.setSize(ui_texture.getSize().x, ui_texture.getSize().y);
    ui_texture.setView(ui_view);
    sf::RenderTarget& target = ui_texture;
    if (selected_tool == &select_tool) {
        if (select_tool.rectangle_select.active) {
            render_rectangle_select(target, select_tool.rectangle_select);
        }
    } else if (selected_tool == &drag_tool) {
        if (drag_tool.mouse_joint) {
            sf::Vector2f grabbed_point = world_to_screenf(drag_tool.mouse_joint->GetAnchorB());
            draw_line(target, grabbed_point, mousePosf, sf::Color::Yellow);
        }
    } else if (selected_tool == &rotate_tool) {
        if (rotate_tool.object) {
            sf::Vector2f body_origin = world_to_screenf(rotate_tool.object->rigid_body->GetPosition());
            draw_line(target, body_origin, mousePosf, sf::Color::Yellow);
        }
    } else if (selected_tool == &edit_tool) {
        if (edit_tool.mode == EditTool::ADD && edit_tool.edge_vertex != -1) {
            // ghost edge
            sf::Vector2i v1 = world_to_screen(ground->getVertexPos(edit_tool.edge_vertex));
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
            b2Vec2 v1 = ground->getVertexPos(edit_tool.highlighted_edge);
            b2Vec2 v2 = ground->getVertexPos(edit_tool.highlighted_edge + 1);
            sf::Vector2f v1_screen = world_to_screenf(v1);
            sf::Vector2f v2_screen = world_to_screenf(v2);
            sf::Vector2f vec = v2_screen - v1_screen;
            float angle = atan2(vec.y, vec.x);
            edit_tool.edge_highlight.setPosition(v1_screen);
            edit_tool.edge_highlight.setRotation(utils::to_degrees(angle));
            edit_tool.edge_highlight.setSize(sf::Vector2f(utils::get_length(vec), 3.0f));
            target.draw(edit_tool.edge_highlight);
            // ghost vertex on the edge
            sf::Vector2f ghost_vertex_pos = world_to_screenf(edit_tool.insertVertexPos);
            edit_tool.vertex_rect.setFillColor(sf::Color(255, 0, 0, 128));
            edit_tool.vertex_rect.setPosition(ghost_vertex_pos);
            target.draw(edit_tool.vertex_rect);
        }
        // ground vertices
        for (size_t i = 0; i < ground->getVertexCount(); i++) {
            edit_tool.vertex_rect.setPosition(world_to_screenf(ground->getVertexPos(i)));
            bool selected = ground->isVertexSelected(i);
            sf::Color vertex_color = selected ? sf::Color(255, 255, 0) : sf::Color(255, 0, 0);
            edit_tool.vertex_rect.setFillColor(vertex_color);
            target.draw(edit_tool.vertex_rect);
        }
        // ground edge normals
        for (size_t i = 0; i < ground->getVertexCount() - 1; i++) {
            sf::Vector2f norm_v1, norm_v2;
            get_screen_normal(ground->getVertexPos(i), ground->getVertexPos(i + 1), norm_v1, norm_v2);
            draw_line(target, norm_v1, norm_v2, sf::Color(0, 255, 255));
        }
        if (edit_tool.mode == EditTool::HOVER && edit_tool.highlighted_vertex != -1) {
            // highlighted vertex
            sf::Vector2f vertex_pos = world_to_screenf(ground->getVertexPos(edit_tool.highlighted_vertex));
            edit_tool.vertex_highlight_rect.setPosition(vertex_pos);
            target.draw(edit_tool.vertex_highlight_rect);
        } else if (edit_tool.mode == EditTool::SELECT && edit_tool.rectangle_select.active) {
            //selection box
            render_rectangle_select(target, edit_tool.rectangle_select);
        }
    }

    root_widget.render();

    ui_texture.display();
    sf::Sprite ui_sprite(ui_texture.getTexture());
    window.draw(ui_sprite);
}

void Application::maximize_window() {
    sf::WindowHandle windowHandle = window.getSystemHandle();
    ShowWindow(windowHandle, SW_MAXIMIZE);
}

std::string Application::serialize() {
    std::string str;
    TokenWriter tw(&str);
    tw << "camera\n";
    {
        TokenWriterIndent camera_indent(tw);
        tw << "center" << viewCenterX << viewCenterY << "\n";
        tw << "zoom" << zoomFactor << "\n";
    }
    tw << "/camera" << "\n\n";
    for (size_t i = 0; i < game_objects.size(); i++) {
        GameObject* gameobject = game_objects[i].get();
        game_objects[i]->serialize(tw);
        if (i < game_objects.size() - 1) {
            tw << "\n\n";
        }
    }
    return str;
}

void Application::deserialize(std::string str, bool set_camera) {
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
                    gameobject = BoxObject::deserialize(tr, world.get());
                } else if (type == "ball") {
                    gameobject = BallObject::deserialize(tr, world.get());
                } else if (type == "car") {
                    gameobject = CarObject::deserialize(tr, world.get());
                } else if (type == "ground") {
                    gameobject = GroundObject::deserialize(tr, world.get());
                    ground = static_cast<GroundObject*>(gameobject.get());
                } else {
                    throw std::runtime_error("Unknown object type: " + type);
                }
                game_objects.push_back(std::move(gameobject));
            } else {
                throw std::runtime_error("Unknown entity type: " + entity);
            }
        }
    } catch (std::exception exc) {
        throw std::runtime_error("Line " + std::to_string(tr.getLine(-1)) + ": " + exc.what());
    }
}

void Application::save(std::string filename) {
    std::string str = serialize();
    utils::str_to_file(str, filename);
    logger << "Saved to " << filename << "\n";
}

void Application::load(std::string filename) {
    try {
        std::string str = utils::file_to_str(filename);
        deserialize(str, true);
        logger << "Loaded from " << filename << "\n";
    } catch (std::exception exc) {
        throw std::runtime_error(__FUNCTION__": " + filename + ": " + std::string(exc.what()));
    }
}

void Application::quicksave() {
    quicksave_str = serialize();
    logger << "Quicksave\n";
}

void Application::quickload() {
    if (quicksave_str.size() > 0) {
        deserialize(quicksave_str, true);
        logger << "Quickload\n";
        if (history.getCurrent().type != HistoryEntry::QUICKLOAD) {
            history.save(HistoryEntry::QUICKLOAD);
        }
    } else {
        logger << "Can't quickload\n";
    }
}

Tool* Application::try_select_tool(int index) {
    if (tools.size() > index) {
        Tool* tool = tools[index];
        selected_tool = tool;
        create_tool.create_panel_widget->setVisible(tool == &create_tool);
        edit_tool.edit_window_widget->setVisible(tool == &edit_tool);
        for (size_t i = 0; i < tools.size(); i++) {
            tools[i]->widget->setFillColor(sf::Color(128, 128, 128));
        }
        tool->widget->setFillColor(sf::Color::Yellow);
        return tool;
    }
    return nullptr;
}

void Application::select_create_type(int type) {
    create_tool.type = static_cast<CreateTool::ObjectType>(type);
    for (size_t j = 0; j < create_tool.create_buttons.size(); j++) {
        create_tool.create_buttons[j]->setFillColor(sf::Color(128, 128, 128));
    }
    create_tool.create_buttons[type]->setFillColor(sf::Color(0, 175, 255));
}

void Application::toggle_pause() {
    paused = !paused;
    paused_rect_widget->setVisible(paused);
}

b2Vec2 Application::b2_screen_to_world(sf::Vector2i screen_pos) {
    sf::Vector2f pos = world_texture.mapPixelToCoords(screen_pos, world_view);
    return tob2(pos);
}

sf::Vector2f Application::sf_screen_to_world(sf::Vector2i screen_pos) {
    sf::Vector2f pos = world_texture.mapPixelToCoords(screen_pos, world_view);
    return pos;
}

sf::Vector2i Application::world_to_screen(b2Vec2 world_pos) {
    sf::Vector2i pos = world_texture.mapCoordsToPixel(tosf(world_pos), world_view);
    return pos;
}

sf::Vector2f Application::world_to_screenf(sf::Vector2f world_pos) {
    sf::Vector2i pos = world_texture.mapCoordsToPixel(world_pos, world_view);
    return to2f(pos);
}

sf::Vector2f Application::world_to_screenf(b2Vec2 world_pos) {
    sf::Vector2i vec2i = world_to_screen(world_pos);
    return to2f(vec2i);
}

sf::Vector2f Application::world_dir_to_screenf(b2Vec2 world_dir) {
    //TODO: calculate new direction using an actual sf::View
    return sf::Vector2f(world_dir.x, -world_dir.y);
}

b2Fixture* Application::get_fixture_at(sf::Vector2i screen_pos) {
    b2Vec2 world_pos = b2_screen_to_world(screen_pos);
    b2Vec2 world_pos_next = b2_screen_to_world(screen_pos + sf::Vector2i(1, 1));
    b2Vec2 midpoint = 0.5f * (world_pos + world_pos_next);
    QueryCallback callback;
    b2AABB aabb;
    aabb.upperBound = world_pos;
    aabb.lowerBound = world_pos_next;
    world->QueryAABB(&callback, aabb);
    for (size_t i = 0; i < callback.fixtures.size(); i++) {
        b2Fixture* fixture = callback.fixtures[i];
        if (fixture->TestPoint(midpoint)) {
            return fixture;
        }
    }
    return nullptr;
}

GameObject* Application::get_object_at(sf::Vector2i screen_pos) {
    GameObject* result = nullptr;
    b2Fixture* fixture = get_fixture_at(mousePos);
    if (fixture) {
        b2Body* body = fixture->GetBody();
        result = get_gameobject(body);
    }
    return result;
}

ptrdiff_t Application::mouse_get_ground_vertex() {
    ptrdiff_t closest_vertex_i = 0;
    sf::Vector2i closest_vertex_pos = world_to_screen(ground->getVertexPos(0));
    int closest_vertex_offset = utils::get_max_offset(closest_vertex_pos, mousePos);
    for (size_t i = 1; i < ground->getVertexCount(); i++) {
        sf::Vector2i vertex_pos = world_to_screen(ground->getVertexPos(i));
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

ptrdiff_t Application::mouse_get_ground_edge() {
    for (size_t i = 0; i < ground->getVertexCount() - 1; i++) {
        b2Vec2 p1 = ground->getVertexPos(i);
        b2Vec2 p2 = ground->getVertexPos(i + 1);
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
        b2Vec2 b2_p1 = tob2(world_to_screenf(p1));
        b2Vec2 b2_p2 = tob2(world_to_screenf(p2));
        float dist = utils::distance_to_line(b2_mouse_pos, b2_p1, b2_p2);
        if (dist <= EditTool::EDGE_HIGHLIGHT_DISTANCE) {
            return i;
        }
    }
    return -1;
}

ptrdiff_t Application::mouse_get_edge_vertex() {
    ptrdiff_t v_start_i = 0;
    ptrdiff_t v_end_i = ground->getVertexCount() - 1;
    sf::Vector2f v_start = world_to_screenf(ground->getVertexPos(v_start_i));
    sf::Vector2f v_end = world_to_screenf(ground->getVertexPos(v_end_i));
    float v_start_dist = utils::get_length(mousePosf - v_start);
    float v_end_dist = utils::get_length(mousePosf - v_end);
    if (v_start_dist <= v_end_dist) {
        return v_start_i;
    } else {
        return v_end_i;
    }
}

void Application::select_vertices_in_rect(const RectangleSelect& rectangle_select) {
    sf::Vector2f mpos = sfMousePosWorld;
    sf::Vector2f origin = rectangle_select.select_origin;
    float left = std::min(mpos.x, origin.x);
    float top = std::min(mpos.y, origin.y);
    float right = std::max(mpos.x, origin.x);
    float bottom = std::max(mpos.y, origin.y);
    float width = right - left;
    float height = bottom - top;
    sf::FloatRect rect(left, top, width, height);
    for (size_t i = 0; i < ground->getVertexCount(); i++) {
        if (utils::contains_point(rect, tosf(ground->getVertexPos(i)))) {
            ground->selectVertex(i);
        }
    }
}

void Application::select_objects_in_rect(const RectangleSelect& rectangle_select) {

}

void Application::render_rectangle_select(sf::RenderTarget& target, RectangleSelect& rectangle_select) {
    sf::Vector2f pos = world_to_screenf(rectangle_select.select_origin);
    rectangle_select.select_rect.setPosition(pos);
    rectangle_select.select_rect.setSize(mousePosf - pos);
    target.draw(rectangle_select.select_rect);
}

void Application::get_screen_normal(const b2Vec2& v1, const b2Vec2& v2, sf::Vector2f& norm_v1, sf::Vector2f& norm_v2) {
    b2Vec2 midpoint = 0.5f * (v1 + v2);
    norm_v1 = world_to_screenf(midpoint);
    sf::Vector2f edge_dir = utils::normalize(world_dir_to_screenf(v2 - v1));
    sf::Vector2f norm_dir = utils::rot90CCW(edge_dir);
    norm_v2 = norm_v1 + norm_dir * (float)EditTool::NORMAL_LENGTH;
}

void Application::get_screen_normal(const sf::Vector2i& v1, const sf::Vector2i& v2, sf::Vector2f& norm_v1, sf::Vector2f& norm_v2) {
    sf::Vector2f v1f = to2f(v1);
    sf::Vector2f v2f = to2f(v2);
    sf::Vector2f midpoint = (v1f + v2f) / 2.0f;
    norm_v1 = midpoint;
    sf::Vector2f edge_dir = utils::normalize(v2f - v1f);
    sf::Vector2f norm_dir = utils::rot90CW(edge_dir);
    norm_v2 = norm_v1 + norm_dir * (float)EditTool::NORMAL_LENGTH;
}

void Application::draw_line(sf::RenderTarget& target, const sf::Vector2f& v1, const sf::Vector2f& v2, const sf::Color& color) {
    line_primitive[0].position = v1;
    line_primitive[0].color = color;
    line_primitive[1].position = v2;
    line_primitive[1].color = color;
    target.draw(line_primitive);
}

GameObject* Application::get_gameobject(b2Body* body) {
    GameObject* gameobject = reinterpret_cast<GameObject*>(body->GetUserData().pointer);
    return gameobject;
}

BoxObject* Application::create_box(b2Vec2 pos, float angle, b2Vec2 size, sf::Color color) {
    b2BodyDef def;
    def.type = b2_dynamicBody;
    def.position = pos;
    def.angle = angle;
    std::unique_ptr<BoxObject> uptr = std::make_unique<BoxObject>(world.get(), def, size, color);
    BoxObject* ptr = uptr.get();
    game_objects.push_back(std::move(uptr));
    return ptr;
}

BallObject* Application::create_ball(b2Vec2 pos, float radius, sf::Color color, sf::Color notch_color) {
    b2BodyDef def;
    def.type = b2_dynamicBody;
    def.position = pos;
    std::unique_ptr<BallObject> uptr = std::make_unique<BallObject>(world.get(), def, radius, color, notch_color);
    BallObject* ptr = uptr.get();
    game_objects.push_back(std::move(uptr));
    return ptr;
}

CarObject* Application::create_car(b2Vec2 pos, std::vector<float> lengths, std::vector<float> wheels, sf::Color color) {
    b2BodyDef def;
    def.type = b2_dynamicBody;
    def.position = pos;
    std::unique_ptr<CarObject> uptr = std::make_unique<CarObject>(world.get(), def, lengths, wheels, color);
    CarObject* ptr = uptr.get();
    game_objects.push_back(std::move(uptr));
    return ptr;
}

GroundObject* Application::create_ground(b2Vec2 pos, std::vector<b2Vec2> vertices, sf::Color color) {
    b2BodyDef def;
    def.position = pos;
    std::unique_ptr<GroundObject> uptr = std::make_unique<GroundObject>(world.get(), def, vertices, color);
    GroundObject* ptr = uptr.get();
    game_objects.push_back(std::move(uptr));
    return ptr;
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

int FpsCounter::getFps() {
    return fps;
}
