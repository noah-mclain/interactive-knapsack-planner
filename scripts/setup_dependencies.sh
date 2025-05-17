#!/bin/bash

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

# Detect OS using uname
OS_TYPE=$(uname -s)
OS_ARCH=$(uname -m)
WINDOWS=false; LINUX=false; MAC=false; MINGW=false

print_info "Setting up dependencies for platform: $OS_TYPE ($OS_ARCH)"

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
        else
            print_info "Detected Windows environment without MinGW ($OS_ARCH)"
        fi
        ;;
    *)
        print_warning "Unknown OS type: $OS_TYPE, proceeding with generic setup"
        ;;
esac

# Create directories
mkdir -p external/imgui

# Clone ImGui repository
if [ ! -d "external/imgui/.git" ]; then
    print_info "Downloading ImGui..."
    git clone https://github.com/ocornut/imgui.git external/imgui
    
    # Check if clone was successful
    if [ $? -ne 0 ]; then
        print_error "Failed to clone ImGui repository"
        exit 1
    fi
else
    print_info "ImGui repository already exists, updating..."
    cd external/imgui
    git pull
    cd ../..
fi

# Ensure the backends directory exists
print_info "Checking ImGui backends..."
if [ ! -d "external/imgui/backends" ]; then
    print_warning "ImGui backends directory not found. Your ImGui version might be outdated."
    print_warning "Please update ImGui to the latest version."
    exit 1
fi

# Platform-specific setup
if $LINUX; then
    print_info "Checking SFML on Linux..."
    if ! dpkg -l | grep -q libsfml-dev; then
        print_info "SFML doesn't seem to be installed. Please install it with:"
        echo "sudo apt-get install libsfml-dev"
    fi
elif $MAC; then
    print_info "Checking SFML on macOS..."
    if ! brew list | grep -q sfml; then
        print_info "SFML doesn't seem to be installed. Please install it with:"
        echo "brew install sfml"
    fi
    
    # Check for Apple Silicon Mac and add specific advice
    if [[ "$OS_ARCH" == "arm64" ]]; then
        print_info "Detected Apple Silicon Mac. Ensuring Homebrew paths are correct."
        # Additional Apple Silicon specific setup could go here
    fi
elif $WINDOWS; then
    print_info "SFML on Windows should be installed manually."
    echo "Please follow instructions at: https://www.sfml-dev.org/download.php"
fi

print_success "Dependencies setup completed successfully" 