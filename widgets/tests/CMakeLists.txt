set(WIDGET_TESTS_HEADER_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/include/widget_tests/widget_test.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/widget_tests/widget_tests.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/widget_tests/widget_tests_application.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/widget_tests/widget_tests_basic.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/widget_tests/widget_tests_button.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/widget_tests/widget_tests_canvas.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/widget_tests/widget_tests_checkbox.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/widget_tests/widget_tests_container.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/widget_tests/widget_tests_dropdown.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/widget_tests/widget_tests_render.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/widget_tests/widget_tests_post_actions.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/widget_tests/widget_tests_scroll_area.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/widget_tests/widget_tests_size_policy.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/widget_tests/widget_tests_text.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/widget_tests/widget_tests_textbox.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/widget_tests/widget_tests_tree_view.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/widget_tests/widget_tests_toposort.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/widget_tests/widget_tests_widget_link.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/widget_tests/widget_tests_window.h"
)
set(WIDGET_TESTS_SOURCE_FILES
    "src/widget_test.cpp"
    "src/widget_tests.cpp"
    "src/widget_tests_application.cpp"
    "src/widget_tests_basic.cpp"
    "src/widget_tests_button.cpp"
    "src/widget_tests_canvas.cpp"
    "src/widget_tests_checkbox.cpp"
    "src/widget_tests_container.cpp"
    "src/widget_tests_dropdown.cpp"
    "src/widget_tests_render.cpp"
    "src/widget_tests_post_actions.cpp"
    "src/widget_tests_scroll_area.cpp"
    "src/widget_tests_size_policy.cpp"
    "src/widget_tests_text.cpp"
    "src/widget_tests_textbox.cpp"
    "src/widget_tests_toposort.cpp"
    "src/widget_tests_tree_view.cpp"
    "src/widget_tests_widget_link.cpp"
    "src/widget_tests_window.cpp"
)
add_library(widgets_tests ${WIDGET_TESTS_HEADER_FILES} ${WIDGET_TESTS_SOURCE_FILES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/include PREFIX "Header Files" FILES ${WIDGET_TESTS_HEADER_FILES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX "Source Files" FILES ${WIDGET_TESTS_SOURCE_FILES})
set_property(TARGET widgets_tests PROPERTY FOLDER "tests")

add_executable(RUN_WIDGET_TESTS "src/main.cpp")
target_link_libraries(RUN_WIDGET_TESTS PUBLIC widgets_tests)
target_include_directories(widgets_tests PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../include")
target_include_directories(widgets_tests PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_include_directories(widgets_tests PUBLIC "${CMAKE_SOURCE_DIR}")
target_include_directories(widgets_tests PUBLIC "${INCLUDE_DIR}")

target_link_libraries(widgets_tests PUBLIC logger)
target_link_libraries(widgets_tests PUBLIC test_lib)
target_link_libraries(widgets_tests PUBLIC widgets_lib)
target_link_libraries(widgets_tests PUBLIC common_lib)
set_property(TARGET widgets_tests PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}")
