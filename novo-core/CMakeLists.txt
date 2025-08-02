cmake_minimum_required(VERSION 3.25 FATAL_ERROR)

set(CORE_PROJECT_NAME novo-core)

project(${CORE_PROJECT_NAME})

add_library(${CORE_PROJECT_NAME} STATIC 
    includes/novo-core/Window.hpp
    includes/novo-core/Shader.hpp
    includes/novo-core/VAO.hpp
    includes/novo-core/VBO.hpp

    precompile/novo-precompiles/DefaultShader.h
)


target_include_directories(${CORE_PROJECT_NAME} PUBLIC includes)

target_include_directories(${CORE_PROJECT_NAME} PUBLIC precompile)


add_subdirectory(../external/glfw ${CMAKE_CURRENT_BINARY_DIR}/glfw)

target_link_libraries(${CORE_PROJECT_NAME} PUBLIC glfw)

add_subdirectory(../external/glad ${CMAKE_CURRENT_BINARY_DIR}/glad)

target_link_libraries(${CORE_PROJECT_NAME} PUBLIC glad)

add_subdirectory(../external/glm ${CMAKE_CURRENT_BINARY_DIR}/glm)

target_link_libraries(${CORE_PROJECT_NAME} PUBLIC glm)


set(IMGUI_INCLUDES
    ../external/imgui/imgui.h
    ../external/imgui/backends/imgui_impl_glfw.h
    ../external/imgui/backends/imgui_impl_opengl3.h
)

set(IMGUI_SOURCES
    ../external/imgui/imgui.cpp
    ../external/imgui/backends/imgui_impl_glfw.cpp
    ../external/imgui/backends/imgui_impl_opengl3.cpp
    ../external/imgui/imgui_demo.cpp
    ../external/imgui/imgui_draw.cpp
    ../external/imgui/imgui_tables.cpp
    ../external/imgui/imgui_widgets.cpp
)

add_library(imgui STATIC ${IMGUI_INCLUDES} ${IMGUI_SOURCES})

target_include_directories(imgui PUBLIC ../external)
target_include_directories(imgui PUBLIC ../external/imgui)
target_link_libraries(imgui PRIVATE glad glfw)

target_link_libraries(${CORE_PROJECT_NAME} PUBLIC imgui)