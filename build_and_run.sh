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
    if [[ -n "$MSYSTEM" ]]; then
        MINGW=true
        echo "Detected MinGW environment: $MSYSTEM"
        
        # Detect UCRT64 specifically
        if [[ "$MSYSTEM" == "UCRT64" || "$MSYSTEM" == "MINGW64" ]]; then
            MINGW_FLAVOR="ucrt64"
            echo "Using UCRT64/MINGW64 toolchain"
        elif [[ "$MSYSTEM" == "MINGW32" ]]; then
            MINGW_FLAVOR="mingw32"
            echo "Using MINGW32 toolchain"
        else
            MINGW_FLAVOR="mingw"
            echo "Using generic MinGW toolchain"
        fi
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
        
        # Add verbose output for better debugging
        cmake .. -G "MinGW Makefiles" -DCMAKE_VERBOSE_MAKEFILE=ON
        
        # Try to build with detailed output
        # Use specific make command based on MinGW flavor
        set +e  # Don't exit on error for this command
        
        if [[ "$MINGW_FLAVOR" == "ucrt64" ]]; then
            # For UCRT64, might need specific path or environment
            mingw32-make VERBOSE=1
        else
            # Default MinGW make command
            mingw32-make VERBOSE=1
        fi
        
        BUILD_RESULT=$?
        set -e  # Restore exit on error
        
        if [ $BUILD_RESULT -ne 0 ]; then
            echo "Build failed with exit code $BUILD_RESULT"
            echo "Please check the error messages above for more details."
            exit $BUILD_RESULT
        fi
        
        # Run the executable if build succeeded
        echo "Running executable..."
        if [ -f "./bin/knapsack_main.exe" ]; then
            ./bin/knapsack_main.exe
        else
            echo "Error: Executable not found at ./bin/knapsack_main.exe"
            exit 1
        fi
    else
        # Standard Windows-specific build with Visual Studio
        cmake .. -G "Visual Studio 16 2019" -A x64
        cmake --build . --config Release
        
        # Run the executable
        echo "Running executable..."
        if [ -f "./bin/Release/knapsack_main.exe" ]; then
            ./bin/Release/knapsack_main.exe
        else
            echo "Error: Executable not found at ./bin/Release/knapsack_main.exe"
            exit 1
        fi
    fi
else
    # Linux/macOS build
    cmake .. -DCMAKE_VERBOSE_MAKEFILE=ON
    make VERBOSE=1
    
    # Run the executable
    echo "Running executable..."
    if [ -f "./bin/knapsack_main" ]; then
        ./bin/knapsack_main
    else
        echo "Error: Executable not found at ./bin/knapsack_main"
        exit 1
    fi
fi

# Return to original directory
cd ..

echo "Build and execution completed successfully!"