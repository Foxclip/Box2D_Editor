add_executable(tests)

target_sources(tests PRIVATE
    "${INCLUDE_DIR}/tests/compvector_tests.h"
    "${INCLUDE_DIR}/tests/event_tests.h"
    "${INCLUDE_DIR}/tests/logger_tests.h"
    "${INCLUDE_DIR}/tests/searchindex_tests.h"
    "${INCLUDE_DIR}/tests/simulation_tests.h"
    "${INCLUDE_DIR}/tests/widget_tests.h"
    "${INCLUDE_DIR}/tests/test.h"
    "${INCLUDE_DIR}/tests/tests.h"
)
target_sources(tests PRIVATE
    "compvector_tests.cpp"
    "event_tests.cpp"
    "logger_tests.cpp"
    "main.cpp"
    "searchindex_tests.cpp"
    "simulation_tests.cpp"
    "test.cpp"
    "widget_tests.cpp"
)
target_include_directories(tests PRIVATE "${INCLUDE_DIR}")
target_include_directories(tests PUBLIC "${CMAKE_SOURCE_DIR}")
target_include_directories(tests PUBLIC "${CMAKE_SOURCE_DIR}/sfml/include/")
target_include_directories(tests PUBLIC "${CMAKE_SOURCE_DIR}/box2d/include/")
target_link_libraries(tests PUBLIC common_lib)
target_link_libraries(tests PUBLIC widgets_lib)
target_link_libraries(tests PUBLIC simulation_lib)
set_property(TARGET tests PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}")
