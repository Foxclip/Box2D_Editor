#pragma once

#include <SFML/Graphics.hpp>
#include <functional>
#include "utils.h"
#include "logger.h"
#include "global.h"
#include "compvector.h"
#include "searchindex.h"

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

class Widget;
class WidgetList;

class WidgetTransform {
public:
	WidgetTransform(Widget* widget);
	const sf::Transform& getTransform() const;
	const sf::Transform& getGlobalTransform() const;
	const sf::Transform& getInverseGlobalTransform() const;
	const sf::Vector2f& getPosition() const;
	float getRotation() const;
	const sf::Vector2f& getScale() const;
	void invalidateGlobalTransform();
	void setPosition(const sf::Vector2f& position);
	void setRotation(float angle);
	void setScale(const sf::Vector2f& scale);

private:
	Widget* widget = nullptr;
	mutable sf::Transform global_transform;
	mutable sf::Transform inv_global_transform;
	mutable bool global_transform_valid = false;
	mutable bool inv_global_transform_valid = false;

	void recalcGlobalTransform() const;
	void recalcInverseGlobalTransform() const;

};

class WidgetUnclippedRegion {
public:
	WidgetUnclippedRegion(Widget* widget);
	const sf::FloatRect& get() const;
	const sf::FloatRect& getQuantized() const;
	bool isNonZero() const;
	bool isQuantizedNonZero() const;
	void recalc() const;
	void invalidate();

private:
	Widget* widget = nullptr;
	mutable sf::FloatRect unclippedRegion;
	mutable sf::FloatRect quantizedUnclippedRegion;
	mutable bool valid = false;

};

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
	void updateMouseState();
	virtual bool isVisualPositionQuantized() const;
	WidgetVisibility checkVisibility() const;
	void processClick(const sf::Vector2f& pos);
	void processRelease(const sf::Vector2f& pos);
	virtual bool isFocusable() const;
	bool isFocused() const;
	const std::string& getName() const;
	const std::string& getFullName() const;
	bool getClipChildren() const;
	Widget* getParent() const;
	CompoundVector<Widget*> getParentChain() const;
	const CompoundVector<Widget*>& getChildren() const;
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
	const sf::Vector2f getTopLeft() const;
	const sf::Vector2f getTopRight() const;
	const sf::Vector2f getBottomLeft() const;
	const sf::Vector2f getBottomRight() const;
	const sf::Vector2f getGlobalTopLeft() const;
	const sf::Vector2f getGlobalTopRight() const;
	const sf::Vector2f getGlobalBottomLeft() const;
	const sf::Vector2f getGlobalBottomRight() const;
	const sf::Vector2f getVisualGlobalTopLeft() const;
	const sf::Vector2f getVisualGlobalTopRight() const;
	const sf::Vector2f getVisualGlobalBottomLeft() const;
	const sf::Vector2f getVisualGlobalBottomRight() const;
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
	void setName(const std::string& new_name);
	void setClipChildren(bool value);
	void removeFocus();
	virtual void processKeyboardEvent(const sf::Event& event);
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
	CompoundVector<Widget*> children;
	SearchIndexMultiple<std::string, Widget> children_names;
	Anchor origin_anchor = CUSTOM;
	Anchor parent_anchor = CUSTOM;
	sf::Vector2f anchor_offset = sf::Vector2f(0.0f, 0.0f);
	WidgetUnclippedRegion unclipped_region = WidgetUnclippedRegion(this);
	bool visible = true;
	bool click_through = true;
	bool clip_children = false;
	bool mouseIn = false;

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
	virtual void update();
	virtual void internalOnSetParent(Widget* parent);
	virtual void internalOnClick(const sf::Vector2f& pos);
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

class ShapeWidget : public Widget {
public:
	ShapeWidget(WidgetList& widget_list);
	sf::FloatRect getLocalBounds() const override;
	sf::FloatRect getParentLocalBounds() const override;
	sf::FloatRect getGlobalBounds() const override;
	virtual const sf::Color& getFillColor() const override;
	virtual void setFillColor(const sf::Color& color) override;
	void setOutlineColor(const sf::Color& color);
	void setOutlineThickness(float thickness);

protected:
	virtual sf::Shape& getShape() = 0;
	virtual const sf::Shape& getShape() const = 0;
};

class RectangleWidget : public ShapeWidget {
public:
	RectangleWidget(WidgetList& widget_list);
	void setSize(float width, float height);
	void setSize(const sf::Vector2f& size);

protected:
	sf::RectangleShape rect;

	sf::Drawable& getDrawable() override;
	const sf::Drawable& getDrawable() const override;
	sf::Transformable& getTransformable() override;
	const sf::Transformable& getTransformable() const override;
	sf::Shape& getShape() override;
	const sf::Shape& getShape() const override;

private:

};

class ContainerWidget : public RectangleWidget {
public:
	ContainerWidget(WidgetList& widget_list);
	void setAutoResize(bool value);
	void setHorizontal(bool value);
	void setPadding(float padding);
	void setHorizontalPadding(float padding);
	void setVerticalPadding(float padding);
	void setVerticalAlignment(Alignment alignment);
	void setHorizontalAlignment(Alignment alignment);

protected:
	void update() override;
	Anchor alignmentToAnchor(Alignment alignment) const;
	float alignmentToOffset(Alignment alignment, float max_size) const;

private:
	bool auto_resize = true;
	bool horizontal = true;
	float horizontal_padding = 0.0f;
	float vertical_padding = 0.0f;
	Alignment vertical_alignment = ALIGN_TOP;
	Alignment horizontal_alignment = ALIGN_LEFT;

};

class CheckboxWidget : public RectangleWidget {
public:
	std::function<void(bool)> OnValueChanged = [](bool new_value) { };

	CheckboxWidget(WidgetList& widget_list);
	bool isFocusable() const override;
	bool getValue() const;
	const sf::Color& getFillColor() const override;
	const sf::Color& getHighlightFillColor() const;
	const sf::Color& getCheckFillColor() const;
	void setFillColor(const sf::Color& color) override;
	void setHighlightFillColor(const sf::Color& color);
	void setCheckFillColor(const sf::Color& color);
	void setValueSilent(bool value);
	void setValue(bool value);
	void toggleValue();

protected:
	void update() override;
	void internalOnClick(const sf::Vector2f& pos) override;
	void internalOnMouseEnter(const sf::Vector2f& pos) override;
	void internalOnMouseExit(const sf::Vector2f& pos) override;

private:
	bool checked = false;
	const sf::Vector2f DEFAULT_SIZE = sf::Vector2f(20.0f, 20.0f);
	float check_size = 0.6f;
	RectangleWidget* check_widget = nullptr;
	sf::Color background_fill_color = sf::Color(50, 50, 50);
	sf::Color highlight_fill_color = sf::Color(100, 100, 100);
	sf::Color check_fill_color = sf::Color(255, 128, 0);

};

class TextWidget : public Widget {
public:
	TextWidget(WidgetList& widget_list);
	bool isVisualPositionQuantized() const override;
	sf::FloatRect getLocalBounds() const override;
	sf::FloatRect getParentLocalBounds() const override;
	sf::FloatRect getGlobalBounds() const override;
	sf::FloatRect getVisualLocalBounds() const override;
	sf::FloatRect getVisualParentLocalBounds() const override;
	sf::FloatRect getVisualGlobalBounds() const override;
	const sf::Font* getFont() const;
	size_t getStringSize() const;
	unsigned int getCharacterSize() const;
	const sf::Color& getFillColor() const override;
	const sf::String& getString() const;
	float getKerning(size_t index) const;
	sf::Vector2f getLocalCharPos(size_t index, bool top_aligned, bool with_kerning) const;
	sf::Vector2f getParentLocalCharPos(size_t index, bool top_aligned, bool with_kerning) const;
	sf::Vector2f getGlobalCharPos(size_t index, bool top_aligned, bool with_kerning) const;
	size_t getCharAt(const sf::Vector2f& pos) const;
	void setFont(const sf::Font& font);
	void setCharacterSize(unsigned int size);
	void setFillColor(const sf::Color& color) override;
	void setAdjustLocalBounds(bool value);
	void setString(const sf::String& string);
	void insert(size_t pos, const sf::String& str);
	void erase(size_t index_first, size_t count = 1);

protected:
	bool adjust_local_bounds = true;

	sf::Drawable& getDrawable() override;
	const sf::Drawable& getDrawable() const override;
	sf::Transformable& getTransformable() override;
	const sf::Transformable& getTransformable() const override;
	sf::Vector2f getRenderPositionOffset() const override;
	void update() override;

private:
	sf::Text text;

};

class TextBoxWidget : public RectangleWidget {
public:
	enum TextBoxType {
		TEXT,
		INTEGER,
		FLOAT,
	};
	std::function<void(bool)> OnEditModeToggle = [](bool new_value) { };
	std::function<void(const sf::String&)> OnValueChanged = [](const sf::String& new_value) { };
	std::function<void(const sf::String&)> OnConfirm = [](const sf::String& value) { };
	std::function<void()> OnCancel = []() { };

	TextBoxWidget(WidgetList& widget_list);
	bool isFocusable() const override;
	const sf::Color& getFillColor() const override;
	const sf::Color& getHighlightColor() const;
	const sf::Color& getTextColor() const;
	const sf::Color& getEditorColor() const;
	const sf::Color& getEditorTextColor() const;
	const sf::Color& getFailFillColor() const;
	const sf::Color& getEditFailFillColor() const;
	const sf::Font* getFont() const;
	unsigned int getCharacterSize() const;
	const sf::String& getValue() const;
	bool isValidValue() const;
	long long getLLValue() const;
	float getFloatValue() const;
	size_t getStringSize() const;
	TextBoxType getType() const;
	size_t getCursorPos() const;
	sf::Vector2f getLocalCharPos(size_t index, bool top_aligned, bool with_kerning) const;
	sf::Vector2f getGlobalCharPos(size_t index, bool top_aligned, bool with_kerning) const;
	void setFillColor(const sf::Color& color) override;
	void setHighlightColor(const sf::Color& color);
	void setTextColor(const sf::Color& color);
	void setEditorColor(const sf::Color& color);
	void setEditorTextColor(const sf::Color& color);
	void setFailFillColor(const sf::Color& color);
	void setEditFailFillColor(const sf::Color& color);
	void setFont(const sf::Font& font);
	void setCharacterSize(unsigned int size);
	void setValueSilent(const sf::String& value);
	void setValue(const sf::String& value);
	void setType(TextBoxType type);
	void setCursorPos(size_t pos);
	void typeChar(sf::Uint32 code);
	void insert(size_t pos, const sf::String& str);
	void erase(size_t index_first, size_t count);
	void processKeyboardEvent(const sf::Event& event);

protected:
	void update() override;
	void updateColors();
	void internalOnClick(const sf::Vector2f& pos) override;
	void internalOnEditModeToggle(bool value);
	void internalOnFocused() override;
	void internalOnFocusLost() override;
	void internalOnMouseEnter(const sf::Vector2f& pos) override;
	void internalOnMouseExit(const sf::Vector2f& pos) override;
	void internalOnValueChanged(const sf::String& new_value);
	void internalOnConfirm(const sf::String& value);
	void internalOnCancel();
	void updateValid();
	void setEditMode(bool value);
	void insertSilent(size_t pos, const sf::String& str);
	void eraseSilent(size_t index_first, size_t count);

private:
	const sf::Vector2f DEFAULT_SIZE = sf::Vector2f(40.0f, 20.0f);
	const sf::Vector2f TEXT_VIEW_ZERO_POS = sf::Vector2f(2.0f, 0.0f);
	const sf::Vector2f CURSOR_OFFSET = sf::Vector2f(0.0f, 0.0f);
	const std::string VALID_INTEGER_CHARS = "+-0123456789";
	const std::string VALID_FLOAT_CHARS = "+-.e0123456789";
	const float CURSOR_MOVE_MARGIN = 2.0f;
	TextWidget* text_widget = nullptr;
	RectangleWidget* cursor_widget = nullptr;
	TextBoxType type = TextBoxType::TEXT;
	size_t cursor_pos = 0;
	bool edit_mode = false;
	bool highlighted = false;
	bool fail_state = false;
	sf::Vector2f text_view_pos;
	sf::Color background_color = sf::Color(50, 50, 50);
	sf::Color highlight_color = sf::Color(100, 100, 100);
	sf::Color text_color = sf::Color(255, 255, 255);
	sf::Color editor_color = sf::Color(255, 255, 255);
	sf::Color editor_text_color = sf::Color(0, 0, 0);
	sf::Color fail_background_color = sf::Color(128, 0, 0);
	sf::Color editor_fail_background_color = sf::Color(255, 128, 128);
};

class WidgetList {
public:
	bool debug_render = false;

	WidgetList();
	bool contains(const Widget* widget);
	bool isClickBlocked() const;
	bool isReleaseBlocked() const;
	Widget* getRootWidget() const;
	Widget* getFocusedWidget() const;
	Widget* find(const std::string& name) const;
	template<typename T, typename... Args>
	requires std::derived_from<T, Widget>
	T* createWidget(Args&&... args);
	void processClick(const sf::Vector2f pos);
	void processRelease(const sf::Vector2f pos);
	void processKeyboardEvent(const sf::Event& event);
	void render(sf::RenderTarget& target);
	void reset(const sf::Vector2f& root_size);
	void setFocusedWidget(Widget* widget);

private:
	friend class Widget;
	bool click_blocked = false;
	bool release_blocked = false;
	float render_origin_size = 10.0f;
	sf::Color render_bounds_color = sf::Color::Green;
	sf::Color render_origin_color = sf::Color::Red;
	sf::Color focused_widget_bounds_color = sf::Color(0, 200, 255);
	CompoundVectorUptr<Widget> widgets;
	RectangleWidget* root_widget = nullptr;
	Widget* focused_widget_temp = nullptr;
	Widget* focused_widget = nullptr;

};

template<typename T, typename... Args>
requires std::derived_from<T, Widget>
inline T* WidgetList::createWidget(Args&&... args) {
	std::unique_ptr<T> uptr = std::make_unique<T>(*this, args...);
	T* ptr = uptr.get();
	if (root_widget) {
		ptr->setParentSilent(root_widget);
	}
	widgets.add(std::move(uptr));
	return ptr;
}
