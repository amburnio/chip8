cmake_minimum_required(VERSION 4.0)

project(chip8 C)

set(CMAKE_C_STANDARD 11)

add_executable(chip8 src/main.c src/chip8.c src/chip8.h)