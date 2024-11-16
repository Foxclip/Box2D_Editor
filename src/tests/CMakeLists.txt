set(TESTS_INCLUDE_DIR "${INCLUDE_DIR}/tests/")

set(TESTS_HEADER_FILES
    "${TESTS_INCLUDE_DIR}/tests.h"
)
set(TESTS_SOURCE_FILES
    "main.cpp"
)
add_executable(run_tests ${TESTS_HEADER_FILES} ${TESTS_SOURCE_FILES})
source_group(TREE ${TESTS_INCLUDE_DIR} PREFIX "Header Files" FILES ${TESTS_HEADER_FILES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX "Source Files" FILES ${TESTS_SOURCE_FILES})
#set_property(TARGET run_tests PROPERTY FOLDER "tests")

target_include_directories(run_tests PRIVATE "${INCLUDE_DIR}")
target_include_directories(run_tests PUBLIC "${CMAKE_SOURCE_DIR}")
target_include_directories(run_tests PUBLIC "${CMAKE_SOURCE_DIR}/test_lib/include/")
target_include_directories(run_tests PUBLIC "${CMAKE_SOURCE_DIR}/box2d/include/")
target_include_directories(run_tests PUBLIC "${CMAKE_SOURCE_DIR}/sfml/include/")
target_link_libraries(run_tests PUBLIC test_lib)
target_link_libraries(run_tests PUBLIC common_lib)
target_link_libraries(run_tests PUBLIC widgets_lib)
target_link_libraries(run_tests PUBLIC simulation_lib)
target_link_libraries(run_tests PUBLIC editor)
target_link_libraries(run_tests PUBLIC common_tests)
target_link_libraries(run_tests PUBLIC widget_tests)
target_link_libraries(run_tests PUBLIC editor_tests)
set_property(TARGET run_tests PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}")

add_subdirectory(common_tests)
add_subdirectory(widget_tests)
add_subdirectory(editor_tests)
