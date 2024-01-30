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

class Tool {
public:
	std::string name;
	Widget* widget = nullptr;

	Tool();

private:
};

class CreateTool : public Tool {
public:
	enum ObjectType {
		BOX,
		BALL,
	};
	sf::RectangleShape object_type_rect;

	CreateTool();
	CreateTool(Widget* widget);
};

class DragTool : public Tool {
public:
	b2Body* mouse_body = nullptr;
	b2MouseJoint* mouse_joint = nullptr;

	DragTool();
	DragTool(Widget* widget);

private:
};

class MoveTool : public Tool {
public:
	GameObject* object = nullptr;
	bool object_was_enabled = false;
	b2Vec2 offset = b2Vec2();

	MoveTool();
	MoveTool(Widget* widget);

private:
};

class RotateTool : public Tool {
public:
	GameObject* object = nullptr;
	bool object_was_enabled = false;
	float angle_offset = 0.0f;

	RotateTool();
	RotateTool(Widget* widget);

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
	sf::Vector2f select_origin;
	b2Vec2 grabbed_vertex_offset;
	b2Vec2 insertVertexPos;
	sf::RectangleShape vertex_highlight_rect;
	sf::RectangleShape vertex_rect;
	sf::RectangleShape edge_highlight;
	sf::RectangleShape select_rect;

	EditTool();
	EditTool(Widget* widget);
	static std::string modeToStr(EditToolMode mode);

};

extern CreateTool create_tool;
extern DragTool drag_tool;
extern MoveTool move_tool;
extern RotateTool rotate_tool;
extern EditTool edit_tool;
extern std::vector<Tool*> tools;

