#!/usr/bin/env bash

# Exit on error
set -e

# Print colorized info messages
print_info() {
    echo -e "\033[1;34m[INFO]\033[0m $1"
}

print_warning() {
    echo -e "\033[1;33m[WARNING]\033[0m $1"
}

print_error() {
    echo -e "\033[1;31m[ERROR]\033[0m $1"
}

print_success() {
    echo -e "\033[1;32m[SUCCESS]\033[0m $1"
}

print_platform_info() {
    print_info "============= PLATFORM INFORMATION ============="
    print_info "OS Type:     $OS_TYPE"
    print_info "OS Arch:     $OS_ARCH"
    print_info "Build Type:  ${CMAKE_BUILD_TYPE:-Release}"
    
    if $WINDOWS; then
        print_info "Platform:    Windows"
        if $MINGW; then
            print_info "Environment: MinGW ($MINGW_FLAVOR)"
        else
            print_info "Environment: Native Windows"
        fi
        print_info "SFML Path:   E:/Libraries/SFML-3.0.0"
        print_info "SFML Version: 3.0.0"
        print_info "GLSL Version: #version 130"
    elif $MAC; then
        print_info "Platform:    macOS"
        print_info "SFML Version: 3.0.1"
        print_info "GLSL Version: #version 120"
    elif $LINUX; then
        print_info "Platform:    Linux"
        print_info "SFML Version: 3.0.0"
        print_info "GLSL Version: #version 130"
    fi
    print_info "=============================================="
}

# Function to clean build
clean_build() {
    print_info "Cleaning build directory..."
    rm -rf build
    mkdir -p build
}

# Function to manually copy Windows DLLs
copy_windows_dlls() {
    local target_dir="$1"
    local sfml_dir="E:/Libraries/SFML-3.0.0"
    local config="$2"
    
    # Determine suffix based on build configuration
    local dll_suffix=""
    if [[ "$config" == "Debug" ]]; then
        dll_suffix="-d"
    fi
    
    print_info "Copying SFML DLLs to $target_dir..."
    
    # Copy required SFML DLLs
    cp -v "${sfml_dir}/bin/sfml-graphics${dll_suffix}-3.dll" "$target_dir"
    cp -v "${sfml_dir}/bin/sfml-window${dll_suffix}-3.dll" "$target_dir"
    cp -v "${sfml_dir}/bin/sfml-system${dll_suffix}-3.dll" "$target_dir"
    cp -v "${sfml_dir}/bin/sfml-audio${dll_suffix}-3.dll" "$target_dir"
    cp -v "${sfml_dir}/bin/sfml-network${dll_suffix}-3.dll" "$target_dir"
    
    # Copy dependencies (if they exist)
    if [[ -f "${sfml_dir}/bin/openal32.dll" ]]; then
        cp -v "${sfml_dir}/bin/openal32.dll" "$target_dir"
    fi
}

# Check if clean build is requested
if [[ "$1" == "--clean" ]]; then
    clean_build
fi

# Setup dependencies (SFML should be installed by the user, ImGui will be downloaded)
print_info "Setting up dependencies..."
./scripts/setup_dependencies.sh

# Detect OS using uname
OS_TYPE=$(uname -s)
OS_ARCH=$(uname -m)
WINDOWS=false; LINUX=false; MAC=false; MINGW=false; MINGW_FLAVOR=""
BUILD_CONFIG="Release"

case "$OS_TYPE" in
    Linux*)     
        LINUX=true
        print_info "Detected Linux environment ($OS_ARCH)"
        ;;
    Darwin*)    
        MAC=true
        print_info "Detected macOS environment ($OS_ARCH)"
        ;;
    MINGW*|MSYS*|CYGWIN*)
        WINDOWS=true
        
        # Detect MinGW environment
        if [[ -n "$MSYSTEM" ]]; then
            MINGW=true
            print_info "Detected MinGW environment: $MSYSTEM ($OS_ARCH)"
            
            # Detect specific MinGW flavor
            if [[ "$MSYSTEM" == "UCRT64" ]]; then
                MINGW_FLAVOR="ucrt64"
                print_info "Using UCRT64 toolchain"
            elif [[ "$MSYSTEM" == "MINGW64" ]]; then
                MINGW_FLAVOR="mingw64"
                print_info "Using MINGW64 toolchain"
            elif [[ "$MSYSTEM" == "MINGW32" ]]; then
                MINGW_FLAVOR="mingw32"
                print_info "Using MINGW32 toolchain"
            else
                MINGW_FLAVOR="mingw"
                print_info "Using generic MinGW toolchain"
            fi
        else
            print_info "Detected Windows environment without MinGW ($OS_ARCH)"
        fi
        ;;
    *)
        print_error "Unsupported OS: $OS_TYPE"
        exit 1
        ;;
esac

# Check for debug build
if [[ "$1" == "--debug" || "$2" == "--debug" ]]; then
    BUILD_CONFIG="Debug"
    print_info "Building in Debug mode"
fi

# Display platform information
print_platform_info

print_info "Building for $OS_TYPE ($OS_ARCH) in $BUILD_CONFIG mode..."

# Create build directory if it doesn't exist
mkdir -p build

# Navigate to build directory
cd build

# Build options
CMAKE_OPTIONS=""

# Add OS-specific cmake options
if $MAC; then
    # Add arch flag for Apple Silicon if needed
    if [[ "$OS_ARCH" == "arm64" ]]; then
        CMAKE_OPTIONS="$CMAKE_OPTIONS -DCMAKE_OSX_ARCHITECTURES=arm64"
    fi
elif $WINDOWS; then
    # Set Windows SFML path
    CMAKE_OPTIONS="$CMAKE_OPTIONS -DSFML_DIR=E:/Libraries/SFML-3.0.0/lib/cmake/SFML"
    # Set build type
    CMAKE_OPTIONS="$CMAKE_OPTIONS -DCMAKE_BUILD_TYPE=$BUILD_CONFIG"
fi

# Run cmake based on OS with verbose output for debugging
print_info "Configuring project with CMake..."
if $WINDOWS; then
    if $MINGW; then
        # MinGW-specific build
        print_info "Using MinGW build system..."
        
        # Add verbose output for better debugging
        cmake .. -G "MinGW Makefiles" -DCMAKE_VERBOSE_MAKEFILE=ON $CMAKE_OPTIONS
        
        # Try to build with detailed output
        set +e  # Don't exit on error for this command
        print_info "Building project..."
        mingw32-make VERBOSE=1
        BUILD_RESULT=$?
        set -e  # Restore exit on error
        
        if [ $BUILD_RESULT -ne 0 ]; then
            print_error "Build failed with exit code $BUILD_RESULT"
            print_error "Please check the error messages above for more details."
            exit $BUILD_RESULT
        fi
        
        # Manually copy DLLs
        print_info "Ensuring DLLs are available..."
        EXEC_DIR="./bin"
        copy_windows_dlls "$EXEC_DIR" "$BUILD_CONFIG"
        
        # Run the executable if build succeeded
        print_info "Running executable..."
        if [ -f "./bin/knapsack_main.exe" ]; then
            # Change to executable directory to ensure DLLs are found
            cd ./bin
            print_info "Working directory: $(pwd)"
            ./knapsack_main.exe
            # Return to build dir after execution
            cd ..
        else
            print_error "Executable not found at ./bin/knapsack_main.exe"
            exit 1
        fi
    else
        # Standard Windows-specific build with Visual Studio
        print_info "Using Visual Studio build system..."
        # Use a newer Visual Studio version if available
        cmake .. -G "Visual Studio 17 2022" -A x64 $CMAKE_OPTIONS || cmake .. -G "Visual Studio 16 2019" -A x64 $CMAKE_OPTIONS
        
        print_info "Building project..."
        cmake --build . --config $BUILD_CONFIG
        
        # Manually copy DLLs
        print_info "Ensuring DLLs are available..."
        EXEC_DIR="./bin/$BUILD_CONFIG"
        copy_windows_dlls "$EXEC_DIR" "$BUILD_CONFIG"
        
        # Run the executable
        print_info "Running executable..."
        if [ -f "./bin/$BUILD_CONFIG/knapsack_main.exe" ]; then
            # Change to executable directory to ensure DLLs are found
            cd "./bin/$BUILD_CONFIG"
            print_info "Working directory: $(pwd)"
            ./knapsack_main.exe
            # Return to build dir after execution
            cd ../..
        else
            print_error "Executable not found at ./bin/$BUILD_CONFIG/knapsack_main.exe"
            exit 1
        fi
    fi
elif $LINUX; then
    # Linux build
    print_info "Using Unix Makefiles..."
    cmake .. -DCMAKE_VERBOSE_MAKEFILE=ON -DCMAKE_BUILD_TYPE=$BUILD_CONFIG $CMAKE_OPTIONS
    
    print_info "Building project..."
    make VERBOSE=1
    
    # Run the executable
    print_info "Running executable..."
    if [ -f "./bin/knapsack_main" ]; then
        ./bin/knapsack_main
    else
        print_error "Executable not found at ./bin/knapsack_main"
        exit 1
    fi
elif $MAC; then
    # macOS build
    print_info "Using Unix Makefiles..."
    cmake .. -DCMAKE_VERBOSE_MAKEFILE=ON -DCMAKE_BUILD_TYPE=$BUILD_CONFIG $CMAKE_OPTIONS
    
    print_info "Building project..."
    make VERBOSE=1
    
    # Run the executable
    print_info "Running executable..."
    if [ -f "./bin/knapsack_main" ]; then
        ./bin/knapsack_main
    else
        print_error "Executable not found at ./bin/knapsack_main"
        exit 1
    fi
fi

# Return to original directory
cd ..

print_success "Build and execution completed successfully! ✅"