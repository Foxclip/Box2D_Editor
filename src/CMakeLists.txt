add_subdirectory(UI)
add_subdirectory(widgets)

add_library(src_lib)
target_sources(src_lib PRIVATE
    ../include/application.h
    ../include/compvector.h
    ../include/gameobject.h
    ../include/global.h
    ../include/history.h
    ../include/logger.h
    ../include/main.h
    ../include/objectlist.h
    ../include/polygon.h
    ../include/scenes.h
    ../include/searchindex.h
    ../include/serializer.h
    ../include/tools.h
    ../include/utils.h
)
target_sources(src_lib PRIVATE
    application.cpp
    compvector.cpp
    gameobject.cpp
    global.cpp
    history.cpp
    logger.cpp
    main.cpp
    objectlist.cpp
    polygon.cpp
    searchindex.cpp
    serializer.cpp
    tools.cpp
    utils.cpp
)
target_include_directories(src_lib PRIVATE "${CMAKE_SOURCE_DIR}/include")
target_include_directories(src_lib PUBLIC "${CMAKE_SOURCE_DIR}/sfml/include/")
target_include_directories(src_lib PUBLIC "${CMAKE_SOURCE_DIR}/box2d/include/")
target_link_libraries(src_lib PUBLIC sfml-graphics sfml-system sfml-window)
target_link_libraries(src_lib PRIVATE box2d)
target_link_libraries(src_lib PRIVATE ui_lib)
target_link_libraries(src_lib PRIVATE widgets_lib)
