cmake_minimum_required(VERSION 3.20)
project(fasterizer)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(fasterizer
    src/main.cpp
    src/tga.cpp
)

target_include_directories(fasterizer PRIVATE src)

target_compile_options(fasterizer PRIVATE
    $<$<CONFIG:Debug>:-g -fsanitize=address,undefined>
    $<$<CONFIG:Release>:-O3 -march=native>
)
target_link_options(fasterizer PRIVATE
    $<$<CONFIG:Debug>:-fsanitize=address,undefined>
)