set(TESTS_INCLUDE_DIR "${INCLUDE_DIR}/tests/")

set(TESTS_HEADER_FILES
    "${INCLUDE_DIR}/tests/example/test_module_1.h"
    "${INCLUDE_DIR}/tests/example/test_module_2.h"
    "${INCLUDE_DIR}/tests/compvector_tests.h"
    "${INCLUDE_DIR}/tests/event_tests.h"
    "${INCLUDE_DIR}/tests/logger_tests.h"
    "${INCLUDE_DIR}/tests/searchindex_tests.h"
    "${INCLUDE_DIR}/tests/simulation_tests.h"
    "${INCLUDE_DIR}/tests/widget_tests.h"
    "${INCLUDE_DIR}/tests/test.h"
    "${INCLUDE_DIR}/tests/tests.h"
)
set(TESTS_SOURCE_FILES
    "example/test_module_1.cpp"
    "example/test_module_2.cpp"
    "example/main.cpp"
    "compvector_tests.cpp"
    "event_tests.cpp"
    "logger_tests.cpp"
    "main.cpp"
    "searchindex_tests.cpp"
    "simulation_tests.cpp"
    "test.cpp"
    "widget_tests.cpp"
)
add_executable(tests ${TESTS_HEADER_FILES} ${TESTS_SOURCE_FILES})
source_group(TREE ${TESTS_INCLUDE_DIR} PREFIX "Header Files" FILES ${TESTS_HEADER_FILES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX "Source Files" FILES ${TESTS_SOURCE_FILES})

target_include_directories(tests PRIVATE "${INCLUDE_DIR}")
target_include_directories(tests PUBLIC "${CMAKE_SOURCE_DIR}")
target_include_directories(tests PUBLIC "${CMAKE_SOURCE_DIR}/sfml/include/")
target_include_directories(tests PUBLIC "${CMAKE_SOURCE_DIR}/box2d/include/")
target_link_libraries(tests PUBLIC common_lib)
target_link_libraries(tests PUBLIC widgets_lib)
target_link_libraries(tests PUBLIC simulation_lib)
set_property(TARGET tests PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}")
