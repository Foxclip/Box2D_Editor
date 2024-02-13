#pragma once

#include <SFML/Graphics.hpp>
#include "box2d/box2d.h"
#include "tokenizer.h"
#include "polygon.h"

class LineStripShape : public sf::Drawable, public sf::Transformable {
public:
	explicit LineStripShape();
	explicit LineStripShape(sf::VertexArray& varray);
	void setLineColor(sf::Color color);
	sf::VertexArray varray;
	void drawMask(sf::RenderTarget& mask, sf::RenderStates states = sf::RenderStates::Default);
private:
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
	sf::Color line_color;
};

class CircleNotchShape : public sf::Drawable, public sf::Transformable {
public:
	explicit CircleNotchShape(float radius, size_t point_count, size_t notch_segment_count);
	const sf::Color& getCircleColor() const;
	const sf::Color& getNotchColor() const;
	void setCircleColor(const sf::Color& color);
	void setNotchColor(const sf::Color& color);
	void drawMask(sf::RenderTarget& mask, sf::RenderStates states = sf::RenderStates::Default);
private:
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
	sf::VertexArray varray_circle;
	sf::VertexArray varray_notch;
	sf::Color circle_color;
	sf::Color notch_color;
};

struct BodyDef {
	b2BodyDef body_def;
	std::vector<b2FixtureDef> fixture_defs;
};

class GameObject {
public:
	b2Body* rigid_body = nullptr;
	std::vector<std::unique_ptr<GameObject>> children;
	GameObject* parent = nullptr;
	sf::Color color;
	bool hover = false;
	bool selected = false;

	GameObject();
	~GameObject();
	virtual sf::Drawable* getDrawable() = 0;
	virtual sf::Transformable* getTransformable() = 0;
	void updateVisual();
	void render(sf::RenderTarget& target);
	void renderMask(sf::RenderTarget& mask, bool include_children);
	void setVisualPosition(const sf::Vector2f& pos);
	void setVisualRotation(float angle);
	void setEnabled(bool enabled, bool include_children);
	void setPosition(const b2Vec2& pos, bool move_children);
	void setAngle(float angle, bool rotate_children);
	void setLinearVelocity(const b2Vec2& velocity, bool include_children);
	void setAngularVelocity(float velocity, bool include_children);
	void setType(b2BodyType type, bool include_children);
	void setDensity(float density, bool include_children);
	void setFriction(float friction, bool include_children);
	void setRestitution(float restitution, bool include_children);
	virtual TokenWriter& serialize(TokenWriter& tw) = 0;
	static TokenWriter& serializeBody(TokenWriter& tw, b2Body* body);
	static TokenWriter& serializeFixture(TokenWriter& tw, b2Fixture* fixture);
	static TokenWriter& serializeJoint(TokenWriter& tw, b2Joint* p_joint);
	static TokenWriter& serializeRevoluteJoint(TokenWriter& p_tw, b2RevoluteJoint* joint);
	static BodyDef deserializeBody(TokenReader& tr);
	static b2FixtureDef deserializeFixture(TokenReader& tr);
	static b2RevoluteJointDef deserializeRevoluteJoint(TokenReader& tr);

protected:
	virtual void drawMask(sf::RenderTarget& mask) = 0;

};

class BoxObject : public GameObject {
public:
	BoxObject(b2World* world, b2BodyDef def, b2Vec2 size, sf::Color color);
	sf::Drawable* getDrawable() override;
	sf::Transformable* getTransformable() override;
	void drawMask(sf::RenderTarget& mask) override;
	TokenWriter& serialize(TokenWriter& tw) override;
	static std::unique_ptr<BoxObject> deserialize(TokenReader& tr, b2World* world);
	b2Vec2 size = b2Vec2();
private:
	std::unique_ptr<sf::RectangleShape> rect_shape;
};

class BallObject : public GameObject {
public:
	BallObject(b2World* world, b2BodyDef def, float radius, sf::Color color, sf::Color notch_color = sf::Color::Transparent);
	sf::Drawable* getDrawable() override;
	sf::Transformable* getTransformable() override;
	void drawMask(sf::RenderTarget& mask) override;
	TokenWriter& serialize(TokenWriter& tw) override;
	static std::unique_ptr<BallObject> deserialize(TokenReader& tr, b2World* world);
	float radius = 0.0f;
private:
	std::unique_ptr<CircleNotchShape> circle_notch_shape;
	sf::Color notch_color;
};

class CarObject : public GameObject {
public:
	CarObject(b2World* world, b2BodyDef def, std::vector<float> lengths, std::vector<float> wheels, sf::Color color);
	CarObject(
		b2World* world,
		b2BodyDef def,
		std::vector<float> lengths,
		std::vector<std::unique_ptr<BallObject>> wheels,
		std::vector<b2RevoluteJointDef> joint_defs,
		sf::Color color
	);
	PolygonObject* getPolygonObject() const;
	sf::Drawable* getDrawable() override;
	sf::Transformable* getTransformable() override;
	void drawMask(sf::RenderTarget& mask) override;
	TokenWriter& serialize(TokenWriter& tw) override;
	static std::unique_ptr<CarObject> deserialize(TokenReader& tr, b2World* world);
	std::vector<float> lengths;
private:
	std::unique_ptr<PolygonObject> polygon;
	std::vector<b2RevoluteJoint*> wheel_joints;
	void create_shape(std::vector<float> lengths);
	void create_wheel(b2Vec2 wheel_pos, float radius);
};

class GroundVertex {
public:
	b2Vec2 pos = b2Vec2(0.0f, 0.0f);
	b2Vec2 orig_pos = b2Vec2(0.0f, 0.0f);
	bool selected = false;

	GroundVertex(b2Vec2 pos);

private:
};

class GroundObject : public GameObject {
public:
	GroundObject(b2World* world, b2BodyDef def, std::vector<b2Vec2> p_vertices, sf::Color color);
	void moveVertices(const std::vector<size_t>& index_list, const b2Vec2& offset);
	void offsetVertex(size_t index, const b2Vec2& offset, bool sync = true);
	void offsetSelected(const b2Vec2& offset, bool sync = true);
	void saveOffsets();
	size_t getVertexCount();
	const GroundVertex& getVertex(size_t index);
	b2Vec2 getVertexPos(size_t index);
	void setVertexPos(size_t index, const b2Vec2& new_pos);
	bool tryDeleteVertex(ptrdiff_t index);
	void addVertex(size_t index, const b2Vec2& pos);
	void selectVertex(size_t index);
	bool isVertexSelected(size_t index);
	void selectAllVertices();
	void deselectAllVertices();
	void syncVertices();
	sf::Drawable* getDrawable() override;
	sf::Transformable* getTransformable() override;
	void drawMask(sf::RenderTarget& mask) override;
	TokenWriter& serialize(TokenWriter& tw) override;
	static std::unique_ptr<GroundObject> deserialize(TokenReader& tr, b2World* world);
private:
	std::vector<GroundVertex> vertices;
	std::unique_ptr<LineStripShape> line_strip_shape;
	b2ChainShape* getShape();
	std::vector<b2Vec2> getPositions();
	void vertexSet(size_t index, const b2Vec2& new_pos);
};
