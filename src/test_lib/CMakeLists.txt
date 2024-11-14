set(TEST_LIB_INCLUDE_DIR "${INCLUDE_DIR}/test_lib/")

set(TEST_LIB_HEADER_FILES
    "${INCLUDE_DIR}/test_lib/example/test_list_3.h"
    "${INCLUDE_DIR}/test_lib/example/test_module_1.h"
    "${INCLUDE_DIR}/test_lib/example/test_module_2.h"
    "${INCLUDE_DIR}/test_lib/test.h"
)
set(TEST_LIB_SOURCE_FILES
    "example/test_list_3.cpp"
    "example/test_module_1.cpp"
    "example/test_module_2.cpp"
    "main.cpp"
    "test.cpp"
)
add_library(test_lib ${TEST_LIB_HEADER_FILES} ${TEST_LIB_SOURCE_FILES})
source_group(TREE ${TEST_LIB_INCLUDE_DIR} PREFIX "Header Files" FILES ${TEST_LIB_HEADER_FILES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX "Source Files" FILES ${TEST_LIB_SOURCE_FILES})
set_property(TARGET test_lib PROPERTY FOLDER "test_lib")

target_include_directories(test_lib PRIVATE "${INCLUDE_DIR}")
target_include_directories(test_lib PUBLIC "${CMAKE_SOURCE_DIR}")
target_link_libraries(test_lib PUBLIC common_lib)
set_property(TARGET test_lib PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}")
