cmake_minimum_required(VERSION 3.25 FATAL_ERROR)

set(CORE_PROJECT_NAME novo-core)

project(${CORE_PROJECT_NAME})

add_library(${CORE_PROJECT_NAME} STATIC 
    includes/novo-core/test.hpp
    src/novo-core/test.cpp
)


target_include_directories(${CORE_PROJECT_NAME} PUBLIC includes)
