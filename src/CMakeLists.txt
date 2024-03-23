set(INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include")

add_subdirectory(UI)
add_subdirectory(widgets)

add_library(src_lib)
target_sources(src_lib PRIVATE
    "${INCLUDE_DIR}/application.h"
    "${INCLUDE_DIR}/compvector.h"
    "${INCLUDE_DIR}/gameobject.h"
    "${INCLUDE_DIR}/global.h"
    "${INCLUDE_DIR}/history.h"
    "${INCLUDE_DIR}/logger.h"
    "${INCLUDE_DIR}/main.h"
    "${INCLUDE_DIR}/objectlist.h"
    "${INCLUDE_DIR}/polygon.h"
    "${INCLUDE_DIR}/scenes.h"
    "${INCLUDE_DIR}/searchindex.h"
    "${INCLUDE_DIR}/serializer.h"
    "${INCLUDE_DIR}/tools.h"
    "${INCLUDE_DIR}/utils.h"
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
target_include_directories(src_lib PUBLIC "${INCLUDE_DIR}")
target_include_directories(src_lib PUBLIC "${CMAKE_SOURCE_DIR}/sfml/include/")
target_include_directories(src_lib PUBLIC "${CMAKE_SOURCE_DIR}/box2d/include/")
target_link_libraries(src_lib PUBLIC sfml-graphics sfml-system sfml-window)
target_link_libraries(src_lib PRIVATE box2d)
target_link_libraries(src_lib PRIVATE clip)
target_link_libraries(src_lib PRIVATE ui_lib)
target_link_libraries(src_lib PRIVATE widgets_lib)
