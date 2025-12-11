cmake_minimum_required(VERSION 4.0)
project(chip8 C)

set(CMAKE_C_STANDARD 11)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

add_subdirectory(vendored/SDL EXCLUDE_FROM_ALL)

add_executable(chip8 src/main.c src/chip8.c src/chip8.h)

target_link_libraries(chip8 PRIVATE SDL3::SDL3)