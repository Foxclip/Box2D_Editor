set(INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include")

add_subdirectory(widgets)
add_subdirectory(tests)

set(SRC_HEADER_FILES
    "${INCLUDE_DIR}/compvector.h"
    "${INCLUDE_DIR}/editor.h"
    "${INCLUDE_DIR}/global.h"
    "${INCLUDE_DIR}/history.h"
    "${INCLUDE_DIR}/logger.h"
    "${INCLUDE_DIR}/main.h"
    "${INCLUDE_DIR}/scenes.h"
    "${INCLUDE_DIR}/searchindex.h"
    "${INCLUDE_DIR}/serializer.h"
    "${INCLUDE_DIR}/test.h"
    "${INCLUDE_DIR}/tools.h"
    "${INCLUDE_DIR}/utils.h"
    "${INCLUDE_DIR}/simulation/gameobject.h"
    "${INCLUDE_DIR}/simulation/objectlist.h"
    "${INCLUDE_DIR}/simulation/polygon.h"
    "${INCLUDE_DIR}/simulation/shapes.h"
    "${INCLUDE_DIR}/simulation/simulation.h"
    "${INCLUDE_DIR}/UI/create_panel.h"
    "${INCLUDE_DIR}/UI/edit_window.h"
    "${INCLUDE_DIR}/UI/toolbox.h"
)
set(SRC_SOURCE_FILES
    "editor.cpp"
    "global.cpp"
    "logger.cpp"
    "main.cpp"
    "serializer.cpp"
    "test.cpp"
    "tools.cpp"
    "utils.cpp"
    "simulation/gameobject.cpp"
    "simulation/objectlist.cpp"
    "simulation/polygon.cpp"
    "simulation/shapes.cpp"
    "simulation/simulation.cpp"
    "UI/create_panel.cpp"
    "UI/edit_window.cpp"
    "UI/toolbox.cpp"
)
add_library(src_lib ${SRC_HEADER_FILES} ${SRC_SOURCE_FILES})
source_group(TREE ${INCLUDE_DIR} PREFIX "Header Files" FILES ${SRC_HEADER_FILES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX "Source Files" FILES ${SRC_SOURCE_FILES})

target_include_directories(src_lib PUBLIC "${INCLUDE_DIR}")
target_include_directories(src_lib PUBLIC "${CMAKE_SOURCE_DIR}")
target_include_directories(src_lib PUBLIC "${CMAKE_SOURCE_DIR}/sfml/include/")
target_include_directories(src_lib PUBLIC "${CMAKE_SOURCE_DIR}/box2d/include/")
target_link_libraries(src_lib PRIVATE widgets_lib)
target_link_libraries(src_lib PRIVATE tests_lib)
target_link_libraries(src_lib PUBLIC sfml-graphics sfml-system sfml-window)
target_link_libraries(src_lib PRIVATE box2d)
target_link_libraries(src_lib PRIVATE clip)
