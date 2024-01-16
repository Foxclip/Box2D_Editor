#include "application.h"
#include "utils.h"
#include <numbers>
#include <iostream>

std::vector<std::unique_ptr<Tool>> tools;

const auto tob2 = utils::tob2;
const auto tosf = utils::tosf;
const auto to2i = utils::to2i;
const auto to2f = utils::to2f;

void Application::init() {
    sf::ContextSettings cs;
    cs.antialiasingLevel = ANTIALIASING;
    window = std::make_unique<sf::RenderWindow>(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "SFML", sf::Style::Default, cs);
    window->setVerticalSyncEnabled(true);
    selected_tool = create_tool("drag");
    create_tool("move");
    create_tool("edit");
    init_ui();
    init_objects();
    world_view = sf::View(sf::FloatRect(0.0f, 0.0f, WINDOW_WIDTH, WINDOW_HEIGHT));
    ui_view = sf::View(sf::FloatRect(0.0f, 0.0f, WINDOW_WIDTH, WINDOW_HEIGHT));
    maximize_window();
    assert(selected_tool);
}

void Application::start() {
    main_loop();
}

void Application::init_ui() {
    std::string ui_font_filename = "STAN0757.TTF";
    if (!ui_font.loadFromFile(ui_font_filename)) {
        throw std::runtime_error("Font loading error (" + ui_font_filename + ")");
    }
    {
        assert(VERTEX_SIZE % 2 == 1);
        vertex_rect.setSize(sf::Vector2f(VERTEX_SIZE, VERTEX_SIZE));
        vertex_rect.setFillColor(sf::Color::Red);
        vertex_rect.setOrigin(0.0f, 0.0f);
    }
    {
        int vertex_distance = VERTEX_EDITOR_DISTANCE * 2 + 1;
        vertex_editor_rect.setSize(sf::Vector2f(vertex_distance, vertex_distance));
        vertex_editor_rect.setFillColor(sf::Color::Transparent);
        vertex_editor_rect.setOutlineThickness(-1.0f);
        vertex_editor_rect.setOutlineColor(sf::Color::Yellow);
        vertex_editor_rect.setOrigin(0.0f, 0.0f);
    }
    {
        paused_text.setFont(ui_font);
        paused_text.setString("PAUSED");
        paused_text.setCharacterSize(24);
        paused_text.setFillColor(sf::Color::Yellow);
        sf::FloatRect text_bounds = paused_text.getLocalBounds();
        paused_text.setOrigin(text_bounds.left, text_bounds.top);
        const int rect_padding = 10;
        paused_text.setPosition(rect_padding, rect_padding);
        paused_rect.setSize(sf::Vector2f(text_bounds.width + rect_padding * 2, text_bounds.height + rect_padding * 2));
        paused_rect.setFillColor(sf::Color(0, 0, 0, 128));
        paused_rect.setOrigin(0.0f, 0.0f);
        paused_rect.setPosition(0.0f, 0.0f);
    }
    {
        int tools_width = TOOL_RECT_SIZE * tools.size();
        int padding_width = TOOLBOX_PADDING * (tools.size() + 1);
        int toolbox_width = tools_width + padding_width;
        int toolbox_height = TOOLBOX_PADDING * 2 + TOOL_RECT_SIZE;
        toolbox_rect.setSize(sf::Vector2f(toolbox_width, toolbox_height));
        toolbox_rect.setFillColor(sf::Color::Red);
        toolbox_rect.setOrigin(sf::Vector2f(toolbox_width / 2, 0.0f));
        tool_text.setFont(ui_font);
        tool_text.setCharacterSize(16);
        tool_text.setFillColor(sf::Color::Black);
    }
}

void Application::init_objects() {
    b2Vec2 gravity(0.0f, -9.8f);
    world = std::make_unique<b2World>(gravity);

    b2BodyDef mouse_body_def;
    mouse_body = world->CreateBody(&mouse_body_def);

    std::vector<b2Vec2> ground_vertices = {
        b2Vec2(25.0f, 8.0f),
        b2Vec2(15.0f, 2.0f),
        b2Vec2(5.0f, 0.0f),
        b2Vec2(-5.0f, 0.0f),
        b2Vec2(-15.0f, 2.0f),
        b2Vec2(-25.0f, 8.0f),
    };
    ground = create_ground(b2Vec2(0.0f, 0.0f), ground_vertices, sf::Color::White);

    GameObject* box0 = create_box(b2Vec2(0.0f, 1.0f), utils::to_radians(0.0f), b2Vec2(1.0f, 1.0f), sf::Color::Green);
    GameObject* box1 = create_box(b2Vec2(0.1f, 2.0f), utils::to_radians(0.0f), b2Vec2(1.0f, 1.0f), sf::Color::Green);
    GameObject* box2 = create_box(b2Vec2(0.2f, 3.0f), utils::to_radians(0.0f), b2Vec2(1.0f, 1.0f), sf::Color::Green);
    GameObject* ball = create_ball(b2Vec2(0.0f, 5.0f), 0.5f, sf::Color::Green, sf::Color(0, 64, 0));
    std::vector<GameObject*> dynamic_objects = { box0, box1, box2, ball };
    //std::vector<GameObject*> boxes = { box1, box2 };
    //std::vector<GameObject*> boxes = { box0 };
    for (int i = 0; i < dynamic_objects.size(); i++) {
        GameObject* box = dynamic_objects[i];
        box->SetType(b2_dynamicBody, false);
        box->SetDensity(1.0f, false);
        box->SetFriction(0.3f, false);
        box->SetRestitution(0.5f, false);
    }
    //b2DistanceJointDef distance_joint_def;
    //distance_joint_def.Initialize(box1->rigid_body, box2->rigid_body, box1->rigid_body->GetWorldCenter(), box2->rigid_body->GetWorldCenter());
    //b2DistanceJoint* distance_joint = (b2DistanceJoint*)world->CreateJoint(&distance_joint_def);

    std::vector<float> lengths = { 5.0f, 1.0f, 5.0f, 1.0f, 5.0f, 1.0f };
    std::vector<float> wheels = { 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f };
    GameObject* car = create_car(b2Vec2(0.0f, 10.0f), lengths, wheels, sf::Color::Red);
    car->SetType(b2_dynamicBody, false);
    car->SetDensity(1.0f, false);
    car->SetFriction(0.3f, false);
    car->SetRestitution(0.5f, false);
}

void Application::main_loop() {
    while (window->isOpen()) {
        process_input();
        process_world();
        render();
    }
}

void Application::resetView() {
    viewCenterX = 0.0f;
    viewCenterY = 0.0f;
    zoomFactor = 1.0f;
}

void Application::process_input() {
    sf::Event event;
    while (window->pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window->close();
        }
        process_keyboard_event(event);
        process_mouse_event(event);
    }
    process_keyboard();
    process_mouse();
}

void Application::process_keyboard_event(sf::Event event) {
    if (event.type == sf::Event::KeyPressed) {
        switch (event.key.code) {
            case sf::Keyboard::R: resetView(); break;
            case sf::Keyboard::Space: paused = !paused; break;
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
        }
    }
    if (event.type == sf::Event::KeyReleased) {
        switch (event.key.code) { }
    }
}

void Application::process_mouse_event(sf::Event event) {
    mousePos = sf::Mouse::getPosition(*window);
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
                grabbed_vertex = -1;
                if (moving_object) {
                    //TODO: remember state for all children
                    moving_object->SetEnabled(moving_body_was_enabled, true);
                }
                moving_object = nullptr;
                if (mouse_joint) {
                    world->DestroyJoint(mouse_joint);
                    mouse_joint = nullptr;
                }
                break;
        }
    }
    if (event.type == sf::Event::MouseWheelScrolled) {
        if (event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel) {
            zoomFactor *= pow(MOUSE_SCROLL_ZOOM, -event.mouseWheelScroll.delta);
        }
    }
}

void Application::process_keyboard() {
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
        window->close();
    }
}

void Application::process_mouse() {
    if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
        if (grabbed_vertex != -1) {
            ground->move_vertex(grabbed_vertex, b2MousePosWorld);
        }
        if (moving_object) {
            moving_object->SetPosition(b2MousePosWorld - moving_body_offset, true);
        }
        if (mouse_joint) {
            mouse_joint->SetTarget(b2MousePosWorld);
        }
    }
    if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
        sf::Vector2i mouseDelta = mousePrevPos - mousePos;
        viewCenterX += mouseDelta.x * zoomFactor;
        viewCenterY += -mouseDelta.y * zoomFactor;
    }
    mousePrevPos = mousePos;
}

void Application::process_left_click() {
    for (int tool_i = 0; tool_i < tools.size(); tool_i++) {
        Tool* tool = tools[tool_i].get();
        if (utils::contains_point(tool->shape, to2f(mousePos))) {
            selected_tool = tool;
            return;
        }
    }
    if (selected_tool->name == "drag") {
        b2Fixture* grabbed_fixture = get_fixture_at(mousePos);
        if (grabbed_fixture) {
            b2Body* grabbed_body = grabbed_fixture->GetBody();
            b2MouseJointDef mouse_joint_def;
            mouse_joint_def.bodyA = mouse_body;
            mouse_joint_def.bodyB = grabbed_body;
            mouse_joint_def.damping = 1.0f;
            mouse_joint_def.maxForce = 5000.0f * grabbed_body->GetMass();
            mouse_joint_def.stiffness = 50.0f;
            mouse_joint_def.target = b2MousePosWorld;
            mouse_joint = (b2MouseJoint*)world->CreateJoint(&mouse_joint_def);
        }
    } else if (selected_tool->name == "move") {
        b2Fixture* fixture = get_fixture_at(mousePos);
        if (fixture) {
            b2Body* body = fixture->GetBody();
            GameObject* gameobject = reinterpret_cast<GameObject*>(body->GetUserData().pointer);
            moving_body_was_enabled = body->IsEnabled();
            moving_body_offset = b2MousePosWorld - body->GetPosition();
            gameobject->SetEnabled(false, true);
            gameobject->SetLinearVelocity(b2Vec2(0.0f, 0.0f), true);
            gameobject->SetAngularVelocity(0.0f, true);
            moving_object = gameobject;
        }
    } else if (selected_tool->name == "edit") {
        int index;
        sf::Vector2i position;
        if (mouse_get_ground_vertex(index, position)) {
            grabbed_vertex = index;
        }
    }
}

void Application::process_world() {
    if (!paused) {
        world->Step(timeStep, VELOCITY_ITERATIONS, POSITION_ITERATIONS);
    }
}

void Application::render() {
    window->clear();
    render_world();
    render_ui();
    window->display();
}

void Application::render_world() {
    world_view.setCenter(viewCenterX, viewCenterY);
    world_view.setSize(window->getSize().x * zoomFactor, -1.0f * window->getSize().y * zoomFactor);
    window->setView(world_view);

    for (int i = 0; i < game_objects.size(); i++) {
        GameObject* object = game_objects[i].get();
        object->UpdateVisual();
        window->draw(*object->drawable.get());
    }

    if (mouse_joint) {
        sf::Vector2f grabbed_point = tosf(mouse_joint->GetAnchorB());
        line_primitive[0].position = grabbed_point;
        line_primitive[0].color = sf::Color::Yellow;
        line_primitive[1].position = sfMousePosWorld;
        line_primitive[1].color = sf::Color::Yellow;
        window->draw(line_primitive);
    }
}

void Application::render_ui() {
    ui_view.setCenter(window->getSize().x / 2.0f, window->getSize().y / 2.0f);
    ui_view.setSize(window->getSize().x, window->getSize().y);
    window->setView(ui_view);

    if (selected_tool->name == "edit") {
        b2ChainShape* chain = static_cast<b2ChainShape*>(ground->rigid_body->GetFixtureList()->GetShape());
        for (int i = 0; i < chain->m_count; i++) {
            int vertex_size = VERTEX_SIZE / 2;
            vertex_rect.setPosition(world_to_screenf(chain->m_vertices[i]) - sf::Vector2f(vertex_size, vertex_size));
            window->draw(vertex_rect);
        }
        int index;
        sf::Vector2i position;
        if (mouse_get_ground_vertex(index, position)) {
            int vertex_distance = VERTEX_EDITOR_DISTANCE;
            vertex_editor_rect.setPosition(to2f(position) - sf::Vector2f(vertex_distance, vertex_distance));
            window->draw(vertex_editor_rect);
        }
    }

    toolbox_rect.setPosition(window->getSize().x / 2, 0.0f);
    //window->draw(toolbox_rect);
    sf::FloatRect toolbox_bounds = toolbox_rect.getGlobalBounds();
    sf::Vector2f toolbox_corner = sf::Vector2f(toolbox_bounds.left, toolbox_bounds.top);
    for (int i = 0; i < tools.size(); i++) {
        Tool* tool = tools[i].get();
        sf::RectangleShape& tool_rect = tool->shape;
        int x = i * (TOOLBOX_PADDING + TOOL_RECT_SIZE) + TOOLBOX_PADDING;
        int y = TOOLBOX_PADDING;
        sf::Vector2f pos = toolbox_corner + sf::Vector2f(x, y);
        tool_rect.setPosition(pos);
        if (utils::contains_point(tool_rect, to2f(mousePos))) {
            tool_rect.setOutlineThickness(1.0f);
        } else {
            tool_rect.setOutlineThickness(0.0f);
        }
        if (tool == selected_tool) {
            tool_rect.setFillColor(sf::Color::Yellow);
        } else {
            tool_rect.setFillColor(sf::Color(128, 128, 128));
        }
        tool_text.setString(tools[i]->name);
        tool_text.setPosition(tool_rect.getPosition() + tool_rect.getSize() / 2.0f);
        utils::set_origin_to_center_normal(tool_text);
        window->draw(tool_rect);
        window->draw(tool_text);
    }

    if (paused) {
        window->draw(paused_rect);
        window->draw(paused_text);
    }
}

void Application::maximize_window() {
    sf::WindowHandle windowHandle = window->getSystemHandle();
    ShowWindow(windowHandle, SW_MAXIMIZE);
}

Tool* Application::create_tool(std::string name) {
    std::unique_ptr<Tool> uptr = std::make_unique<Tool>(name);
    Tool* ptr = uptr.get();
    tools.push_back(std::move(uptr));
    return ptr;
}

Tool* Application::try_select_tool(int index) {
    if (tools.size() > index) {
        selected_tool = tools[index].get();
        return selected_tool;
    }
    return nullptr;
}

b2Vec2 Application::b2_screen_to_world(sf::Vector2i screen_pos) {
    sf::Vector2f pos = window->mapPixelToCoords(screen_pos, world_view);
    return tob2(pos);
}

sf::Vector2f Application::sf_screen_to_world(sf::Vector2i screen_pos) {
    sf::Vector2f pos = window->mapPixelToCoords(screen_pos, world_view);
    return pos;
}

sf::Vector2i Application::world_to_screen(b2Vec2 world_pos) {
    sf::Vector2i pos = window->mapCoordsToPixel(tosf(world_pos), world_view);
    return pos;
}

sf::Vector2f Application::world_to_screenf(b2Vec2 world_pos) {
    sf::Vector2i vec2i = world_to_screen(world_pos);
    return sf::Vector2f(vec2i.x, vec2i.y);
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
    for (int i = 0; i < callback.fixtures.size(); i++) {
        b2Fixture* fixture = callback.fixtures[i];
        if (fixture->TestPoint(midpoint)) {
            return fixture;
        }
    }
    return nullptr;
}

bool Application::mouse_get_ground_vertex(int& p_index, sf::Vector2i& p_position) {
    b2ChainShape* chain = static_cast<b2ChainShape*>(ground->rigid_body->GetFixtureList()->GetShape());
    int closest_vertex_i = 0;
    sf::Vector2i closest_vertex_pos = world_to_screen(chain->m_vertices[0]);
    int closest_vertex_offset = utils::get_max_offset(closest_vertex_pos, mousePos);
    for (int i = 1; i < chain->m_count; i++) {
        sf::Vector2i vertex_pos = world_to_screen(chain->m_vertices[i]);
        float offset = utils::get_max_offset(vertex_pos, mousePos);
        if (offset < closest_vertex_offset) {
            closest_vertex_i = i;
            closest_vertex_pos = vertex_pos;
            closest_vertex_offset = offset;
        }
    }
    if (closest_vertex_offset <= VERTEX_EDITOR_DISTANCE) {
        p_index = closest_vertex_i;
        p_position = closest_vertex_pos;
        return true;
    }
    return false;
}

GameObject* Application::create_box(b2Vec2 pos, float angle, b2Vec2 size, sf::Color color) {
    b2BodyDef bodyDef;
    bodyDef.position = pos;
    bodyDef.angle = angle;
    b2Body* body = world->CreateBody(&bodyDef);
    b2PolygonShape box_shape;
    box_shape.SetAsBox(size.x / 2.0f, size.y / 2.0f);
    b2Fixture* fixture = body->CreateFixture(&box_shape, 1.0f);
    std::unique_ptr<sf::Shape> shape = std::make_unique<sf::RectangleShape>(tosf(size));
    shape->setOrigin(size.x / 2.0f, size.y / 2.0f);
    shape->setFillColor(color);
    std::unique_ptr<GameObject> uptr = std::make_unique<GameObject>(std::move(shape), body);
    GameObject* ptr = uptr.get();
    body->GetUserData().pointer = reinterpret_cast<uintptr_t>(ptr);
    game_objects.push_back(std::move(uptr));
    return ptr;
}

GameObject* Application::create_ball(b2Vec2 pos, float radius, sf::Color color, sf::Color notch_color) {
    b2BodyDef bodyDef;
    bodyDef.position = pos;
    b2Body* body = world->CreateBody(&bodyDef);
    b2CircleShape circle_shape;
    circle_shape.m_radius = radius;
    b2Fixture* fixture = body->CreateFixture(&circle_shape, 1.0f);
    std::unique_ptr<CircleNotchShape> circle_notch_shape = std::make_unique<CircleNotchShape>(radius, 30, 4);
    circle_notch_shape->setCircleColor(color);
    circle_notch_shape->setNotchColor(notch_color);
    std::unique_ptr<GameObject> uptr = std::make_unique<GameObject>(std::move(circle_notch_shape), body);
    GameObject* ptr = uptr.get();
    body->GetUserData().pointer = reinterpret_cast<uintptr_t>(ptr);
    game_objects.push_back(std::move(uptr));
    return ptr;
}

CarObject* Application::create_car(b2Vec2 pos, std::vector<float> lengths, std::vector<float> wheels, sf::Color color) {
    std::unique_ptr<sf::ConvexShape> shape = std::make_unique<sf::ConvexShape>(lengths.size());
    float pi = std::numbers::pi;
    auto get_pos = [&](int i) {
        float angle = (float)i / lengths.size() * 2 * pi;
        b2Vec2 vector = b2Vec2(std::cos(angle), std::sin(angle));
        int index = i < lengths.size() ? i : i % lengths.size();
        b2Vec2 pos = lengths[index] * vector;
        return pos;
    };
    b2BodyDef bodyDef;
    bodyDef.position = pos;
    b2Body* body = world->CreateBody(&bodyDef);
    std::vector<GameObject*> wheel_objects;
    std::vector<b2RevoluteJoint*> wheel_joints;
    for (int i = 0; i < lengths.size(); i++) {
        b2Vec2 vertices[3];
        vertices[0] = b2Vec2(0.0f, 0.0f);
        vertices[1] = get_pos(i);
        vertices[2] = get_pos(i + 1);
        b2PolygonShape triangle;
        triangle.Set(vertices, 3);
        b2Fixture* fixture = body->CreateFixture(&triangle, 1.0f);
        if (wheels[i] > 0.0f) {
            b2Vec2 anchor_pos = vertices[1];
            b2Vec2 anchor_pos_world = pos + anchor_pos;
            GameObject* wheel = create_ball(anchor_pos_world, wheels[i], sf::Color::Yellow, sf::Color(64, 64, 0));
            wheel->SetType(b2_dynamicBody, false);
            wheel->SetDensity(1.0f, false);
            wheel->SetFriction(0.3f, false);
            wheel->SetRestitution(0.5f, false);
            wheel_objects.push_back(wheel);
            b2RevoluteJointDef wheel_joint_def;
            wheel_joint_def.Initialize(body, wheel->rigid_body, anchor_pos_world);
            wheel_joint_def.maxMotorTorque = 30.0f;
            wheel_joint_def.motorSpeed = -10.0f;
            wheel_joint_def.enableMotor = true;
            b2RevoluteJoint* wheel_joint = (b2RevoluteJoint*)world->CreateJoint(&wheel_joint_def);
            wheel_joints.push_back(wheel_joint);
        }
        shape->setPoint(i, tosf(vertices[1]));
    }

    shape->setFillColor(color);
    std::unique_ptr<CarObject> uptr = std::make_unique<CarObject>(std::move(shape), body, wheel_objects, wheel_joints);
    uptr->position = pos;
    CarObject* ptr = uptr.get();
    body->GetUserData().pointer = reinterpret_cast<uintptr_t>(ptr);
    game_objects.push_back(std::move(uptr));
    return ptr;
}

GroundObject* Application::create_ground(b2Vec2 pos, std::vector<b2Vec2> vertices, sf::Color color) {
    b2BodyDef bodyDef;
    bodyDef.position = pos;
    b2Body* body = world->CreateBody(&bodyDef);
    b2ChainShape chain;
    chain.CreateChain(vertices.data(), vertices.size(), vertices.front(), vertices.back());
    b2Fixture* fixture = body->CreateFixture(&chain, 1.0f);
    sf::VertexArray drawable_vertices(sf::LinesStrip, vertices.size());
    for (int i = 0; i < vertices.size(); i++) {
        drawable_vertices[i].position = tosf(vertices[i]);
    }
    std::unique_ptr<LineStripShape> line_strip = std::make_unique<LineStripShape>(drawable_vertices);
    line_strip->setLineColor(color);
    std::unique_ptr<GroundObject> uptr = std::make_unique<GroundObject>(std::move(line_strip), body);
    GroundObject* ptr = uptr.get();
    body->GetUserData().pointer = reinterpret_cast<uintptr_t>(ptr);
    game_objects.push_back(std::move(uptr));
    return ptr;
}

bool QueryCallback::ReportFixture(b2Fixture* fixture) {
    fixtures.push_back(fixture);
    return true;
}

Tool::Tool(std::string name) {
    this->name = name;
    shape.setSize(sf::Vector2f(TOOL_RECT_SIZE, TOOL_RECT_SIZE));
    shape.setFillColor(sf::Color(128, 128, 128));
    shape.setOutlineColor(sf::Color::Yellow);
}
