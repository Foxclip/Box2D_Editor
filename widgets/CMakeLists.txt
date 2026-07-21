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
)

target_include_directories(widgets_lib PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_include_directories(widgets_lib PUBLIC "${CMAKE_SOURCE_DIR}")
target_include_directories(widgets_lib PUBLIC "${CMAKE_SOURCE_DIR}/SFML/include/")
target_include_directories(widgets_lib PUBLIC "${CMAKE_SOURCE_DIR}/logger/include/")
target_link_libraries(widgets_lib PUBLIC sfml-graphics sfml-system sfml-window)
target_link_libraries(widgets_lib PUBLIC clip)

add_subdirectory(tests)
