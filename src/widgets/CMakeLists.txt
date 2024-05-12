add_library(widgets_lib)
target_sources(widgets_lib PRIVATE
    "${INCLUDE_DIR}/widgets/application.h"
    "${INCLUDE_DIR}/widgets/canvas_widget.h"
    "${INCLUDE_DIR}/widgets/checkbox_widget.h"
    "${INCLUDE_DIR}/widgets/container_widget.h"
    "${INCLUDE_DIR}/widgets/drawing.h"
    "${INCLUDE_DIR}/widgets/empty_widget.h"
    "${INCLUDE_DIR}/widgets/rectangle_widget.h"
    "${INCLUDE_DIR}/widgets/shape_widget.h"
    "${INCLUDE_DIR}/widgets/text_widget.h"
    "${INCLUDE_DIR}/widgets/textbox_widget.h"
    "${INCLUDE_DIR}/widgets/widget.h"
    "${INCLUDE_DIR}/widgets/widget_list.h"
    "${INCLUDE_DIR}/widgets/widget_render_queue.h"
    "${INCLUDE_DIR}/widgets/widget_transform.h"
    "${INCLUDE_DIR}/widgets/widget_unclipped_region.h"
    "${INCLUDE_DIR}/widgets/widgets.h"
    "${INCLUDE_DIR}/widgets/widgets_common.h"
    "${INCLUDE_DIR}/widgets/window_widget.h"
)
target_sources(widgets_lib PRIVATE
    application.cpp
    canvas_widget.cpp
    checkbox_widget.cpp
    container_widget.cpp
    drawing.cpp
    empty_widget.cpp
    rectangle_widget.cpp
    shape_widget.cpp
    text_widget.cpp
    textbox_widget.cpp
    widget.cpp
    widget_list.cpp
    widget_render_queue.cpp
    widget_transform.cpp
    widget_unclipped_region.cpp
    widgets_common.cpp
    window_widget.cpp
)
target_include_directories(widgets_lib PRIVATE "${INCLUDE_DIR}")
target_include_directories(widgets_lib PUBLIC "${CMAKE_SOURCE_DIR}")
target_include_directories(widgets_lib PUBLIC "${CMAKE_SOURCE_DIR}/sfml/include/")
target_link_libraries(widgets_lib PUBLIC sfml-window)
