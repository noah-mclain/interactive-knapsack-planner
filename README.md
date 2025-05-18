# Interactive Knapsack Planner

An interactive knapsack problem solver with a graphical user interface built using SFML 3.0 and ImGui.

## Prerequisites

### Install SFML 3.0

#### macOS

```bash
brew install sfml
```

#### Ubuntu/Debian

```bash
# SFML 3.0 may not be in official repositories yet, you might need to build from source:
# https://github.com/SFML/SFML/wiki/SFML-3:-Preparing-and-building

# For older SFML versions:
# sudo apt-get install libsfml-dev
```

#### Windows with MSYS2

```bash
pacman -S mingw-w64-x86_64-sfml
```

### ImGui

ImGui will be automatically downloaded when you run the build script.

## Building and Running

1. Clone the repository:

```bash
git clone https://github.com/yourusername/interactive-knapsack-planner.git
cd interactive-knapsack-planner
```

2. Run the build script:

```bash
./build_and_run.sh
```

For a clean build:

```bash
./build_and_run.sh --clean
```

## Project Structure

- `backend/`: Core knapsack algorithm implementation
- `external/`: External dependencies (ImGui)
- `scripts/`: Utility scripts for building and setup

## License

[Your license information]

## Implemented Algorithms

- **Greedy Approach**: Sort items by profit-to-weight ratio (suboptimal but fast)
- **Brute Force Approach**: Examine all possible combinations (optimal but exponential complexity)
- **Dynamic Programming Approach**: Efficient solution with optimal results (polynomial complexity)

## Quick Start

The easiest way to build and run the project is to use the provided build script:

```bash
# Run the Python script (works on Windows, macOS, and Linux)
python3 build_run.py
```

This script will:

1. Detect your operating system
2. Check if CMake is installed
3. Create and clean the build directory
4. Configure the project with CMake
5. Build the project using multiple CPU cores
6. Run the executable

## Building the Project Manually

### Prerequisites

- CMake 3.12 or higher
- C++ compiler with C++14 support

### Build Instructions

1. Create a build directory:

   ```bash
   mkdir build && cd build
   ```

2. Configure CMake:

   ```bash
   cmake ..
   ```

3. Build the project:

   ```bash
   cmake --build .
   ```

### Running the Application

After building, you can run the knapsack solver:

```bash
./bin/knapsack_main
```

## Project Structure

- `backend/` - Contains the C++ implementation of knapsack algorithms
  - `include/` - Header files
    - `knapsack/` - Knapsack algorithm interfaces
      - `greedy_knapsack.hpp` - Greedy algorithm interface
      - `dp_knapsack.hpp` - Dynamic programming algorithm interface
      - `brute_force_knapsack.hpp` - Brute force algorithm interface
      - `knapsack_solver.hpp` - General solver interface
  - `src/` - Implementation files
    - `greedy_knapsack.cpp` - Greedy algorithm implementation
    - `dp_knapsack.cpp` - Dynamic programming algorithm implementation
    - `brute_force_knapsack.cpp` - Brute force algorithm implementation
    - `knapsack_solver.cpp` - General solver implementation
  - `main.cpp` - Main application entry point

## Performance

The implemented algorithms provide performance metrics:

- Execution time
- Time complexity
- Memory usage
- Space complexity

## Note About SFML 3.0

This project requires SFML 3.0 which has several API changes compared to SFML 2.x:

- System module is no longer a separate component
- Some namespace and function changes

If you encounter build issues, make sure you have SFML 3.0+ installed.
