add_library(ui_lib)
target_sources(ui_lib PRIVATE
    "${INCLUDE_DIR}/UI/create_panel.h"
    "${INCLUDE_DIR}/UI/edit_window.h"
    "${INCLUDE_DIR}/UI/toolbox.h"
)
target_sources(ui_lib PRIVATE
    create_panel.cpp
    edit_window.cpp
    toolbox.cpp
)
target_include_directories(ui_lib PRIVATE "${INCLUDE_DIR}")
target_include_directories(ui_lib PUBLIC "${CMAKE_SOURCE_DIR}/sfml/include/")
target_include_directories(ui_lib PUBLIC "${CMAKE_SOURCE_DIR}/box2d/include/")
