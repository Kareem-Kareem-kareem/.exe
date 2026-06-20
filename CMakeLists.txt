cmake_minimum_required(VERSION 3.15)
project(StealAndCookGame)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Source files
add_executable(StealAndCook
    src/main.cpp
)

if(WIN32)
    target_compile_definitions(StealAndCook PRIVATE
        WIN32_LEAN_AND_MEAN
        NOMINMAX
    )
endif()
