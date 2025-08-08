# Require a minimum version of CMake to be used for this project
cmake_minimum_required(VERSION 3.24)

# Define the project name and set the language to C
project(spaceXplorerV2 C)

# Specify that we're using C11 standard
set(CMAKE_C_STANDARD 11)

# Define the executable target and its source files
add_executable(spaceXplorerV2 main.c game.c)
