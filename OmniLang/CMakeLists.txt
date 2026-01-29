cmake_minimum_required(VERSION 3.10)
project(OmniLang)

set(CMAKE_CXX_STANDARD 17)

# Include directories
include_directories(src)

# Source files
set(SOURCES
    src/main.cpp
    src/Lexer.cpp
    src/Parser.cpp
)

# Executable
add_executable(omni ${SOURCES})
