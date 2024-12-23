set(WIDGET_TESTS_INCLUDE_DIR "${TESTS_INCLUDE_DIR}/widget_tests/")

set(WIDGET_TESTS_HEADER_FILES
    "${WIDGET_TESTS_INCLUDE_DIR}/widget_test.h"
    "${WIDGET_TESTS_INCLUDE_DIR}/widget_tests.h"
    "${WIDGET_TESTS_INCLUDE_DIR}/widget_tests_application.h"
    "${WIDGET_TESTS_INCLUDE_DIR}/widget_tests_basic.h"
    "${WIDGET_TESTS_INCLUDE_DIR}/widget_tests_canvas.h"
    "${WIDGET_TESTS_INCLUDE_DIR}/widget_tests_checkbox.h"
    "${WIDGET_TESTS_INCLUDE_DIR}/widget_tests_container.h"
    "${WIDGET_TESTS_INCLUDE_DIR}/widget_tests_dropdown.h"
    "${WIDGET_TESTS_INCLUDE_DIR}/widget_tests_scroll_area.h"
    "${WIDGET_TESTS_INCLUDE_DIR}/widget_tests_size_policy.h"
    "${WIDGET_TESTS_INCLUDE_DIR}/widget_tests_text.h"
    "${WIDGET_TESTS_INCLUDE_DIR}/widget_tests_textbox.h"
    "${WIDGET_TESTS_INCLUDE_DIR}/widget_tests_tree_view.h"
    "${WIDGET_TESTS_INCLUDE_DIR}/widget_tests_toposort.h"
    "${WIDGET_TESTS_INCLUDE_DIR}/widget_tests_widget_link.h"
    "${WIDGET_TESTS_INCLUDE_DIR}/widget_tests_window.h"
)
set(WIDGET_TESTS_SOURCE_FILES
    "widget_test.cpp"
    "widget_tests.cpp"
    "widget_tests_application.cpp"
    "widget_tests_basic.cpp"
    "widget_tests_canvas.cpp"
    "widget_tests_checkbox.cpp"
    "widget_tests_container.cpp"
    "widget_tests_dropdown.cpp"
    "widget_tests_scroll_area.cpp"
    "widget_tests_size_policy.cpp"
    "widget_tests_text.cpp"
    "widget_tests_textbox.cpp"
    "widget_tests_toposort.cpp"
    "widget_tests_tree_view.cpp"
    "widget_tests_widget_link.cpp"
    "widget_tests_window.cpp"
)
add_library(widget_tests ${WIDGET_TESTS_HEADER_FILES} ${WIDGET_TESTS_SOURCE_FILES})
source_group(TREE ${WIDGET_TESTS_INCLUDE_DIR} PREFIX "Header Files" FILES ${WIDGET_TESTS_HEADER_FILES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX "Source Files" FILES ${WIDGET_TESTS_SOURCE_FILES})
set_property(TARGET widget_tests PROPERTY FOLDER "tests")

target_include_directories(widget_tests PRIVATE "${INCLUDE_DIR}")
target_include_directories(widget_tests PUBLIC "${CMAKE_SOURCE_DIR}")
target_include_directories(widget_tests PUBLIC "${CMAKE_SOURCE_DIR}/test_lib/include/")
target_include_directories(widget_tests PUBLIC "${CMAKE_SOURCE_DIR}/box2d/include/")
target_include_directories(widget_tests PUBLIC "${CMAKE_SOURCE_DIR}/sfml/include/")
target_link_libraries(widget_tests PUBLIC test_lib)
target_link_libraries(widget_tests PUBLIC common_lib)
target_link_libraries(widget_tests PUBLIC widgets_lib)
target_link_libraries(widget_tests PUBLIC simulation_lib)
set_property(TARGET widget_tests PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}")
