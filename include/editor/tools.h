#pragma once

#include "simulation/gameobject.h"
#include "common/event.h"
#include <set>

const float TOOL_RECT_WIDTH = 60.0f;
const float TOOL_RECT_HEIGHT = 40.0f;
const int TOOL_TEXT_SIZE = 16;
const float CREATE_RECT_WIDTH = 40.0f;
const float CREATE_RECT_HEIGHT = 40.0f;
const int CREATE_PANEL_PADDING = 10;
const b2Vec2 NEW_BOX_SIZE = b2Vec2(1.0f, 1.0f);
const sf::Color NEW_BOX_COLOR = sf::Color(0, 255, 0);
const float NEW_BALL_RADIUS = 0.5f;
const sf::Color NEW_BALL_COLOR = sf::Color(0, 255, 0);
const sf::Color NEW_BALL_NOTCH_COLOR = sf::Color(0, 64, 0);
const float ROTATE_ANGLE_STEP = 5.0f;

class RectangleSelect {
public:
	bool active = false;
	sf::Vector2f select_origin = sf::Vector2f(0.0f, 0.0f);
	sf::RectangleShape select_rect;

	RectangleSelect();
	void reset();
};

// add new tool:
//     create class inherited from Tool in tools.h
//     add extern global variable at the end of tools.h
//     define that variable in tools.cpp
//     add it to tools vector in tools.cpp
//     set it's display name in the contructor
//     write reset method which resets all fields that need to be reset during scene deserialization

namespace fw {
	class Widget;
}

class Tool {
public:
	std::string name;
	fw::Widget* widget = nullptr;
	Event<bool> OnSetSelected;
	Event<bool> OnSetSelectedWithButton;

	Tool();
	virtual void reset() = 0;

private:
};

class SelectTool : public Tool {
public:
	GameObject* hover_object = nullptr;
	RectangleSelect rectangle_select;
	CompVector<GameObject*> objects_in_rect;
	Event<GameObject*> OnObjectSelected;
	Event<GameObject*> OnObjectDeselected;

	SelectTool();
	size_t selectedCount() const;
	const CompVector<GameObject*>& getSelectedObjects() const;
	void setSelected(const CompVector<GameObject*>& vec);
	void selectObject(GameObject* object, bool with_children = false);
	void deselectObject(GameObject* object, bool with_children = false);
	void toggleSelect(GameObject* object, bool with_children = false);
	void selectSingleObject(GameObject* object, bool with_children = false);
	void addToRectSelection(GameObject* object);
	void applyRectSelection();
	void clearSelected();
	void clearRectSelected();
	void reset() override;

private:
	CompVector<GameObject*> selected_objects;

};

class DragTool : public Tool {
public:
	dp::DataPointerUnique<b2Body, std::function<void(b2Body*)>> mouse_body = dp::DataPointerUnique<b2Body, std::function<void(b2Body*)>>("Mouse body", nullptr);
	dp::DataPointerUnique<b2MouseJoint, std::function<void(b2MouseJoint*)>> mouse_joint = dp::DataPointerUnique<b2MouseJoint, std::function<void(b2MouseJoint*)>>("Mouse joint", nullptr);

	DragTool();
	void reset() override;

private:
};

class MoveTool : public Tool {
public:
	b2Vec2 orig_cursor_pos = b2Vec2(0.0f, 0.0f);
	CompVector<GameObject*> moving_objects;
	Tool* selected_tool = nullptr;

	MoveTool();
	void reset() override;

private:
};

class RotateTool : public Tool {
public:
	b2Vec2 pivot_pos = b2Vec2(0.0f, 0.0f);
	b2Vec2 orig_cursor_pos = b2Vec2(0.0f, 0.0f);
	CompVector<GameObject*> rotating_objects;
	float orig_mouse_angle = 0.0f;
	Tool* selected_tool = nullptr;

	RotateTool();
	void reset() override;

private:
};

class EditWindow;

class EditTool : public Tool {
public:
	static const int VERTEX_SIZE = 9;
	static const int VERTEX_HIGHLIGHT_DISTANCE = 10;
	static const int EDGE_HIGHLIGHT_DISTANCE = 10;
	static const int NORMAL_LENGTH = 30;
	enum EditToolMode {
		HOVER,
		SELECT,
		ADD,
		INSERT,
		MOVE,
	};
	EditToolMode mode = HOVER;
	ptrdiff_t grabbed_vertex = -1;
	ptrdiff_t highlighted_vertex = -1;
	ptrdiff_t highlighted_edge = -1;
	ptrdiff_t edge_vertex = -1;
	b2Vec2 grabbed_vertex_offset = b2Vec2(0.0f, 0.0f);
	b2Vec2 insertVertexPos = b2Vec2(0.0f, 0.0f);
	RectangleSelect rectangle_select;
	sf::RectangleShape vertex_highlight_rect;
	sf::RectangleShape vertex_rect;
	sf::RectangleShape edge_highlight;
	EditWindow* edit_window_widget = nullptr;
	Tool* selected_tool = nullptr;

	EditTool();
	void reset() override;
	static std::string modeToStr(EditToolMode mode);

};

class CreatePanel;

class CreateTool : public Tool {
public:
	enum ObjectType {
		BOX,
		BALL,
		mode_count,
	};
	ObjectType type = BOX;
	CompVector<fw::Widget*> create_buttons;
	CreatePanel* create_panel_widget = nullptr;

	CreateTool();
	void reset() override;
	static std::string create_type_name(ObjectType type);
};
