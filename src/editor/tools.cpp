#include "editor/tools.h"

Tool::Tool() { }

void CreateTool::reset() { }

CreateTool::CreateTool() : Tool() {
    name = "create";
}

std::string CreateTool::create_type_name(ObjectType type) {
    switch (type) {
        case CreateTool::BOX: return "box";
        case CreateTool::BALL: return "ball";
        default: mAssert(false, "Unknown create object type"); return "unknown";
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
    orig_cursor_pos = b2Vec2(0.0f, 0.0f);
    moving_objects = CompVector<GameObject*>();
    selected_tool = nullptr;
}

RotateTool::RotateTool() : Tool() {
    name = "rotate";
}

void RotateTool::reset() {
    pivot_pos = b2Vec2(0.0f, 0.0f);
    orig_cursor_pos = b2Vec2(0.0f, 0.0f);
    rotating_objects = CompVector<GameObject*>();
    orig_mouse_angle = 0.0f;
    selected_tool = nullptr;
}

EditTool::EditTool() : Tool() {
    name = "edit";
    assert(VERTEX_SIZE % 2 == 1);
    vertex_rect.setSize(sf::Vector2f(VERTEX_SIZE, VERTEX_SIZE));
    vertex_rect.setFillColor(sf::Color::Red);
    vertex_rect.setOrigin(vertex_rect.getSize() / 2.0f);
    float vertex_distance = VERTEX_HIGHLIGHT_DISTANCE * 2.0f + 1.0f;
    vertex_highlight_rect.setSize(sf::Vector2f(vertex_distance, vertex_distance));
    vertex_highlight_rect.setFillColor(sf::Color::Transparent);
    vertex_highlight_rect.setOutlineThickness(-1.0f);
    vertex_highlight_rect.setOutlineColor(sf::Color::Yellow);
    vertex_highlight_rect.setOrigin(vertex_highlight_rect.getSize() / 2.0f);
    edge_highlight.setFillColor(sf::Color::Yellow);
    edge_highlight.setOrigin(sf::Vector2f(0.0f, 1.5f));
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
    selected_tool = nullptr;
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

size_t SelectTool::selectedCount() const {
    return selected_objects.size();
}

const CompVector<GameObject*>& SelectTool::getSelectedObjects() const {
    return selected_objects;
}

void SelectTool::setSelected(const CompVector<GameObject*>& vec) {
    clearSelected();
    for (size_t i = 0; i < vec.size(); i++) {
        GameObject* object = vec[i];
        selected_objects.add(object);
        OnObjectSelected(object);
    }
}

void SelectTool::selectObject(GameObject* object, bool with_children) {
    if (object) {
        object->selected = true;
        selected_objects.add(object);
        OnObjectSelected(object);
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
        OnObjectDeselected(object);
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
            OnObjectDeselected(obj);
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
