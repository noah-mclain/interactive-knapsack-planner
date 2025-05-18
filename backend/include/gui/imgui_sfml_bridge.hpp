#pragma once

#include <SFML/Graphics.hpp>
#include "imgui.h"

namespace ImGuiSFMLBridge {
    // Initialize ImGui with SFML
    bool Initialize(sf::RenderWindow& window, const char* glsl_version);
    
    // Process SFML events for ImGui
    void ProcessEvent(const sf::Event& event);
    
    // Update ImGui for new frame with time delta
    void Update(sf::RenderWindow& window, sf::Time dt);
    
    // Render ImGui to the SFML window
    void Render(sf::RenderWindow& window);
    
    // Shutdown ImGui
    void Shutdown();
} 