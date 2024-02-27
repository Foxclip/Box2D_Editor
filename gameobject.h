#pragma once

#include <set>
#include <SFML/Graphics.hpp>
#include "box2d/box2d.h"
#include "serializer.h"
#include "polygon.h"
#include "compvector.h"

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

class EditableVertex {
public:
	b2Vec2 pos = b2Vec2(0.0f, 0.0f);
	b2Vec2 orig_pos = b2Vec2(0.0f, 0.0f);
	bool selected = false;

	EditableVertex(b2Vec2 pos);

private:
};

class GameObject;

class Joint {
public:
	GameObject* object1 = nullptr;
	GameObject* object2 = nullptr;

	virtual TokenWriter& serialize(TokenWriter& tw) const = 0;
	~Joint();

protected:
	b2Joint* joint;
};

class RevoluteJoint : public Joint {
public:
	RevoluteJoint(b2RevoluteJointDef& def, b2World* world, GameObject* object1, GameObject* object2);
	RevoluteJoint(b2RevoluteJoint* joint);
	TokenWriter& serialize(TokenWriter& tw) const override;
	static b2RevoluteJointDef deserialize(TokenReader& tr, ptrdiff_t& p_body_a, ptrdiff_t& p_body_b);

private:
};

class GameObject {
public:
	ptrdiff_t id = -1;
	ptrdiff_t parent_id = -1;
	ptrdiff_t new_id = -1;
	b2Body* rigid_body = nullptr;
	sf::Color color;
	bool hover = false;
	bool selected = false;
	bool draw_varray = false;
	b2Vec2 cursor_offset = b2Vec2(0.0f, 0.0f);
	b2Vec2 orig_pos = b2Vec2(0.0f, 0.0f);
	float orig_angle = 0.0f;
	bool was_enabled = true;
	CompoundVector<Joint*> joints;

	GameObject();
	~GameObject();
	virtual bool isClosed() const = 0;
	virtual sf::Drawable* getDrawable() const = 0;
	virtual sf::Transformable* getTransformable() const = 0;
	const b2Vec2& getPosition() const;
	float getRotation() const;
	GameObject* getParent() const;
	std::vector<GameObject*> getParentChain() const;
	const std::vector<GameObject*>& getChildren() const;
	GameObject* getChild(size_t index) const;
	b2Vec2 toGlobal(const b2Vec2& pos) const;
	b2Vec2 toLocal(const b2Vec2& pos) const;
	ptrdiff_t getChildIndex(const GameObject* object) const;
	void updateVisual();
	virtual void render(sf::RenderTarget& target);
	void renderMask(sf::RenderTarget& mask);
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
	void moveVertices(const std::vector<size_t>& index_list, const b2Vec2& offset);
	void offsetVertex(size_t index, const b2Vec2& offset, bool sync = true);
	void offsetSelected(const b2Vec2& offset, bool sync = true);
	void saveOffsets();
	size_t indexLoop(ptrdiff_t index) const;
	size_t getVertexCount() const;
	size_t getEdgeCount() const;
	const EditableVertex& getVertex(size_t index) const;
	b2Vec2 getGlobalVertexPos(size_t index) const;
	void setGlobalVertexPos(size_t index, const b2Vec2& new_pos);
	bool tryDeleteVertex(ptrdiff_t index);
	void addVertexGlobal(size_t index, const b2Vec2& pos);
	void selectVertex(size_t index);
	bool isVertexSelected(size_t index) const;
	void selectAllVertices();
	void deselectAllVertices();
	virtual void syncVertices() = 0;
	virtual TokenWriter& serialize(TokenWriter& tw) const = 0;
	static TokenWriter& serializeBody(TokenWriter& tw, b2Body* body);
	static TokenWriter& serializeFixture(TokenWriter& tw, b2Fixture* fixture);
	static BodyDef deserializeBody(TokenReader& tr);
	static b2FixtureDef deserializeFixture(TokenReader& tr);
	static GameObject* getGameobject(b2Body* body);

protected:
	std::vector<EditableVertex> vertices;
	GameObject* parent = nullptr;

	virtual void drawMask(sf::RenderTarget& mask) = 0;
	std::vector<b2Vec2> getPositions();
	void vertexSet(size_t index, const b2Vec2& new_pos);
	void destroyFixtures();

private:
	friend class GameObjectList;
	CompoundVector<GameObject*> children; 
	void setParent(GameObject* parent);

};

class BoxObject : public GameObject {
public:
	b2Vec2 size = b2Vec2();

	BoxObject(b2World* world, b2BodyDef def, b2Vec2 size, sf::Color color);
	bool isClosed() const override;
	sf::Drawable* getDrawable() const override;
	sf::Transformable* getTransformable() const override;
	void drawMask(sf::RenderTarget& mask) override;
	TokenWriter& serialize(TokenWriter& tw) const override;
	static std::unique_ptr<BoxObject> deserialize(TokenReader& tr, b2World* world);
	void syncVertices() override;

private:
	std::unique_ptr<sf::RectangleShape> rect_shape;
};

class BallObject : public GameObject {
public:
	float radius = 0.0f;

	BallObject(b2World* world, b2BodyDef def, float radius, sf::Color color, sf::Color notch_color = sf::Color::Transparent);
	bool isClosed() const override;
	sf::Drawable* getDrawable() const override;
	sf::Transformable* getTransformable() const override;
	void drawMask(sf::RenderTarget& mask) override;
	TokenWriter& serialize(TokenWriter& tw) const override;
	static std::unique_ptr<BallObject> deserialize(TokenReader& tr, b2World* world);
	void syncVertices() override;

private:
	std::unique_ptr<CircleNotchShape> circle_notch_shape;
	sf::Color notch_color;

};

class PolygonObject : public GameObject {
public:
	PolygonObject(
		b2World* world,
		b2BodyDef def,
		const std::vector<b2Vec2>& vertices,
		const sf::Color& color
	);
	bool isClosed() const override;
	SplittablePolygon* getSplittablePolygon() const;
	sf::Drawable* getDrawable() const override;
	sf::Transformable* getTransformable() const override;
	void render(sf::RenderTarget& target) override;
	void drawMask(sf::RenderTarget& mask) override;
	TokenWriter& serialize(TokenWriter& tw) const override;
	static std::unique_ptr<PolygonObject> deserialize(TokenReader& tr, b2World* world);
	void syncVertices() override;

private:
	std::unique_ptr<SplittablePolygon> polygon;
};

class ChainObject : public GameObject {
public:
	ChainObject(b2World* world, b2BodyDef def, std::vector<b2Vec2> p_vertices, sf::Color color);
	bool isClosed() const override;
	sf::Drawable* getDrawable() const override;
	sf::Transformable* getTransformable() const override;
	void drawMask(sf::RenderTarget& mask) override;
	TokenWriter& serialize(TokenWriter& tw) const override;
	static std::unique_ptr<ChainObject> deserialize(TokenReader& tr, b2World* world);
	void syncVertices() override;

private:
	std::unique_ptr<LineStripShape> line_strip_shape;

	b2ChainShape* getShape() const;

};
