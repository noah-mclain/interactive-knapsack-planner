#!/bin/bash

# Create directories
mkdir -p external/imgui

# Clone ImGui repository
if [ ! -d "external/imgui/.git" ]; then
    echo "Downloading ImGui..."
    git clone https://github.com/ocornut/imgui.git external/imgui
    
    # Check if clone was successful
    if [ $? -ne 0 ]; then
        echo "Failed to clone ImGui repository"
        exit 1
    fi
else
    echo "ImGui repository already exists, updating..."
    cd external/imgui
    git pull
    cd ../..
fi

# Download ImGui-SFML binding files if they don't exist
if [ ! -f "external/imgui/backends/imgui_impl_sfml.cpp" ]; then
    echo "Setting up ImGui-SFML backends..."
    mkdir -p external/imgui/backends
    
    # Create imgui_impl_sfml.cpp and imgui_impl_sfml.h in the backends directory
    curl -o external/imgui/backends/imgui_impl_sfml.cpp https://raw.githubusercontent.com/eliasdaler/imgui-sfml/master/imgui-SFML.cpp
    curl -o external/imgui/backends/imgui_impl_sfml.h https://raw.githubusercontent.com/eliasdaler/imgui-sfml/master/imgui-SFML.h
    
    # Check if download was successful
    if [ ! -f "external/imgui/backends/imgui_impl_sfml.cpp" ] || [ ! -f "external/imgui/backends/imgui_impl_sfml.h" ]; then
        echo "Failed to download ImGui-SFML binding files"
        exit 1
    fi
fi

echo "Dependencies setup completed successfully" 