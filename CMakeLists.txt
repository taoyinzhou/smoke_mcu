cmake_minimum_required(VERSION 3.10)

# Set the project name
project (smoke_sensor)

set(CMAKE_C_STANDARD 11)

# Add an executable
add_executable(smoke_sensor 
    src/main.c
    src/analyze.c
    src/data.c
    src/command.c
    src/io.c
    src/test.c
    src/utils.c)
target_include_directories(smoke_sensor PRIVATE include)
