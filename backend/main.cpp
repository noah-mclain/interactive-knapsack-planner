#include <SFML/Graphics.hpp>
#include "imgui.h"
#include "backends/imgui_impl_opengl3.h"
#include "knapsack/knapsack.h"
#include "config.h"

#include <iostream>
#include <vector>
#include <string>
#include <cstddef>

// Simple application that displays a knapsack problem solver UI
int main() {
    // Create SFML window
    sf::RenderWindow window(sf::VideoMode({800, 600}), "Interactive Knapsack Planner");
    window.setFramerateLimit(60);
    
    // Initialize ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui::StyleColorsDark();
    
    // Use the GLSL version defined at build time by CMake
    const char* glsl_version = GLSL_VERSION;
    
    // Initialize OpenGL3 backend with platform-specific GLSL version
    ImGui_ImplOpenGL3_Init(glsl_version);
    
    // Clock for ImGui timing
    sf::Clock deltaClock;
    
    // Knapsack solver data
    std::vector<int> weights = {23, 4, 102, 77, 4, 6, 2, 7, 3, 5, 4};
    std::vector<int> values = {87, 16, 58, 9, 12, 6, 18, 9, 14, 11, 13};
    int capacity = 25;
    
    // Main loop
    while (window.isOpen()) {
        // Process events but don't try to check specific types
        auto eventOption = window.pollEvent();
        while (eventOption) {
            eventOption = window.pollEvent();
        }
        
        // Start ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        
        // Set display size and timing
        io.DisplaySize = ImVec2(static_cast<float>(window.getSize().x), static_cast<float>(window.getSize().y));
        io.DeltaTime = deltaClock.restart().asSeconds();
        
        ImGui::NewFrame();
        
        // Simple ImGui Window
        ImGui::SetNextWindowPos(ImVec2(50, 50), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(550, 500), ImGuiCond_FirstUseEver);
        
        ImGui::Begin("Knapsack Planner");
        
        ImGui::Text("Interactive Knapsack Problem Solver");
        ImGui::Separator();
        
        // Capacity slider
        ImGui::SliderInt("Knapsack Capacity", &capacity, 1, 200);
        
        // Items list
        ImGui::Text("Current Items:");
        for (std::size_t i = 0; i < weights.size(); i++) {
            ImGui::Text("Item %zu: Weight %d, Value %d", i+1, weights[i], values[i]);
        }
        
        ImGui::Separator();
        
        // Add item section
        static int new_weight = 0;
        static int new_value = 0;
        ImGui::InputInt("Weight", &new_weight);
        ImGui::InputInt("Value", &new_value);
        
        if (ImGui::Button("Add Item")) {
            if (new_weight > 0) {
                weights.push_back(new_weight);
                values.push_back(new_value);
                new_weight = 0;
                new_value = 0;
            }
        }
        
        ImGui::SameLine();
        
        if (ImGui::Button("Solve")) {
            // Create boolean array for algorithms
            bool algorithms[3] = {true, true, true}; // Greedy, Brute Force, Dynamic Programming
            
            // Initialize solver
            KnapsackSolver solver(weights.size(), weights, values, capacity, algorithms);
            
            // Solve problem
            solver.solve();
            
            // Output result
            std::cout << "Solved knapsack problem with capacity " << capacity << std::endl;
        }
        
        ImGui::End(); // End of window
        
        // Render
        window.clear(sf::Color(50, 50, 50));
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        window.display();
    }
    
    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui::DestroyContext();
    
    return 0;
}
