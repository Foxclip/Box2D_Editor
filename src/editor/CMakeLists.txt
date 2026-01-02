set(EDITOR_INCLUDE_DIR "${INCLUDE_DIR}/editor")

set(EDITOR_HEADER_FILES
    "${EDITOR_INCLUDE_DIR}/editor.h"
    "${EDITOR_INCLUDE_DIR}/scenes.h"
    "${EDITOR_INCLUDE_DIR}/tools.h"
    "${EDITOR_INCLUDE_DIR}/UI/create_panel.h"
    "${EDITOR_INCLUDE_DIR}/UI/edit_window.h"
    "${EDITOR_INCLUDE_DIR}/UI/menu.h"
    "${EDITOR_INCLUDE_DIR}/UI/outliner.h"
    "${EDITOR_INCLUDE_DIR}/UI/toolbox.h"
)
set(EDITOR_SOURCE_FILES
    "editor.cpp"
    "tools.cpp"
    "UI/create_panel.cpp"
    "UI/edit_window.cpp"
    "UI/menu.cpp"
    "UI/outliner.cpp"
    "UI/toolbox.cpp"
)
add_library(editor ${EDITOR_HEADER_FILES} ${EDITOR_SOURCE_FILES})
source_group(TREE ${EDITOR_INCLUDE_DIR} PREFIX "Header Files" FILES ${EDITOR_HEADER_FILES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX "Source Files" FILES ${EDITOR_SOURCE_FILES})

target_include_directories(editor PUBLIC "${INCLUDE_DIR}")
target_include_directories(editor PUBLIC "${CMAKE_SOURCE_DIR}")
target_include_directories(editor PUBLIC "${CMAKE_SOURCE_DIR}/sfml/include/")
target_include_directories(editor PUBLIC "${CMAKE_SOURCE_DIR}/box2d/include/")
target_link_libraries(editor PUBLIC common_lib)
target_link_libraries(editor PUBLIC simulation_lib)
target_link_libraries(editor PUBLIC widgets_lib)
target_link_libraries(editor PUBLIC logger)
