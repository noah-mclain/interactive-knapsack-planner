cmake_minimum_required(VERSION 3.12)

# Add knapsack library
file(GLOB KNAPSACK_SOURCES "src/*.cpp")
add_library(knapsack ${KNAPSACK_SOURCES})

# Set C++ standard
target_compile_features(knapsack PUBLIC cxx_std_14)

# Include directories
target_include_directories(knapsack PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Create main executable
add_executable(knapsack_main
    main.cpp
)

# Link the executable with the knapsack library
target_link_libraries(knapsack_main PRIVATE knapsack)
