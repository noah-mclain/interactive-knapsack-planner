cmake_minimum_required(VERSION 3.12)
project(interactive-knapsack-planner VERSION 0.1.0 LANGUAGES CXX)

# Set default build type to Release
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type" FORCE)
endif()

# SFML 3.0 requires C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Configure for different platforms
if(WIN32)
  # Windows-specific configurations
  add_definitions(-D_CRT_SECURE_NO_WARNINGS)
  set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
  # Set Windows GLSL version
  set(GLSL_VERSION "#version 130")
elseif(APPLE)
  # macOS-specific configurations
  set(CMAKE_MACOSX_RPATH ON)
  
  # Add Homebrew specific paths for macOS
  include_directories("/opt/homebrew/include")
  link_directories("/opt/homebrew/lib")
  
  # Set macOS GLSL version
  set(GLSL_VERSION "#version 120")
elseif(UNIX)
  # Linux-specific configurations
  add_compile_options(-Wall -Wextra)
  # Set Linux GLSL version
  set(GLSL_VERSION "#version 130")
endif()

# Configure the header file
configure_file(
  "${CMAKE_SOURCE_DIR}/backend/config.h.in"
  "${CMAKE_BINARY_DIR}/generated/config.h"
)

# Include the generated header directory
include_directories("${CMAKE_BINARY_DIR}/generated")

# Output directories - use platform-specific conventions
if(WIN32)
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
else()
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
endif()

# Print the GLSL version we're using
message(STATUS "Using GLSL version: ${GLSL_VERSION}")

# Find SFML package
find_package(SFML 3.0.1 COMPONENTS Graphics Window Audio Network REQUIRED)

# Find OpenGL
find_package(OpenGL REQUIRED)

# Add ImGui as a dependency (We'll build it from source)
set(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui)
if(NOT EXISTS ${IMGUI_DIR})
  message(FATAL_ERROR "ImGui directory not found. Please download ImGui and place it in ${IMGUI_DIR}")
endif()

# Collect ImGui source files - use ImGui's OpenGL3 backend instead of custom SFML one
set(IMGUI_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)

# Create ImGui library
add_library(imgui STATIC ${IMGUI_SOURCES})
target_include_directories(imgui PUBLIC 
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
    ${SFML_INCLUDE_DIR}
    ${OPENGL_INCLUDE_DIR}
)
target_link_libraries(imgui PUBLIC 
    sfml-graphics 
    sfml-window
    ${OPENGL_LIBRARIES}
)

# Add the backend directory
add_subdirectory(backend)

# Print build configuration
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "Platform: ${CMAKE_SYSTEM_NAME}")
message(STATUS "SFML found: ${SFML_FOUND} (version ${SFML_VERSION})")
message(STATUS "C++ standard: C++${CMAKE_CXX_STANDARD}")
