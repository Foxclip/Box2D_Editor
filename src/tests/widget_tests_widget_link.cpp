#include "tests/widget_tests.h"

void WidgetTests::widgetLinkBasicTest(test::Test& test) {
    fw::Application application(window);
    application.init(test.name, 800, 600, 0, false);
    application.start(true);
    application.advance();
    sf::Vector2f size(30.0f, 30.0f);
    sf::Vector2f position1(100.0f, 100.0f);
    sf::Vector2f position1_1(50.0f, 50.0f);
    sf::Vector2f position1_2(position1.x + size.x, position1.y);
    fw::RectangleWidget* rectangle_1_widget = application.getWidgets().createRectangleWidget(size);
    fw::RectangleWidget* rectangle_2_widget = application.getWidgets().createRectangleWidget(size);
    rectangle_1_widget->setName("rect1");
    rectangle_1_widget->setPosition(position1);
    rectangle_2_widget->setName("rect2");
    rectangle_2_widget->setPosition(position1_1);
    rectangle_2_widget->addLink(
        "POS_X",
        { rectangle_1_widget->getPosXTarget(), rectangle_1_widget->getSizeXTarget() },
        [=]() {
            rectangle_2_widget->setGlobalPositionX(rectangle_1_widget->getGlobalTopRight().x);
        }
    );
    rectangle_2_widget->addLink(
        "POS_Y",
        rectangle_1_widget->getPosYTarget(),
        [=]() {
            rectangle_2_widget->setGlobalPositionY(rectangle_1_widget->getGlobalTopRight().y);
        }
    );
    application.advance();
    T_VEC2_APPROX_COMPARE(rectangle_2_widget->getPosition(), position1_2);
}

void WidgetTests::widgetLinkTargetsFuncTest(test::Test& test) {
    fw::Application application(window);
    application.init(test.name, 800, 600, 0, false);
    application.start(true);
    application.advance();
    sf::Vector2f size(30.0f, 30.0f);
    sf::Vector2f position1(100.0f, 100.0f);
    sf::Vector2f position1_1(50.0f, 50.0f);
    sf::Vector2f position1_2(position1.x + size.x, position1.y);
    fw::RectangleWidget* rectangle_1_widget = application.getWidgets().createRectangleWidget(size);
    fw::RectangleWidget* rectangle_2_widget = application.getWidgets().createRectangleWidget(size);
    rectangle_1_widget->setName("rect1");
    rectangle_1_widget->setPosition(position1);
    rectangle_2_widget->setName("rect2");
    rectangle_2_widget->setPosition(position1_1);
    rectangle_2_widget->addLink(
        "POS_X",
        [=]() {
            return CompVector<fw::WidgetUpdateTarget*>{ rectangle_1_widget->getPosXTarget(), rectangle_1_widget->getSizeXTarget() };
        },
        [=]() {
            rectangle_2_widget->setGlobalPositionX(rectangle_1_widget->getGlobalTopRight().x);
        }
    );
    rectangle_2_widget->addLink(
        "POS_Y",
        [=]() {
            return CompVector<fw::WidgetUpdateTarget*>{ rectangle_1_widget->getPosYTarget() };
        },
        [=]() {
            rectangle_2_widget->setGlobalPositionY(rectangle_1_widget->getGlobalTopRight().y);
        }
    );
    application.advance();
    T_VEC2_APPROX_COMPARE(rectangle_2_widget->getPosition(), position1_2);
}

void WidgetTests::widgetLinkContainerTest(test::Test& test) {
    fw::Application application(window);
    application.init(test.name, 800, 600, 0, false);
    application.start(true);
    application.advance();
    float container_padding = 10.0f;
    sf::Vector2f size(30.0f, 30.0f);
    sf::Vector2f position1(100.0f, 100.0f);
    sf::Vector2f position1_1(50.0f, 50.0f);
    sf::Vector2f position1_2(position1.x + container_padding + size.x, position1.y + container_padding);
    fw::ContainerWidget* container_widget = application.getWidgets().createContainerWidget(size);
    fw::RectangleWidget* rectangle_1_widget = application.getWidgets().createRectangleWidget(size);
    fw::RectangleWidget* rectangle_2_widget = application.getWidgets().createRectangleWidget(size);
    container_widget->setPosition(position1);
    container_widget->setPadding(container_padding);
    rectangle_1_widget->setName("rect1");
    rectangle_1_widget->setPosition(position1);
    rectangle_1_widget->setParent(container_widget);
    rectangle_2_widget->setName("rect2");
    rectangle_2_widget->setPosition(position1_1);
    rectangle_2_widget->addLink(
        "POS_X",
        { rectangle_1_widget->getPosXTarget(), rectangle_1_widget->getSizeXTarget() },
        [=]() {
            rectangle_2_widget->setGlobalPositionX(rectangle_1_widget->getGlobalTopRight().x);
        }
    );
    rectangle_2_widget->addLink(
        "POS_Y",
        rectangle_1_widget->getPosYTarget(),
        [=]() {
            rectangle_2_widget->setGlobalPositionY(rectangle_1_widget->getGlobalTopRight().y);
        }
    );
    application.advance();
    T_VEC2_APPROX_COMPARE(rectangle_2_widget->getPosition(), position1_2);
}

void WidgetTests::widgetLinkRemoveTest(test::Test& test) {
    fw::Application application(window);
    application.init(test.name, 800, 600, 0, false);
    application.start(true);
    application.advance();
    sf::Vector2f size(30.0f, 30.0f);
    sf::Vector2f position1(100.0f, 100.0f);
    sf::Vector2f position2(200.0f, 300.0f);
    sf::Vector2f position1_1(50.0f, 50.0f);
    sf::Vector2f position1_2(position1.x + size.x, position1.y);
    sf::Vector2f position2_1(position1.x + size.x, position2.y);
    sf::Vector2f position3(400.0f, 400.0f);
    fw::RectangleWidget* rectangle_1_widget = application.getWidgets().createRectangleWidget(size);
    fw::RectangleWidget* rectangle_2_widget = application.getWidgets().createRectangleWidget(size);
    rectangle_1_widget->setName("rect1");
    rectangle_1_widget->setPosition(position1);
    rectangle_2_widget->setName("rect2");
    rectangle_2_widget->setPosition(position1_1);
    fw::WidgetLink* link1 = rectangle_2_widget->addLink(
        "POS_X",
        { rectangle_1_widget->getPosXTarget(), rectangle_1_widget->getSizeXTarget() },
        [=]() {
            rectangle_2_widget->setGlobalPositionX(rectangle_1_widget->getGlobalTopRight().x);
        }
    );
    fw::WidgetLink* link2 = rectangle_2_widget->addLink(
        "POS_Y",
        rectangle_1_widget->getPosYTarget(),
        [=]() {
            rectangle_2_widget->setGlobalPositionY(rectangle_1_widget->getGlobalTopRight().y);
        }
    );
    fw::WidgetLink* link3 = rectangle_2_widget->addLink(
        "LINK_3",
        link1,
        [=]() {
            // nothing
        }
    );
    fw::WidgetLink* link4 = rectangle_2_widget->addLink(
        "LINK_4",
        link2,
        [=]() {
            // nothing
        }
    );
    application.advance();
    T_ASSERT(T_VEC2_APPROX_COMPARE(rectangle_2_widget->getPosition(), position1_2));

    link1->remove();
    rectangle_1_widget->setPosition(position2);
    application.advance();
    T_ASSERT(T_VEC2_APPROX_COMPARE(rectangle_2_widget->getPosition(), position2_1));

    rectangle_1_widget->remove();
    rectangle_2_widget->setPosition(position3);
    application.advance();
    T_ASSERT(T_VEC2_APPROX_COMPARE(rectangle_2_widget->getPosition(), position3));
}
