#pragma once

#include <SFML/Graphics.hpp>
#include <functional>
#include "compvector.h"
#include "searchindex.h"
#include "widgets_common.h"
#include "drawing.h"
#include "widget_transform.h"
#include "widget_render_queue.h"
#include "widget_unclipped_region.h"

namespace fw {

	struct WidgetVisibility {
		bool addedToRoot = false;
		bool allParentsVisible = false;
		bool renderableSetting = false;
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
	// wAssert(!widget_list.isLocked());
	// And if if changes render queue:
	// widget_list.render_queue.invalidate();

	class Widget {
	public:
		enum class Anchor {
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
		enum class Alignment {
			ALIGN_TOP,
			ALIGN_CENTER,
			ALIGN_BOTTOM,
			ALIGN_LEFT,
			ALIGN_RIGHT
		};
		ptrdiff_t debug_id = -1;

		std::function<void(const sf::Vector2f& pos)> OnLeftPress = [](const sf::Vector2f& pos) { };
		std::function<void(const sf::Vector2f& pos)> OnRightPress = [](const sf::Vector2f& pos) { };
		std::function<void(const sf::Vector2f& pos)> OnLeftRelease = [](const sf::Vector2f& pos) { };
		std::function<void(const sf::Vector2f& pos)> OnRightRelease = [](const sf::Vector2f& pos) { };
		std::function<void(const sf::Vector2f& pos)> OnMouseEnter = [](const sf::Vector2f& pos) { };
		std::function<void(const sf::Vector2f& pos)> OnMouseExit = [](const sf::Vector2f& pos) { };
		std::function<void(const sf::Vector2f& pos)> OnProcessMouse = [](const sf::Vector2f& pos) { };
		std::function<void()> OnFocused = []() { };
		std::function<void()> OnFocusLost = []() { };
		std::function<void()> OnUpdate = []() { };
		std::function<void(sf::RenderTarget& target)> OnBeforeGlobalRender = [](sf::RenderTarget& target) { };
		std::function<void(sf::RenderTarget& target)> OnBeforeRender = [](sf::RenderTarget& target) { };
		std::function<void(sf::RenderTarget& target)> OnAfterRender = [](sf::RenderTarget& target) { };
		std::function<void(sf::RenderTarget& target)> OnAfterGlobalRender = [](sf::RenderTarget& target) { };
		std::function<void(unsigned int, unsigned int)> OnWindowResized = [](unsigned int width, unsigned int height) { };
		std::function<sf::Cursor::Type()> GetCursorType = []() { return sf::Cursor::Arrow; };

		Widget(WidgetList& list);
		bool isMouseOver() const;
		void updateMouseState(const sf::Vector2f& mouse_pos);
		virtual bool isVisualPositionQuantized() const;
		bool isRenderable() const;
		bool isVisible() const;
		bool isClickThrough() const;
		bool getChildrenLocked() const;
		WidgetVisibility checkVisibility() const;
		void processLeftPress(const sf::Vector2f& pos);
		void processRightPress(const sf::Vector2f& pos);
		void processLeftRelease(const sf::Vector2f& pos);
		void processRightRelease(const sf::Vector2f& pos);
		void processMouse(const sf::Vector2f& pos);
		bool isFocusable() const;
		bool isFocused() const;
		bool getForceCustomCursor() const;
		const std::string& getName() const;
		const std::string& getFullName() const;
		bool getClipChildren() const;
		GlobalRenderLayer getGlobalRenderLayer() const;
		size_t getLocalRenderLayer() const;
		size_t getParentLocalRenderLayer() const;
		sf::Shader* getShader() const;
		Widget* getParent() const;
		CompVector<Widget*> getParentChain() const;
		const CompVector<Widget*>& getChildren() const;
		CompVector<Widget*> getAllChildren() const;
		Widget* getChild(size_t index) const;
		Widget* find(const std::string& name) const;
		CompVector<Widget*> getRenderQueue() const;
		virtual sf::FloatRect getLocalBounds() const = 0;
		sf::FloatRect getParentLocalBounds() const;
		sf::FloatRect getGlobalBounds() const;
		virtual sf::FloatRect getVisualLocalBounds() const;
		sf::FloatRect getVisualParentLocalBounds() const;
		sf::FloatRect getVisualGlobalBounds() const;
		const sf::FloatRect& getUnclippedRegion() const;
		const sf::FloatRect& getQuantizedUnclippedRegion() const;
		sf::Vector2f toGlobal(const sf::Vector2f& pos) const;
		sf::Vector2f toLocal(const sf::Vector2f& pos) const;
		sf::Vector2f getSize() const;
		float getWidth() const;
		float getHeight() const;
		float getGlobalWidth() const;
		float getGlobalHeight() const;
		Anchor getParentAnchor() const;
		sf::Vector2f getAnchorOffset() const;
		const sf::Vector2f& getOrigin() const;
		const sf::Vector2f& getPosition() const;
		sf::Vector2f getGlobalPosition() const;
		sf::Vector2f getGlobalOriginPosition() const;
		sf::Vector2f getCenter() const;
		sf::Vector2f getGlobalCenter() const;
		sf::Vector2f getVisualGlobalCenter() const;
		sf::Vector2f getTop() const;
		sf::Vector2f getLeft() const;
		sf::Vector2f getRight() const;
		sf::Vector2f getBottom() const;
		sf::Vector2f getGlobalTop() const;
		sf::Vector2f getGlobalLeft() const;
		sf::Vector2f getGlobalRight() const;
		sf::Vector2f getGlobalBottom() const;
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
		void setPosition(float x, float y);
		void setPosition(const sf::Vector2f& position);
		void setGlobalPosition(float x, float y);
		void setGlobalPosition(const sf::Vector2f& position);
		void setRotation(float angle);
		virtual void setRenderable(bool value);
		void setVisible(bool value);
		void toggleVisible();
		void setClickThrough(bool value);
		void setFocusable(bool value);
		void setParentSilent(Widget* new_parent);
		void setParent(Widget* new_parent);
		void lockChildren();
		void unlockChildren();
		void setForceCustomCursor(bool value);
		void setName(const std::string& new_name);
		void setClipChildren(bool value);
		void setRenderLayer(GlobalRenderLayer layer);
		void setLocalRenderLayer(size_t layer);
		void setParentLocalRenderLayer(size_t layer);
		void setShader(sf::Shader* shader);
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
		bool children_locked = false;
		SearchIndexMultiple<std::string, Widget*> children_names;
		sf::Shader* shader = nullptr;
		GlobalRenderLayer global_layer = GlobalRenderLayer::BASE;
		std::map<Widget*, size_t> local_layers;
		Anchor origin_anchor = Anchor::CUSTOM;
		Anchor parent_anchor = Anchor::CUSTOM;
		sf::Vector2f anchor_offset = sf::Vector2f(0.0f, 0.0f);
		WidgetUnclippedRegion unclipped_region = WidgetUnclippedRegion(this);
		bool visible = true;
		bool renderable = true;
		bool is_focusable = false;
		bool click_through = true;
		bool clip_children = false;
		bool mouseIn = false;
		bool force_custom_cursor = false;

		sf::Vector2f anchorToPos(Anchor p_anchor, const sf::Vector2f& size);
		const sf::Transform& getTransform() const;
		const sf::Transform& getInverseTransform() const;
		const sf::Transform& getGlobalTransform() const;
		const sf::Transform& getParentGlobalTransform() const;
		const sf::Transform& getInverseGlobalTransform() const;
		const sf::Transform& getInverseParentGlobalTransform() const;
		virtual sf::Drawable* getDrawable() = 0;
		virtual const sf::Drawable* getDrawable() const = 0;
		virtual sf::Transformable* getTransformable() = 0;
		virtual const sf::Transformable* getTransformable() const = 0;
		virtual sf::Vector2f getRenderPositionOffset() const;
		virtual void addChild(Widget* child);
		virtual void removeChild(Widget* child);
		void updateAnchoredPosition();
		void update();
		virtual void internalUpdate();
		virtual void internalOnSetParent(Widget* parent);
		virtual void internalOnLeftPress(const sf::Vector2f& pos);
		virtual void internalOnRightPress(const sf::Vector2f& pos);
		virtual void internalOnLeftRelease(const sf::Vector2f& pos);
		virtual void internalOnRightRelease(const sf::Vector2f& pos);
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

}
