add_library(widgets_lib)
target_sources(widgets_lib PRIVATE
    ../../include/widgets/checkbox_widget.h
    ../../include/widgets/container_widget.h
    ../../include/widgets/rectangle_widget.h
    ../../include/widgets/shape_widget.h
    ../../include/widgets/text_widget.h
    ../../include/widgets/textbox_widget.h
    ../../include/widgets/widget.h
    ../../include/widgets/widget_list.h
    ../../include/widgets/widget_transform.h
    ../../include/widgets/widget_unclipped_region.h
    ../../include/widgets/widgets.h
)
target_sources(widgets_lib PRIVATE
    checkbox_widget.cpp
    container_widget.cpp
    rectangle_widget.cpp
    shape_widget.cpp
    text_widget.cpp
    textbox_widget.cpp
    widget.cpp
    widget_list.cpp
    widget_transform.cpp
    widget_unclipped_region.cpp
)
target_include_directories(widgets_lib PRIVATE "${CMAKE_SOURCE_DIR}/include/widgets")
target_include_directories(widgets_lib PUBLIC "E:/Libs/SFML-2.6.0/include/")
target_include_directories(widgets_lib PUBLIC "${CMAKE_SOURCE_DIR}/box2d/include/")
target_link_libraries(widgets_lib PRIVATE box2d)
