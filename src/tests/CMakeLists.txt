set(TESTS_INCLUDE_DIR "${INCLUDE_DIR}/tests/")

set(TESTS_HEADER_FILES
    "${TESTS_INCLUDE_DIR}/tests.h"
)
set(TESTS_SOURCE_FILES
    "main.cpp"
)
add_executable(RUN_ALL_TESTS ${TESTS_HEADER_FILES} ${TESTS_SOURCE_FILES})
source_group(TREE ${TESTS_INCLUDE_DIR} PREFIX "Header Files" FILES ${TESTS_HEADER_FILES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX "Source Files" FILES ${TESTS_SOURCE_FILES})
#set_property(TARGET RUN_ALL_TESTS PROPERTY FOLDER "tests")

target_include_directories(RUN_ALL_TESTS PRIVATE "${INCLUDE_DIR}")
target_include_directories(RUN_ALL_TESTS PUBLIC "${CMAKE_SOURCE_DIR}")
target_include_directories(RUN_ALL_TESTS PUBLIC "${CMAKE_SOURCE_DIR}/test_lib/include/")
target_include_directories(RUN_ALL_TESTS PUBLIC "${CMAKE_SOURCE_DIR}/box2d/include/")
target_include_directories(RUN_ALL_TESTS PUBLIC "${CMAKE_SOURCE_DIR}/sfml/include/")
target_link_libraries(RUN_ALL_TESTS PUBLIC test_lib)
target_link_libraries(RUN_ALL_TESTS PUBLIC common_lib)
target_link_libraries(RUN_ALL_TESTS PUBLIC widgets_lib)
target_link_libraries(RUN_ALL_TESTS PUBLIC simulation_lib)
target_link_libraries(RUN_ALL_TESTS PUBLIC editor)
target_link_libraries(RUN_ALL_TESTS PUBLIC common_tests)
target_link_libraries(RUN_ALL_TESTS PUBLIC widget_tests)
target_link_libraries(RUN_ALL_TESTS PUBLIC editor_tests)
set_property(TARGET RUN_ALL_TESTS PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}")

add_subdirectory(common_tests)
add_subdirectory(widget_tests)
add_subdirectory(editor_tests)
