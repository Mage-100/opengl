cmake_minimum_required(VERSION 4.0)
project(_opengl)

set(CMAKE_CXX_STANDARD 17)

find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)

add_library(common_headers INTERFACE)
target_include_directories(common_headers INTERFACE
    "${CMAKE_SOURCE_DIR}/include"
)

add_subdirectory(src/glass-ui)
