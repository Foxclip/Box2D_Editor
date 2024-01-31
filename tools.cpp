#include "tools.h"

SelectTool select_tool;
CreateTool create_tool;
DragTool drag_tool;
MoveTool move_tool;
RotateTool rotate_tool;
EditTool edit_tool;
std::vector<Tool*> tools = { &select_tool, &drag_tool, &move_tool, &rotate_tool, &edit_tool, &create_tool, };

Tool::Tool() { }

void CreateTool::reset() { }

CreateTool::CreateTool() : Tool() {
    name = "create";
}

std::string CreateTool::create_type_name(ObjectType type) {
    switch (type) {
        case CreateTool::BOX: return "box"; break;
        case CreateTool::BALL: return "ball"; break;
    }
}

DragTool::DragTool() : Tool() {
    name = "drag";
}

void DragTool::reset() {
    mouse_body = nullptr;
    mouse_joint = nullptr;
}

MoveTool::MoveTool() : Tool() {
    name = "move";
}

void MoveTool::reset() {
    object = nullptr;
    object_was_enabled = false;
    offset = b2Vec2(0.0f, 0.0f);
}

RotateTool::RotateTool() : Tool() {
    name = "rotate";
}

void RotateTool::reset() {
    object = nullptr;
    object_was_enabled = false;
    angle_offset = 0.0f;
}

EditTool::EditTool() : Tool() {
    name = "edit";
    assert(VERTEX_SIZE % 2 == 1);
    vertex_rect.setSize(sf::Vector2f(VERTEX_SIZE, VERTEX_SIZE));
    vertex_rect.setFillColor(sf::Color::Red);
    vertex_rect.setOrigin(vertex_rect.getSize() / 2.0f);
    int vertex_distance = VERTEX_HIGHLIGHT_DISTANCE * 2 + 1;
    vertex_highlight_rect.setSize(sf::Vector2f(vertex_distance, vertex_distance));
    vertex_highlight_rect.setFillColor(sf::Color::Transparent);
    vertex_highlight_rect.setOutlineThickness(-1.0f);
    vertex_highlight_rect.setOutlineColor(sf::Color::Yellow);
    vertex_highlight_rect.setOrigin(vertex_highlight_rect.getSize() / 2.0f);
    edge_highlight.setFillColor(sf::Color::Yellow);
    edge_highlight.setOrigin(sf::Vector2f(0.0f, 1.5f));
    select_rect.setFillColor(sf::Color::Transparent);
    select_rect.setOutlineThickness(-1.0f);
    select_rect.setOutlineColor(sf::Color::Yellow);
}

void EditTool::reset() {
    mode = HOVER;
    grabbed_vertex = -1;
    highlighted_vertex = -1;
    highlighted_edge = -1;
    edge_vertex = -1;
    selection = false;
    select_origin = sf::Vector2f(0.0f, 0.0f);
    grabbed_vertex_offset = b2Vec2(0.0f, 0.0f);
    insertVertexPos = b2Vec2(0.0f, 0.0f);
}

std::string EditTool::modeToStr(EditToolMode mode) {
    switch (mode) {
        case EditTool::HOVER: return "HOVER";
        case EditTool::SELECT: return "SELECT";
        case EditTool::ADD: return "ADD";
        case EditTool::INSERT: return "INSERT";
        case EditTool::MOVE: return "MOVE";
        default: return "Unknown";
    }
}

SelectTool::SelectTool() {
    name = "select";
}

void SelectTool::reset() {
    selected_object = nullptr;
}
