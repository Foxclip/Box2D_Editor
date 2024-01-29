#include "tools.h"

CreateTool create_tool;
DragTool drag_tool;
MoveTool move_tool;
RotateTool rotate_tool;
EditTool edit_tool;
std::vector<Tool*> tools = { &create_tool, &drag_tool, &move_tool, &rotate_tool, &edit_tool };

Tool::Tool() {
    shape.setSize(sf::Vector2f(TOOL_RECT_WIDTH, TOOL_RECT_HEIGHT));
    shape.setFillColor(sf::Color(128, 128, 128));
    shape.setOutlineColor(sf::Color::Yellow);
}

CreateTool::CreateTool() : Tool() {
    name = "create";
}

DragTool::DragTool() : Tool() {
    name = "drag";
}

MoveTool::MoveTool() : Tool() {
    name = "move";
}

RotateTool::RotateTool() : Tool() {
    name = "rotate";
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
