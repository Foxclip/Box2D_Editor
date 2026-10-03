set(COMMON_INCLUDE_DIR "${INCLUDE_DIR}/common")

set(COMMON_HEADER_FILES
    "${COMMON_INCLUDE_DIR}/filedialog.h"
    "${COMMON_INCLUDE_DIR}/utils.h"
)
set(COMMON_SOURCE_FILES
    "filedialog.cpp"
    "utils.cpp"
)

add_library(common_lib ${COMMON_HEADER_FILES} ${COMMON_SOURCE_FILES})
source_group(TREE ${COMMON_INCLUDE_DIR} PREFIX "Header Files" FILES ${COMMON_HEADER_FILES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX "Source Files" FILES ${COMMON_SOURCE_FILES})

target_include_directories(common_lib PUBLIC "${INCLUDE_DIR}")
target_include_directories(common_lib PUBLIC "${CMAKE_SOURCE_DIR}")
target_include_directories(common_lib PUBLIC "${CMAKE_SOURCE_DIR}/box2d/include/")
target_link_libraries(common_lib PUBLIC widgets_lib)
