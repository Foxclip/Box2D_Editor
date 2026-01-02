set(SIMULATION_INCLUDE_DIR "${INCLUDE_DIR}/simulation")

set(SIMULATION_HEADER_FILES
    "${SIMULATION_INCLUDE_DIR}/gameobject.h"
    "${SIMULATION_INCLUDE_DIR}/gameobject_transform.h"
    "${SIMULATION_INCLUDE_DIR}/joint.h"
    "${SIMULATION_INCLUDE_DIR}/objectlist.h"
    "${SIMULATION_INCLUDE_DIR}/polygon.h"
    "${SIMULATION_INCLUDE_DIR}/serializer.h"
    "${SIMULATION_INCLUDE_DIR}/shapes.h"
    "${SIMULATION_INCLUDE_DIR}/simulation.h"
)
set(SIMULATION_SOURCE_FILES
    "gameobject.cpp"
    "gameobject_transform.cpp"
    "joint.cpp"
    "objectlist.cpp"
    "polygon.cpp"
    "serializer.cpp"
    "shapes.cpp"
    "simulation.cpp"
)
add_library(simulation_lib ${SIMULATION_HEADER_FILES} ${SIMULATION_SOURCE_FILES})
source_group(TREE ${SIMULATION_INCLUDE_DIR} PREFIX "Header Files" FILES ${SIMULATION_HEADER_FILES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX "Source Files" FILES ${SIMULATION_SOURCE_FILES})

target_include_directories(simulation_lib PUBLIC "${INCLUDE_DIR}")
target_include_directories(simulation_lib PUBLIC "${CMAKE_SOURCE_DIR}")
target_include_directories(simulation_lib PUBLIC "${CMAKE_SOURCE_DIR}/sfml/include/")
target_include_directories(simulation_lib PUBLIC "${CMAKE_SOURCE_DIR}/box2d/include/")
target_include_directories(simulation_lib PUBLIC "${CMAKE_SOURCE_DIR}/logger/include/")
target_link_libraries(simulation_lib PUBLIC sfml-graphics)
target_link_libraries(simulation_lib PUBLIC box2d)
