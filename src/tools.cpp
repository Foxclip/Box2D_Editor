#include "../include/tools.h"

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

bool CreateTool::showInToolPanel() const {
    return true;
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

bool DragTool::showInToolPanel() const {
    return true;
}

void DragTool::reset() {
    mouse_body = nullptr;
    mouse_joint = nullptr;
}

MoveTool::MoveTool() : Tool() {
    name = "move";
}

bool MoveTool::showInToolPanel() const {
    return false;
}

void MoveTool::reset() {
    orig_cursor_pos = b2Vec2(0.0f, 0.0f);
    moving_objects = CompoundVector<GameObject*>();
}

RotateTool::RotateTool() : Tool() {
    name = "rotate";
}

bool RotateTool::showInToolPanel() const {
    return false;
}

void RotateTool::reset() {
    pivot_pos = b2Vec2(0.0f, 0.0f);
    orig_cursor_pos = b2Vec2(0.0f, 0.0f);
    rotating_objects = CompoundVector<GameObject*>();
    orig_mouse_angle = 0.0f;
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
}

bool EditTool::showInToolPanel() const {
    return false;
}

void EditTool::reset() {
    mode = HOVER;
    grabbed_vertex = -1;
    highlighted_vertex = -1;
    highlighted_edge = -1;
    edge_vertex = -1;
    rectangle_select.reset();
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

bool SelectTool::showInToolPanel() const {
    return true;
}

size_t SelectTool::selectedCount() const {
    return selected_objects.size();
}

const CompoundVector<GameObject*>& SelectTool::getSelectedObjects() const {
    return selected_objects;
}

void SelectTool::setSelected(const CompoundVector<GameObject*> vec) {
    clearSelected();
    for (size_t i = 0; i < vec.size(); i++) {
        selected_objects.add(vec[i]);
    }
}

void SelectTool::selectObject(GameObject* object, bool with_children) {
    if (object) {
        object->selected = true;
        selected_objects.add(object);
        if (with_children) {
            for (size_t i = 0; i < object->getChildren().size(); i++) {
                selectObject(object->getChild(i), true);
            }
        }
    }
}

void SelectTool::deselectObject(GameObject* object, bool with_children) {
    if (object) {
        object->selected = false;
        selected_objects.remove(object);
        if (with_children) {
            for (size_t i = 0; i < object->getChildren().size(); i++) {
                deselectObject(object->getChild(i), true);
            }
        }
    }
}

void SelectTool::toggleSelect(GameObject* object, bool with_children) {
    if (object) {
        if (object->selected) {
            deselectObject(object, with_children);
        } else {
            selectObject(object, with_children);
        }
    }
}

void SelectTool::reset() {
    hover_object = nullptr;
    clearSelected();
    rectangle_select.reset();
}

void SelectTool::clearSelected() {
    for (GameObject* obj : selected_objects) {
        if (obj) {
            obj->selected = false;
        }
    }
    selected_objects.clear();
}

void SelectTool::clearRectSelected() {
    for (GameObject* obj : objects_in_rect) {
        deselectObject(obj);
    }
    objects_in_rect.clear();
}

void SelectTool::selectSingleObject(GameObject* object, bool with_children) {
    clearSelected();
    selectObject(object, with_children);
}

void SelectTool::addToRectSelection(GameObject* object) {
    if (object && !object->selected) {
        selectObject(object);
        objects_in_rect.add(object);
    }
}

void SelectTool::applyRectSelection() {
    objects_in_rect.clear();
}

RectangleSelect::RectangleSelect() {
    select_rect.setFillColor(sf::Color::Transparent);
    select_rect.setOutlineThickness(-1.0f);
    select_rect.setOutlineColor(sf::Color::Yellow);
}

void RectangleSelect::reset() {
    active = false;
    select_origin = sf::Vector2f(0.0f, 0.0f);
}
