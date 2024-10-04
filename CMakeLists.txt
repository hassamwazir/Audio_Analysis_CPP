cmake_minimum_required(VERSION 3.10)

# Define the project name and C++ standard
project(CalculatorApp)
set(CMAKE_CXX_STANDARD 17)

# Include header files
include_directories(include)

# Define the source files for the project
set(SOURCES
    src/main.cpp
    src/LoadFile.cpp
)

# Create the executable
add_executable(load_audio_file ${SOURCES})