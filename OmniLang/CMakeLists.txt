cmake_minimum_required(VERSION 3.10)
project(OmniLang)

set(CMAKE_CXX_STANDARD 17)

# Source files
set(SOURCES
    src/main.cpp
    src/Lexer.cpp
    src/Parser.cpp
)

# Executable
add_executable(omni ${SOURCES})

# Keep <windows.h> from defining min/max macros that break std::min/std::max
if(WIN32)
    target_compile_definitions(omni PRIVATE NOMINMAX)
endif()
