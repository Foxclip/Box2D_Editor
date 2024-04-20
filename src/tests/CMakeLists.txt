add_library(tests_lib)
target_sources(tests_lib PRIVATE
    "${INCLUDE_DIR}/tests/compvector_tests.h"
    "${INCLUDE_DIR}/tests/logger_tests.h"
    "${INCLUDE_DIR}/tests/searchindex_tests.h"
    "${INCLUDE_DIR}/tests/simulation_tests.h"
    "${INCLUDE_DIR}/tests/widget_tests.h"
    "${INCLUDE_DIR}/tests/tests.h"
)
target_sources(tests_lib PRIVATE
    "compvector_tests.cpp"
    "logger_tests.cpp"
    "searchindex_tests.cpp"
    "simulation_tests.cpp"
    "widget_tests.cpp"
)
target_include_directories(tests_lib PRIVATE "${INCLUDE_DIR}")
target_include_directories(tests_lib PUBLIC "${CMAKE_SOURCE_DIR}")
target_include_directories(tests_lib PUBLIC "${CMAKE_SOURCE_DIR}/sfml/include/")
target_include_directories(tests_lib PUBLIC "${CMAKE_SOURCE_DIR}/box2d/include/")
