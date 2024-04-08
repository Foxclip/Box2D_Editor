#pragma once

#include <set>
#include <SFML/Graphics.hpp>
#include <box2d/box2d.h>
#include "compvector.h"
#include "serializer.h"
#include "polygon.h"
#include "utils.h"
#include "shapes.h"
#include "joint.h"
#include "gameobject_transform.h"

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
	bool operator==(const EditableVertex& other) const;

private:
};

class GameObjectList;

class GameObject {
public:
	ptrdiff_t id = -1;
	ptrdiff_t parent_id = -1;
	b2Body* rigid_body = nullptr;
	sf::Color color;
	bool hover = false;
	bool selected = false;
	bool draw_varray = false;
	bool draw_indices = false;
	b2Vec2 cursor_offset = b2Vec2_zero;
	b2Vec2 orig_pos = b2Vec2_zero;
	float orig_angle = 0.0f;
	bool was_enabled = true;
	CompVector<Joint*> joints;

	GameObject();
	~GameObject();
	virtual bool isClosed() const = 0;
	ptrdiff_t getId() const;
	const std::string& getName() const;
	virtual sf::Drawable* getDrawable() const = 0;
	virtual sf::Transformable* getTransformable() const = 0;
	b2BodyType getType() const;
	const b2Vec2& getPosition() const;
	const b2Vec2& getLinearVelocity() const;
	float getAngularVelocity() const;
	float getRotation() const;
	const b2Vec2& getGlobalPosition() const;
	float getGlobalRotation() const;
	const b2Transform& getTransform() const;
	const b2Transform& getGlobalTransform() const;
	b2Transform getParentGlobalTransform() const;
	GameObject* getParent() const;
	CompVector<GameObject*> getParentChain() const;
	const CompVector<GameObject*>& getChildren() const;
	CompVector<GameObject*> getAllChildren() const;
	GameObject* getChild(size_t index) const;
	b2Vec2 toGlobal(const b2Vec2& pos);
	b2Vec2 toLocal(const b2Vec2& pos);
	float toGlobalAngle(float angle);
	float toLocalAngle(float angle);
	b2Vec2 toParentLocal(const b2Vec2& pos);
	float toParentLocalAngle(float angle);
	ptrdiff_t getChildIndex(const GameObject* object) const;
	void updateVisual();
	virtual void render(sf::RenderTarget& target);
	void renderMask(sf::RenderTarget& mask);
	void setParent(GameObject* new_parent);
	void setName(const std::string& new_name);
	void setEnabled(bool enabled, bool include_children);
	void setGlobalTransform(const b2Transform& transform);
	void setGlobalPosition(const b2Vec2& pos);
	void setGlobalAngle(float angle);
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
	const std::vector<EditableVertex>& getVertices() const;
	b2Vec2 getGlobalVertexPos(size_t index);
	void setGlobalVertexPos(size_t index, const b2Vec2& new_pos);
	bool tryDeleteVertex(ptrdiff_t index);
	void addVertexGlobal(size_t index, const b2Vec2& pos);
	void selectVertex(size_t index);
	bool isVertexSelected(size_t index) const;
	void selectAllVertices();
	void deselectAllVertices();
	virtual void syncVertices(bool save_velocities = false);
	void transformFromRigidbody();
	void transformToRigidbody();
	std::string serialize() const;
	virtual TokenWriter& serialize(TokenWriter& tw) const = 0;
	static TokenWriter& serializeBody(TokenWriter& tw, b2Body* body);
	static TokenWriter& serializeFixture(TokenWriter& tw, b2Fixture* fixture);
	static BodyDef deserializeBody(TokenReader& tr);
	static b2FixtureDef deserializeFixture(TokenReader& tr);
	static GameObject* getGameobject(b2Body* body);
	virtual bool operator==(const GameObject& other) const;

protected:
	GameObjectList* object_list = nullptr;
	std::string name = "<unnamed>";
	std::vector<EditableVertex> vertices;
	GameObject* parent = nullptr;

	virtual void drawMask(sf::RenderTarget& mask) = 0;
	std::vector<b2Vec2> getPositions() const;
	void setVisualPosition(const sf::Vector2f& pos);
	void setVisualRotation(float angle);
	void vertexSet(size_t index, const b2Vec2& new_pos);
	void destroyFixtures();
	virtual void internalSyncVertices() = 0;

private:
	friend class GameObjectList;
	friend class GameObjectTransforms;
	ptrdiff_t new_id = -1;
	CompVector<GameObject*> children;
	GameObjectTransforms transforms = GameObjectTransforms(this);

};

class BoxObject : public GameObject {
public:
	b2Vec2 size = b2Vec2();

	BoxObject(GameObjectList* object_list, b2BodyDef def, b2Vec2 size, sf::Color color);
	bool isClosed() const override;
	sf::Drawable* getDrawable() const override;
	sf::Transformable* getTransformable() const override;
	void drawMask(sf::RenderTarget& mask) override;
	using GameObject::serialize;
	TokenWriter& serialize(TokenWriter& tw) const override;
	static std::unique_ptr<BoxObject> deserialize(const std::string& str, GameObjectList* object_list);
	static std::unique_ptr<BoxObject> deserialize(TokenReader& tr, GameObjectList* object_list);
	void internalSyncVertices() override;
	bool operator==(const BoxObject& other) const;

private:
	std::unique_ptr<sf::RectangleShape> rect_shape;
};

class BallObject : public GameObject {
public:
	float radius = 0.0f;

	BallObject(
		GameObjectList* object_list,
		b2BodyDef def,
		float radius,
		sf::Color color,
		sf::Color notch_color = sf::Color::Transparent
	);
	bool isClosed() const override;
	sf::Drawable* getDrawable() const override;
	sf::Transformable* getTransformable() const override;
	void drawMask(sf::RenderTarget& mask) override;
	using GameObject::serialize;
	TokenWriter& serialize(TokenWriter& tw) const override;
	static std::unique_ptr<BallObject> deserialize(const std::string& str, GameObjectList* object_list);
	static std::unique_ptr<BallObject> deserialize(TokenReader& tr, GameObjectList* object_list);
	void internalSyncVertices() override;
	bool operator==(const BallObject& other) const;

private:
	std::unique_ptr<CircleNotchShape> circle_notch_shape;
	sf::Color notch_color;

};

class PolygonObject : public GameObject {
public:
	PolygonObject(
		GameObjectList* object_list,
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
	using GameObject::serialize;
	TokenWriter& serialize(TokenWriter& tw) const override;
	static std::unique_ptr<PolygonObject> deserialize(const std::string& str, GameObjectList* object_list);
	static std::unique_ptr<PolygonObject> deserialize(TokenReader& tr, GameObjectList* object_list);
	void internalSyncVertices() override;
	bool operator==(const PolygonObject& other) const;

private:
	std::unique_ptr<SplittablePolygon> polygon;
};

class ChainObject : public GameObject {
public:
	ChainObject(GameObjectList* object_list, b2BodyDef def, std::vector<b2Vec2> p_vertices, sf::Color color);
	bool isClosed() const override;
	sf::Drawable* getDrawable() const override;
	sf::Transformable* getTransformable() const override;
	void drawMask(sf::RenderTarget& mask) override;
	using GameObject::serialize;
	TokenWriter& serialize(TokenWriter& tw) const override;
	static std::unique_ptr<ChainObject> deserialize(const std::string& str, GameObjectList* object_list);
	static std::unique_ptr<ChainObject> deserialize(TokenReader& tr, GameObjectList* object_list);
	void internalSyncVertices() override;
	bool operator==(const ChainObject& other) const;

private:
	std::unique_ptr<LineStripShape> line_strip_shape;

	b2ChainShape* getShape() const;

};
