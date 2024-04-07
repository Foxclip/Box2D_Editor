add_library(tests_lib)
target_sources(tests_lib PRIVATE
    "${INCLUDE_DIR}/tests/simulation_tests.h"
)
target_sources(tests_lib PRIVATE
    "simulation_tests.cpp"
)
target_include_directories(tests_lib PRIVATE "${INCLUDE_DIR}")
target_include_directories(tests_lib PUBLIC "${CMAKE_SOURCE_DIR}")
target_include_directories(tests_lib PUBLIC "${CMAKE_SOURCE_DIR}/sfml/include/")
target_include_directories(tests_lib PUBLIC "${CMAKE_SOURCE_DIR}/box2d/include/")
