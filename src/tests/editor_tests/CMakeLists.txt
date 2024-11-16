set(EDITOR_TESTS_HEADER_FILES
    "${TESTS_INCLUDE_DIR}/editor_tests.h"
)
set(EDITOR_TESTS_SOURCE_FILES
    "editor_tests.cpp"
)
add_library(editor_tests ${EDITOR_TESTS_HEADER_FILES} ${EDITOR_TESTS_SOURCE_FILES})
source_group(TREE ${TESTS_INCLUDE_DIR} PREFIX "Header Files" FILES ${EDITOR_TESTS_HEADER_FILES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX "Source Files" FILES ${EDITOR_TESTS_SOURCE_FILES})
set_property(TARGET editor_tests PROPERTY FOLDER "tests")

target_include_directories(editor_tests PRIVATE "${INCLUDE_DIR}")
target_include_directories(editor_tests PUBLIC "${CMAKE_SOURCE_DIR}")
target_include_directories(editor_tests PUBLIC "${CMAKE_SOURCE_DIR}/test_lib/include/")
target_include_directories(editor_tests PUBLIC "${CMAKE_SOURCE_DIR}/box2d/include/")
target_include_directories(editor_tests PUBLIC "${CMAKE_SOURCE_DIR}/sfml/include/")
target_link_libraries(editor_tests PUBLIC test_lib)
target_link_libraries(editor_tests PUBLIC common_lib)
target_link_libraries(editor_tests PUBLIC widgets_lib)
target_link_libraries(editor_tests PUBLIC simulation_lib)
target_link_libraries(editor_tests PUBLIC editor)
set_property(TARGET editor_tests PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}")
