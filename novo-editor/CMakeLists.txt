cmake_minimum_required(VERSION 3.25 FATAL_ERROR)

set(EDITOR_PROJECT_NAME novo-editor)

project(${EDITOR_PROJECT_NAME})

add_executable(${EDITOR_PROJECT_NAME}
    src/main.cpp
)

target_link_libraries(${EDITOR_PROJECT_NAME} novo-core)

set_target_properties(${EDITOR_PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_custom_command(TARGET ${EDITOR_PROJECT_NAME} POST_BUILD 
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/${EDITOR_PROJECT_NAME}/res $<TARGET_FILE_DIR:${EDITOR_PROJECT_NAME}>/res
)