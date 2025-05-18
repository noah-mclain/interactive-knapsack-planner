#include <SFML/Graphics.hpp>
#include "imgui.h"
#include "backends/imgui_impl_opengl3.h"
#include "knapsack/knapsack_solver.hpp"
#include "config.h"
#include "gui/imgui_sfml_bridge.hpp"
#include "gui/gui_app.hpp"

#include <iostream>

// Simple application that displays a knapsack problem solver UI
int main() {
    // Create SFML window with larger default size
    sf::RenderWindow window(sf::VideoMode({1024, 768}), "Interactive Algorithm Visualizer");
    window.setFramerateLimit(60);
    
    // Use the GLSL version defined at build time by CMake
    const char* glsl_version = GLSL_VERSION;
    
    // Initialize ImGui using our bridge
    if (!ImGuiSFMLBridge::Initialize(window, glsl_version)) {
        std::cerr << "Failed to initialize ImGui SFML Bridge" << std::endl;
        return 1;
    }
    
    // Create GUI application
    GuiApp app(window);
    
    // Clock for delta time calculation
    sf::Clock deltaClock;
    
    // Main loop
    while (window.isOpen() && !app.shouldClose()) {
        // Process events
        auto eventOption = window.pollEvent();
        while (eventOption) {
            // Process event for our application
            app.processEvents(*eventOption);
            
            eventOption = window.pollEvent();
        }
        
        // Update application
        sf::Time dt = deltaClock.restart();
        app.update(dt);
        
        // Render
        window.clear(sf::Color(30, 30, 30));
        app.render();
        window.display();
    }
    
    // Cleanup
    ImGuiSFMLBridge::Shutdown();
    
    return 0;
}
