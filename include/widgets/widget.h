#pragma once

#include <SFML/Graphics.hpp>
#include <functional>
#include "utils.h"
#include "logger.h"
#include "compvector.h"
#include "searchindex.h"
#include "drawing.h"
#include "widget_transform.h"
#include "widget_render_queue.h"
#include "widget_unclipped_region.h"

struct WidgetVisibility {
	bool addedToRoot = false;
	bool allParentsVisible = false;
	bool visibleSetting = false;
	bool onScreen = false;
	bool nonZeroSize = false;
	bool hasUnclippedRegion = false;
	bool opaque = false;
	//bool notCovered = false;
};

class WidgetList;

// Adding methods:
// If method changes widget, check this:
// assert(!widget_list.isLocked());
// And if if changes render queue:
// widget_list.render_queue.invalidate();

class Widget {
public:
	enum Anchor {
		CUSTOM,
		TOP_LEFT,
		TOP_CENTER,
		TOP_RIGHT,
		CENTER_LEFT,
		CENTER,
		CENTER_RIGHT,
		BOTTOM_LEFT,
		BOTTOM_CENTER,
		BOTTOM_RIGHT,
	};
	enum Alignment {
		ALIGN_TOP,
		ALIGN_CENTER,
		ALIGN_BOTTOM,
		ALIGN_LEFT,
		ALIGN_RIGHT
	};
	ptrdiff_t debug_id = -1;

	std::function<void(const sf::Vector2f& pos)> OnClick = [](const sf::Vector2f& pos) { };
	std::function<void(const sf::Vector2f& pos)> OnRelease = [](const sf::Vector2f& pos) { };
	std::function<void(const sf::Vector2f& pos)> OnMouseEnter = [](const sf::Vector2f& pos) { };
	std::function<void(const sf::Vector2f& pos)> OnMouseExit = [](const sf::Vector2f& pos) { };
	std::function<void()> OnFocused = []() { };
	std::function<void()> OnFocusLost = []() { };

	Widget(WidgetList& list);
	bool isMouseOver() const;
	void updateMouseState(const sf::Vector2f& mouse_pos);
	virtual bool isVisualPositionQuantized() const;
	bool isVisible() const;
	bool isClickThrough() const;
	WidgetVisibility checkVisibility() const;
	void processClick(const sf::Vector2f& pos);
	void processRelease(const sf::Vector2f& pos);
	void processMouse(const sf::Vector2f& pos);
	virtual bool isFocusable() const;
	bool isFocused() const;
	virtual sf::Cursor::Type getCursorType() const;
	bool getForceCustomCursor() const;
	const std::string& getName() const;
	const std::string& getFullName() const;
	bool getClipChildren() const;
	RenderLayer getRenderLayer() const;
	Widget* getParent() const;
	CompVector<Widget*> getParentChain() const;
	const CompVector<Widget*>& getChildren() const;
	Widget* getChild(size_t index) const;
	Widget* find(const std::string& name) const;
	virtual sf::FloatRect getLocalBounds() const = 0;
	virtual sf::FloatRect getParentLocalBounds() const = 0;
	virtual sf::FloatRect getGlobalBounds() const = 0;
	virtual sf::FloatRect getVisualLocalBounds() const;
	virtual sf::FloatRect getVisualParentLocalBounds() const;
	virtual sf::FloatRect getVisualGlobalBounds() const;
	const sf::FloatRect& getUnclippedRegion() const;
	const sf::FloatRect& getQuantizedUnclippedRegion() const;
	sf::Vector2f toGlobal(const sf::Vector2f& pos) const;
	sf::Vector2f toLocal(const sf::Vector2f& pos) const;
	sf::Vector2f getSize() const;
	float getWidth() const;
	float getHeight() const;
	float getGlobalWidth() const;
	float getGlobalHeight() const;
	sf::Vector2f getAnchorOffset() const;
	const sf::Vector2f& getOrigin() const;
	virtual const sf::Vector2f& getPosition() const;
	virtual const sf::Vector2f& getGlobalPosition() const;
	sf::Vector2f getTopLeft() const;
	sf::Vector2f getTopRight() const;
	sf::Vector2f getBottomLeft() const;
	sf::Vector2f getBottomRight() const;
	sf::Vector2f getGlobalTopLeft() const;
	sf::Vector2f getGlobalTopRight() const;
	sf::Vector2f getGlobalBottomLeft() const;
	sf::Vector2f getGlobalBottomRight() const;
	sf::Vector2f getVisualGlobalTopLeft() const;
	sf::Vector2f getVisualGlobalTopRight() const;
	sf::Vector2f getVisualGlobalBottomLeft() const;
	sf::Vector2f getVisualGlobalBottomRight() const;
	virtual const sf::Color& getFillColor() const = 0;
	void setOrigin(Anchor anchor);
	void setOrigin(float x, float y);
	void setOrigin(const sf::Vector2f& origin);
	void setParentAnchor(Anchor anchor);
	void setAnchorOffset(float x, float y);
	void setAnchorOffset(const sf::Vector2f& offset);
	virtual void setFillColor(const sf::Color& color) = 0;
	virtual void setPosition(float x, float y);
	virtual void setPosition(const sf::Vector2f& position);
	void setRotation(float angle);
	void setVisible(bool value);
	void setClickThrough(bool value);
	void setParentSilent(Widget* new_parent);
	void setParent(Widget* new_parent);
	void setForceCustomCursor(bool value);
	void setName(const std::string& new_name);
	void setClipChildren(bool value);
	void setRenderLayer(RenderLayer layer);
	void removeFocus();
	void processKeyboardEvent(const sf::Event& event);
	void render(sf::RenderTarget& target);
	void renderBounds(sf::RenderTarget& target, const sf::Color& color, bool include_children);
	void renderOrigin(sf::RenderTarget& target);

protected:
	friend class WidgetList;
	friend class WidgetTransform;
	friend class WidgetUnclippedRegion;
	std::string name = "<unnamed>";
	std::string full_name;
	WidgetList& widget_list;
	WidgetTransform transforms = WidgetTransform(this);
	Widget* parent = nullptr;
	CompVector<Widget*> children;
	SearchIndexMultiple<std::string, Widget> children_names;
	RenderLayer layer = RenderLayer::BASE;
	Anchor origin_anchor = CUSTOM;
	Anchor parent_anchor = CUSTOM;
	sf::Vector2f anchor_offset = sf::Vector2f(0.0f, 0.0f);
	WidgetUnclippedRegion unclipped_region = WidgetUnclippedRegion(this);
	bool visible = true;
	bool click_through = true;
	bool clip_children = false;
	bool mouseIn = false;
	bool force_custom_cursor = false;

	sf::Vector2f anchorToPos(Anchor p_anchor, const sf::Vector2f& orig, const sf::Vector2f& size);
	const sf::Transform& getGlobalTransform() const;
	const sf::Transform& getParentGlobalTransform() const;
	const sf::Transform& getInverseGlobalTransform() const;
	const sf::Transform& getInverseParentGlobalTransform() const;
	virtual sf::Drawable& getDrawable() = 0;
	virtual const sf::Drawable& getDrawable() const = 0;
	virtual sf::Transformable& getTransformable() = 0;
	virtual const sf::Transformable& getTransformable() const = 0;
	virtual sf::Vector2f getRenderPositionOffset() const;
	void updateAnchoredPosition();
	virtual void update();
	virtual void internalOnSetParent(Widget* parent);
	virtual void internalOnClick(const sf::Vector2f& pos);
	virtual void internalOnRelease(const sf::Vector2f& pos);
	virtual void internalProcessKeyboardEvent(const sf::Event& event);
	virtual void internalProcessMouse(const sf::Vector2f& pos);
	virtual void internalOnMouseEnter(const sf::Vector2f& pos);
	virtual void internalOnMouseExit(const sf::Vector2f& pos);
	virtual void internalOnFocused();
	virtual void internalOnFocusLost();

private:
	WidgetVisibility visibility;
	sf::RenderTexture render_texture;
	sf::View render_view;

	std::string calcFullName() const;
	void updateFullName();
	void updateVisibility();
	void updateRenderTexture(const sf::FloatRect& texture_bounds);

};
