cmake_minimum_required(VERSION 3.15)
project(StealAndCookGraphical)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Fetch Raylib Graphic Library automatically
include(FetchContent)
FetchContent_Declare(
    raylib
    URL https://github.com/raysan5/raylib/archive/refs/tags/5.0.tar.gz
)
FetchContent_MakeAvailable(raylib)

add_executable(StealAndCook
    src/main.cpp
)

target_link_libraries(StealAndCook PRIVATE raylib)

if(WIN32)
    target_compile_definitions(StealAndCook PRIVATE WIN32_LEAN_AND_MEAN NOMINMAX)
endif()
