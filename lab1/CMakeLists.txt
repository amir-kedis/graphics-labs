cmake_minimum_required(VERSION 3.10.0)
project(gfx_lab1 VERSION 0.1.0 LANGUAGES C CXX)

add_subdirectory(vendor/glfw)

include_directories(
  vendor/glfw/include
  vendor/glad/include
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PROJECT_SOURCE_DIR}/bin)

add_executable(
  gfx_lab1 
  main.cpp
  vendor/glad/src/gl.c
)

target_link_libraries(gfx_lab1 glfw)

