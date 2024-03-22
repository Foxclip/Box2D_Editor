add_library(ui_lib)
target_sources(ui_lib PRIVATE
    ../../include/UI/create_panel.h
    ../../include/UI/edit_window.h
    ../../include/UI/toolbox.h
)
target_sources(ui_lib PRIVATE
    create_panel.cpp
    edit_window.cpp
    toolbox.cpp
)
target_include_directories(ui_lib PRIVATE "${CMAKE_SOURCE_DIR}/include/UI")
target_include_directories(ui_lib PUBLIC "${CMAKE_SOURCE_DIR}/sfml/include/")
target_include_directories(ui_lib PUBLIC "${CMAKE_SOURCE_DIR}/box2d/include/")
