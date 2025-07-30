cmake_minimum_required(VERSION 3.25 FATAL_ERROR)

set(CORE_PROJECT_NAME novo-core)

project(${CORE_PROJECT_NAME})

add_library(${CORE_PROJECT_NAME} STATIC 
    includes/novo-core/Window.hpp
)


target_include_directories(${CORE_PROJECT_NAME} PUBLIC includes)


add_subdirectory(../external/glfw ${CMAKE_CURRENT_BINARY_DIR}/glfw)

target_link_libraries(${CORE_PROJECT_NAME} PUBLIC glfw)

add_subdirectory(../external/glad ${CMAKE_CURRENT_BINARY_DIR}/glad)

target_link_libraries(${CORE_PROJECT_NAME} PUBLIC glad)

add_subdirectory(../external/glm ${CMAKE_CURRENT_BINARY_DIR}/glm)

target_link_libraries(${CORE_PROJECT_NAME} PUBLIC glm)