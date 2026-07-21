cmake_minimum_required(VERSION 3.14)
project(widgets LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

add_library(widgets_lib)

target_sources(widgets_lib PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/include/widgets/application.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/widgets/button_widget.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/widgets/canvas_widget.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/widgets/checkbox_widget.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/widgets/container_widget.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/widgets/drawing.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/widgets/dropdown_widget.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/widgets/empty_widget.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/widgets/font.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/widgets/polygon_widget.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/widgets/rectangle_widget.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/widgets/render_texture.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/widgets/scroll_area_widget.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/widgets/shape_widget.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/widgets/text_widget.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/widgets/textbox_widget.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/widgets/tree_view_entry.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/widgets/tree_view_entry_widget.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/widgets/tree_view_widget.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/widgets/widget.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/widgets/widget_link.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/widgets/widget_list.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/widgets/widget_parent_chain.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/widgets/widget_render_queue.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/widgets/widget_transform.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/widgets/widget_unclipped_region.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/widgets/widget_update_queue.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/widgets/widgets.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/widgets/widgets_common.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/widgets/window_widget.h"
)
target_sources(widgets_lib PRIVATE
    src/application.cpp
    src/button_widget.cpp
    src/canvas_widget.cpp
    src/checkbox_widget.cpp
    src/container_widget.cpp
    src/drawing.cpp
    src/dropdown_widget.cpp
    src/empty_widget.cpp
    src/font.cpp
    src/polygon_widget.cpp
    src/rectangle_widget.cpp
    src/render_texture.cpp
    src/scroll_area_widget.cpp
    src/shape_widget.cpp
    src/text_widget.cpp
    src/textbox_widget.cpp
    src/tree_view_entry.cpp
    src/tree_view_entry_widget.cpp
    src/tree_view_widget.cpp
    src/widget.cpp
    src/widget_link.cpp
    src/widget_list.cpp
    src/widget_parent_chain.cpp
    src/widget_render_queue.cpp
    src/widget_transform.cpp
    src/widget_unclipped_region.cpp
    src/widget_update_queue.cpp
    src/widgets_common.cpp
    src/window_widget.cpp
    src/data_pointer_common.cpp
)

target_include_directories(widgets_lib PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

# Handle dependencies: check if targets already exist (parent project) or fetch local submodules
if(NOT TARGET sfml-graphics)
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/third_party/SFML/CMakeLists.txt")
        set(SFML_BUILD_AUDIO OFF CACHE BOOL "" FORCE)
        set(SFML_BUILD_DOC OFF CACHE BOOL "" FORCE)
        set(SFML_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
        set(SFML_BUILD_GRAPHICS ON CACHE BOOL "" FORCE)
        set(SFML_BUILD_NETWORK OFF CACHE BOOL "" FORCE)
        set(SFML_BUILD_TEST_SUITE OFF CACHE BOOL "" FORCE)
        set(SFML_BUILD_WINDOW ON CACHE BOOL "" FORCE)
        set(SFML_GENERATE_PDB OFF CACHE BOOL "" FORCE)
        set(SFML_INSTALL_PKGCONFIG_FILES OFF CACHE BOOL "" FORCE)
        set(SFML_OPENGL_ES OFF CACHE BOOL "" FORCE)
        set(SFML_USE_STATIC_STD_LIBS OFF CACHE BOOL "" FORCE)
        set(SFML_USE_SYSTEM_DEPS OFF CACHE BOOL "" FORCE)
        add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/third_party/SFML")
    else()
        message(FATAL_ERROR "sfml-graphics target not found and third_party/SFML not available")
    endif()
endif()

if(NOT TARGET clip)
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/third_party/clip/CMakeLists.txt")
        set(CLIP_ENABLE_IMAGE OFF CACHE BOOL "" FORCE)
        set(CLIP_EXAMPLES OFF CACHE BOOL "" FORCE)
        set(CLIP_SUPPORT_WINXP OFF CACHE BOOL "" FORCE)
        set(CLIP_TESTS OFF CACHE BOOL "" FORCE)
        add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/third_party/clip")
    else()
        message(FATAL_ERROR "clip target not found and third_party/clip not available")
    endif()
endif()

if(NOT TARGET logger)
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/third_party/logger/CMakeLists.txt")
        add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/third_party/logger")
    else()
        message(FATAL_ERROR "logger target not found and third_party/logger not available")
    endif()
endif()

target_link_libraries(widgets_lib PUBLIC sfml-graphics sfml-system sfml-window)
target_link_libraries(widgets_lib PUBLIC clip)
target_link_libraries(widgets_lib PUBLIC logger)

# Clip include directories (clip doesn't set PUBLIC include dirs, so consumers need explicit path)
if(EXISTS "${CMAKE_SOURCE_DIR}/clip/")
    target_include_directories(widgets_lib PUBLIC "${CMAKE_SOURCE_DIR}")
elseif(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/third_party/clip/")
    target_include_directories(widgets_lib PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/third_party")
endif()

# Logger include directories (logger uses PRIVATE, so consumers need explicit path)
if(EXISTS "${CMAKE_SOURCE_DIR}/logger/include/")
    target_include_directories(widgets_lib PUBLIC "${CMAKE_SOURCE_DIR}/logger/include/")
elseif(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/third_party/logger/include/")
    target_include_directories(widgets_lib PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/third_party/logger/include/")
endif()

# Tests only in embedded builds where test_lib target exists
if(TARGET test_lib)
    add_subdirectory(tests)
endif()
