#!/usr/bin/env bash

# Exit on error
set -e

# Function to clean build
clean_build() {
    echo "[INFO] Cleaning build directory..."
    rm -rf build
    mkdir -p build
}

# Check if clean build is requested
if [[ "$1" == "--clean" ]]; then
    clean_build
fi

# Detect OS using uname
OS_TYPE=$(uname -s)
WINDOWS=false; LINUX=false; MAC=false; MINGW=false; MINGW_FLAVOR=""

case "$OS_TYPE" in
    Linux*)     
        LINUX=true
        echo "[INFO] Detected Linux environment"
        ;;
    Darwin*)    
        MAC=true
        echo "[INFO] Detected macOS environment"
        ;;
    MINGW*|MSYS*|CYGWIN*)
        WINDOWS=true
        
        # Detect MinGW environment
        if [[ -n "$MSYSTEM" ]]; then
            MINGW=true
            echo "[INFO] Detected MinGW environment: $MSYSTEM"
            
            # Detect specific MinGW flavor
            if [[ "$MSYSTEM" == "UCRT64" ]]; then
                MINGW_FLAVOR="ucrt64"
                echo "[INFO] Using UCRT64 toolchain"
            elif [[ "$MSYSTEM" == "MINGW64" ]]; then
                MINGW_FLAVOR="mingw64"
                echo "[INFO] Using MINGW64 toolchain"
            elif [[ "$MSYSTEM" == "MINGW32" ]]; then
                MINGW_FLAVOR="mingw32"
                echo "[INFO] Using MINGW32 toolchain"
            else
                MINGW_FLAVOR="mingw"
                echo "[INFO] Using generic MinGW toolchain"
            fi
        else
            echo "[INFO] Detected Windows environment without MinGW"
        fi
        ;;
    *)
        echo "[ERROR] Unsupported OS: $OS_TYPE"
        exit 1
        ;;
esac

echo "[INFO] Building for $OS_TYPE..."

# Create build directory if it doesn't exist
mkdir -p build

# Navigate to build directory
cd build

# Print commands before execution
set -x

# Run cmake based on OS
if $WINDOWS; then
    if $MINGW; then
        # MinGW-specific build
        echo "[INFO] Using MinGW build system..."
        
        # Add verbose output for better debugging
        cmake .. -G "MinGW Makefiles" -DCMAKE_VERBOSE_MAKEFILE=ON
        
        # Try to build with detailed output
        set +e  # Don't exit on error for this command
        mingw32-make VERBOSE=1
        BUILD_RESULT=$?
        set -e  # Restore exit on error
        
        if [ $BUILD_RESULT -ne 0 ]; then
            echo "[ERROR] Build failed with exit code $BUILD_RESULT"
            echo "[ERROR] Please check the error messages above for more details."
            exit $BUILD_RESULT
        fi
        
        # Run the executable if build succeeded
        echo "[INFO] Running executable..."
        if [ -f "./bin/knapsack_main.exe" ]; then
            ./bin/knapsack_main.exe
        else
            echo "[ERROR] Executable not found at ./bin/knapsack_main.exe"
            exit 1
        fi
    else
        # Standard Windows-specific build with Visual Studio
        cmake .. -G "Visual Studio 16 2019" -A x64
        cmake --build . --config Release
        
        # Run the executable
        echo "[INFO] Running executable..."
        if [ -f "./bin/Release/knapsack_main.exe" ]; then
            ./bin/Release/knapsack_main.exe
        else
            echo "[ERROR] Executable not found at ./bin/Release/knapsack_main.exe"
            exit 1
        fi
    fi
elif $LINUX; then
    # Linux build
    cmake .. -DCMAKE_VERBOSE_MAKEFILE=ON
    make VERBOSE=1
    
    # Run the executable
    echo "[INFO] Running executable..."
    if [ -f "./bin/knapsack_main" ]; then
        ./bin/knapsack_main
    else
        echo "[ERROR] Executable not found at ./bin/knapsack_main"
        exit 1
    fi
elif $MAC; then
    # macOS build
    cmake .. -DCMAKE_VERBOSE_MAKEFILE=ON
    make VERBOSE=1
    
    # Run the executable
    echo "[INFO] Running executable..."
    if [ -f "./bin/knapsack_main" ]; then
        ./bin/knapsack_main
    else
        echo "[ERROR] Executable not found at ./bin/knapsack_main"
        exit 1
    fi
fi

# Return to original directory
cd ..

echo "[SUCCESS] Build and execution completed successfully! ✅"