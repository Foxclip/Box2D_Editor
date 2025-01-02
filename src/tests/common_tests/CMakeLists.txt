set(COMMON_TESTS_HEADER_FILES
    "${TESTS_INCLUDE_DIR}/compvector_tests.h"
    "${TESTS_INCLUDE_DIR}/data_pointer_shared_tests.h"
    "${TESTS_INCLUDE_DIR}/data_pointer_tests.h"
    "${TESTS_INCLUDE_DIR}/data_pointer_unique_tests.h"
    "${TESTS_INCLUDE_DIR}/event_tests.h"
    "${TESTS_INCLUDE_DIR}/logger_tests.h"
    "${TESTS_INCLUDE_DIR}/searchindex_tests.h"
    "${TESTS_INCLUDE_DIR}/simulation_tests.h"
)
set(COMMON_TESTS_SOURCE_FILES
    "compvector_tests.cpp"
    "data_pointer_shared_tests.cpp"
    "data_pointer_tests.cpp"
    "data_pointer_unique_tests.cpp"
    "event_tests.cpp"
    "logger_tests.cpp"
    "searchindex_tests.cpp"
    "simulation_tests.cpp"
)
add_library(common_tests ${COMMON_TESTS_HEADER_FILES} ${COMMON_TESTS_SOURCE_FILES})
source_group(TREE ${TESTS_INCLUDE_DIR} PREFIX "Header Files" FILES ${COMMON_TESTS_HEADER_FILES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX "Source Files" FILES ${COMMON_TESTS_SOURCE_FILES})
set_property(TARGET common_tests PROPERTY FOLDER "tests")

target_include_directories(common_tests PRIVATE "${INCLUDE_DIR}")
target_include_directories(common_tests PUBLIC "${CMAKE_SOURCE_DIR}")
target_include_directories(common_tests PUBLIC "${CMAKE_SOURCE_DIR}/test_lib/include/")
target_include_directories(common_tests PUBLIC "${CMAKE_SOURCE_DIR}/box2d/include/")
target_include_directories(common_tests PUBLIC "${CMAKE_SOURCE_DIR}/sfml/include/")
target_link_libraries(common_tests PUBLIC test_lib)
target_link_libraries(common_tests PUBLIC common_lib)
target_link_libraries(common_tests PUBLIC simulation_lib)
set_property(TARGET common_tests PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}")
