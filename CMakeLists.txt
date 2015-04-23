cmake_minimum_required(VERSION 2.6)

project(FLUID_TEST)

add_executable(Example
example.cpp
)

# this assumes libfluidsynth-dev is installed
target_link_libraries(Example fluidsynth)
