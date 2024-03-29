#pragma once

#include "gameobject.h"
#include "widgets/widget.h"
#include "UI/edit_window.h"
#include <set>

const int TOOL_RECT_WIDTH = 60;
const int TOOL_RECT_HEIGHT = 40;
const int TOOLBOX_PADDING = 10;
const int TOOL_TEXT_SIZE = 16;
const int CREATE_RECT_WIDTH = 40;
const int CREATE_RECT_HEIGHT = 40;
const int CREATE_PANEL_PADDING = 10;
const b2Vec2 NEW_BOX_SIZE = b2Vec2(1.0f, 1.0f);
const sf::Color NEW_BOX_COLOR = sf::Color(0, 255, 0);
const float NEW_BALL_RADIUS = 0.5f;
const sf::Color NEW_BALL_COLOR = sf::Color(0, 255, 0);
const sf::Color NEW_BALL_NOTCH_COLOR = sf::Color(0, 64, 0);

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

class Tool {
public:
	std::string name;
	Widget* widget = nullptr;
	std::function<void(bool)> OnSetSelected = [](bool value) { };

	virtual bool showInToolPanel() const = 0;

	Tool();
	virtual void reset() = 0;

private:
};

class SelectTool : public Tool {
public:
	GameObject* hover_object = nullptr;
	RectangleSelect rectangle_select;
	CompoundVector<GameObject*> objects_in_rect;

	SelectTool();
	bool showInToolPanel() const override;
	size_t selectedCount() const;
	const CompoundVector<GameObject*>& getSelectedObjects() const;
	void setSelected(const CompoundVector<GameObject*> vec);
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
	CompoundVector<GameObject*> selected_objects;

};

class DragTool : public Tool {
public:
	b2Body* mouse_body = nullptr;
	b2MouseJoint* mouse_joint = nullptr;

	DragTool();
	bool showInToolPanel() const override;
	void reset() override;

private:
};

class MoveTool : public Tool {
public:
	b2Vec2 orig_cursor_pos = b2Vec2(0.0f, 0.0f);
	CompoundVector<GameObject*> moving_objects;

	MoveTool();
	bool showInToolPanel() const override;
	void reset() override;

private:
};

class RotateTool : public Tool {
public:
	b2Vec2 pivot_pos = b2Vec2(0.0f, 0.0f);
	b2Vec2 orig_cursor_pos = b2Vec2(0.0f, 0.0f);
	CompoundVector<GameObject*> rotating_objects;
	float orig_mouse_angle = 0.0f;

	RotateTool();
	bool showInToolPanel() const override;
	void reset() override;

private:
};

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

	EditTool();
	bool showInToolPanel() const override;
	void reset() override;
	static std::string modeToStr(EditToolMode mode);

};

class CreateTool : public Tool {
public:
	enum ObjectType {
		BOX,
		BALL,
		mode_count,
	};
	ObjectType type = BOX;
	CompoundVector<Widget*> create_buttons;
	ContainerWidget* create_panel_widget = nullptr;

	CreateTool();
	bool showInToolPanel() const override;
	void reset() override;
	static std::string create_type_name(ObjectType type);
};
