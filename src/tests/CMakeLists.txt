set(TESTS_INCLUDE_DIR "${INCLUDE_DIR}/tests/")

set(TESTS_HEADER_FILES
    "${INCLUDE_DIR}/tests/compvector_tests.h"
    "${INCLUDE_DIR}/tests/data_pointer_tests.h"
    "${INCLUDE_DIR}/tests/editor_tests.h"
    "${INCLUDE_DIR}/tests/event_tests.h"
    "${INCLUDE_DIR}/tests/logger_tests.h"
    "${INCLUDE_DIR}/tests/searchindex_tests.h"
    "${INCLUDE_DIR}/tests/simulation_tests.h"
    "${INCLUDE_DIR}/tests/tests.h"
    "${INCLUDE_DIR}/tests/widget_tests.h"
)
set(TESTS_SOURCE_FILES
    "compvector_tests.cpp"
    "data_pointer_tests.cpp"
    "editor_tests.cpp"
    "event_tests.cpp"
    "logger_tests.cpp"
    "main.cpp"
    "searchindex_tests.cpp"
    "simulation_tests.cpp"
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
add_executable(tests ${TESTS_HEADER_FILES} ${TESTS_SOURCE_FILES})
source_group(TREE ${TESTS_INCLUDE_DIR} PREFIX "Header Files" FILES ${TESTS_HEADER_FILES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX "Source Files" FILES ${TESTS_SOURCE_FILES})

target_include_directories(tests PRIVATE "${INCLUDE_DIR}")
target_include_directories(tests PUBLIC "${CMAKE_SOURCE_DIR}")
target_include_directories(tests PUBLIC "${CMAKE_SOURCE_DIR}/test_lib/include/")
target_include_directories(tests PUBLIC "${CMAKE_SOURCE_DIR}/box2d/include/")
target_include_directories(tests PUBLIC "${CMAKE_SOURCE_DIR}/sfml/include/")
target_link_libraries(tests PUBLIC test_lib)
target_link_libraries(tests PUBLIC common_lib)
target_link_libraries(tests PUBLIC widgets_lib)
target_link_libraries(tests PUBLIC simulation_lib)
target_link_libraries(tests PUBLIC editor)
set_property(TARGET tests PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}")
