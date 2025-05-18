#include "../include/gui/imgui_sfml_bridge.hpp"
#include "backends/imgui_impl_opengl3.h"
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>

// Track input state
static bool s_mousePressed[3] = { false, false, false };
static float s_mouseWheel = 0.0f;
static sf::Clock s_deltaClock;

namespace ImGuiSFMLBridge {

bool Initialize(sf::RenderWindow& window, const char* glsl_version) {
    // Initialize ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable keyboard navigation
    
    // Initialize the OpenGL3 implementation
    if (!ImGui_ImplOpenGL3_Init(glsl_version)) {
        std::cerr << "Failed to initialize ImGui OpenGL3 implementation" << std::endl;
        return false;
    }
    
    // Setup default font
    io.Fonts->AddFontDefault();
    
    // Reset clock
    s_deltaClock.restart();
    
    return true;
}

void ProcessEvent(const sf::Event& event) {
    ImGuiIO& io = ImGui::GetIO();
    
    // SFML 3.0 uses std::variant for events
    if (event.is<sf::Event::Resized>()) {
        if (const auto* resized = event.getIf<sf::Event::Resized>()) {
            io.DisplaySize = ImVec2(static_cast<float>(resized->size.x), 
                                  static_cast<float>(resized->size.y));
        }
    }
    else if (event.is<sf::Event::MouseMoved>()) {
        if (const auto* mouseMoved = event.getIf<sf::Event::MouseMoved>()) {
            io.MousePos = ImVec2(static_cast<float>(mouseMoved->position.x), 
                               static_cast<float>(mouseMoved->position.y));
        }
    }
    else if (event.is<sf::Event::MouseButtonPressed>()) {
        if (const auto* mousePressed = event.getIf<sf::Event::MouseButtonPressed>()) {
            if (mousePressed->button == sf::Mouse::Button::Left)
                s_mousePressed[0] = true;
            if (mousePressed->button == sf::Mouse::Button::Right)
                s_mousePressed[1] = true;
            if (mousePressed->button == sf::Mouse::Button::Middle)
                s_mousePressed[2] = true;
        }
    }
    else if (event.is<sf::Event::MouseButtonReleased>()) {
        if (const auto* mouseReleased = event.getIf<sf::Event::MouseButtonReleased>()) {
            if (mouseReleased->button == sf::Mouse::Button::Left)
                io.MouseDown[0] = false;
            if (mouseReleased->button == sf::Mouse::Button::Right)
                io.MouseDown[1] = false;
            if (mouseReleased->button == sf::Mouse::Button::Middle)
                io.MouseDown[2] = false;
        }
    }
    else if (event.is<sf::Event::MouseWheelScrolled>()) {
        if (const auto* mouseWheel = event.getIf<sf::Event::MouseWheelScrolled>()) {
            if (mouseWheel->wheel == sf::Mouse::Wheel::Vertical)
                s_mouseWheel += mouseWheel->delta;
        }
    }
    else if (event.is<sf::Event::KeyPressed>()) {
        if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()) {
            // Set key modifier states
            io.KeyCtrl = keyPressed->control;
            io.KeyShift = keyPressed->shift;
            io.KeyAlt = keyPressed->alt;
            io.KeySuper = keyPressed->system;
        }
    }
    else if (event.is<sf::Event::KeyReleased>()) {
        if (const auto* keyReleased = event.getIf<sf::Event::KeyReleased>()) {
            // Set key modifier states
            io.KeyCtrl = keyReleased->control;
            io.KeyShift = keyReleased->shift;
            io.KeyAlt = keyReleased->alt;
            io.KeySuper = keyReleased->system;
        }
    }
    else if (event.is<sf::Event::TextEntered>()) {
        if (const auto* textEntered = event.getIf<sf::Event::TextEntered>()) {
            // Add character to input
            if (textEntered->unicode > 0 && textEntered->unicode < 0x10000)
                io.AddInputCharacter(textEntered->unicode);
        }
    }
}

void Update(sf::RenderWindow& window, sf::Time dt) {
    ImGuiIO& io = ImGui::GetIO();
    
    // Setup display size
    sf::Vector2u windowSize = window.getSize();
    io.DisplaySize = ImVec2(static_cast<float>(windowSize.x), static_cast<float>(windowSize.y));
    
    // Setup time step
    io.DeltaTime = dt.asSeconds();
    
    // Update mouse position
    if (window.hasFocus()) {
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        io.MousePos = ImVec2(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));
    } else {
        io.MousePos = ImVec2(-FLT_MAX, -FLT_MAX);
    }
    
    // Update mouse buttons
    io.MouseDown[0] = s_mousePressed[0] || sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);
    io.MouseDown[1] = s_mousePressed[1] || sf::Mouse::isButtonPressed(sf::Mouse::Button::Right);
    io.MouseDown[2] = s_mousePressed[2] || sf::Mouse::isButtonPressed(sf::Mouse::Button::Middle);
    s_mousePressed[0] = s_mousePressed[1] = s_mousePressed[2] = false;
    
    // Update mouse wheel
    io.MouseWheel = s_mouseWheel;
    s_mouseWheel = 0.0f;
    
    // Setup low-level inputs
    ImGui_ImplOpenGL3_NewFrame();
    ImGui::NewFrame();
}

void Render(sf::RenderWindow& window) {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Shutdown() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui::DestroyContext();
}

} // namespace ImGuiSFMLBridge 