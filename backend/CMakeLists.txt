cmake_minimum_required(VERSION 3.12)

# Add knapsack library
file(GLOB KNAPSACK_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
add_library(knapsack ${KNAPSACK_SOURCES})

# Set C++ standard
target_compile_features(knapsack PUBLIC cxx_std_14)

# Platform-specific compiler flags
if(MSVC)
  # MSVC compiler flags (Windows)
  target_compile_options(knapsack PRIVATE /W4)
else()
  # GCC/Clang compiler flags (Linux, macOS)
  target_compile_options(knapsack PRIVATE -Wall -Wextra)
endif()

# Include directories - using generator expressions for better compatibility
target_include_directories(knapsack PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

# Create main executable
add_executable(knapsack_main
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
)

# Link the executable with the knapsack library
target_link_libraries(knapsack_main PRIVATE knapsack)

# Install targets (optional)
include(GNUInstallDirs)
install(TARGETS knapsack knapsack_main 
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
