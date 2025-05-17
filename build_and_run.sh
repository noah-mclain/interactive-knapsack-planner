#!/bin/bash

# Exit on error
set -e

# Function to clean build
clean_build() {
    echo "Cleaning build directory..."
    rm -rf build
    mkdir -p build
}

# Check if clean build is requested
if [[ "$1" == "--clean" ]]; then
    clean_build
fi

# Print commands before execution
set -x

# Detect OS
if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    OS="Linux"
elif [[ "$OSTYPE" == "darwin"* ]]; then
    OS="macOS"
elif [[ "$OSTYPE" == "msys" || "$OSTYPE" == "win32" || "$OSTYPE" == "cygwin" ]]; then
    OS="Windows"
    
    # Detect if running in MinGW/MSYS environment
    if [[ -n "$MSYSTEM" && "$MSYSTEM" == "MINGW"* ]]; then
        MINGW=true
    fi
else
    echo "Unsupported OS: $OSTYPE"
    exit 1
fi

echo "Detected OS: $OS"

# Create build directory if it doesn't exist
mkdir -p build

# Navigate to build directory
cd build

# Run cmake based on OS
if [[ "$OS" == "Windows" ]]; then
    if [[ "$MINGW" == "true" ]]; then
        # MinGW-specific build
        echo "Using MinGW build system..."
        cmake .. -G "MinGW Makefiles"
        mingw32-make
        
        # Run the executable
        echo "Running executable..."
        ./bin/knapsack_main.exe
    else
        # Standard Windows-specific build with Visual Studio
        cmake .. -G "Visual Studio 16 2019" -A x64
        cmake --build . --config Release
        
        # Run the executable
        echo "Running executable..."
        ./bin/Release/knapsack_main.exe
    fi
else
    # Linux/macOS build
    cmake ..
    make
    
    # Run the executable
    echo "Running executable..."
    ./bin/knapsack_main
fi

# Return to original directory
cd ..

echo "Build and execution completed successfully!" 