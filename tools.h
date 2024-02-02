#pragma once

#include "gameobject.h"
#include "widget.h"

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

	Tool();
	virtual void reset() = 0;

private:
};

class SelectTool : public Tool {
public:
	GameObject* hover_object = nullptr;
	GameObject* selected_object = nullptr;

	SelectTool();
	void reset();

private:
};

class DragTool : public Tool {
public:
	b2Body* mouse_body = nullptr;
	b2MouseJoint* mouse_joint = nullptr;

	DragTool();
	void reset();

private:
};

class MoveTool : public Tool {
public:
	GameObject* object = nullptr;
	bool object_was_enabled = false;
	b2Vec2 offset = b2Vec2(0.0f, 0.0f);

	MoveTool();
	void reset();

private:
};

class RotateTool : public Tool {
public:
	GameObject* object = nullptr;
	bool object_was_enabled = false;
	float angle_offset = 0.0f;

	RotateTool();
	void reset();

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
	int grabbed_vertex = -1;
	int highlighted_vertex = -1;
	int highlighted_edge = -1;
	int edge_vertex = -1;
	bool selection = false;
	sf::Vector2f select_origin = sf::Vector2f(0.0f, 0.0f);
	b2Vec2 grabbed_vertex_offset = b2Vec2(0.0f, 0.0f);
	b2Vec2 insertVertexPos = b2Vec2(0.0f, 0.0f);
	sf::RectangleShape vertex_highlight_rect;
	sf::RectangleShape vertex_rect;
	sf::RectangleShape edge_highlight;
	sf::RectangleShape select_rect;
	ContainerWidget* edit_window_widget = nullptr;

	EditTool();
	void reset();
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
	std::vector<Widget*> create_buttons;
	ContainerWidget* create_panel_widget = nullptr;

	CreateTool();
	void reset();
	static std::string create_type_name(ObjectType type);
};

extern SelectTool select_tool;
extern DragTool drag_tool;
extern MoveTool move_tool;
extern RotateTool rotate_tool;
extern EditTool edit_tool;
extern CreateTool create_tool;
extern std::vector<Tool*> tools;

