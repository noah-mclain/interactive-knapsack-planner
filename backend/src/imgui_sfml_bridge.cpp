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
            if (mouseWheel->wheel == sf::Mouse::Wheel::Vertical) {
                // Vertical scrolling - normal behavior
                s_mouseWheel += mouseWheel->delta;
            }
            else if (mouseWheel->wheel == sf::Mouse::Wheel::Horizontal) {
                // Horizontal scrolling (touchpad two-finger swipe)
                io.MouseWheelH += mouseWheel->delta;
            }
        }
    }
    else if (event.is<sf::Event::KeyPressed>()) {
        if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()) {
            // Set key modifier states
            io.KeyCtrl = keyPressed->control;
            io.KeyShift = keyPressed->shift;
            io.KeyAlt = keyPressed->alt;
            io.KeySuper = keyPressed->system;
            
            // Manually map keys - newer ImGui versions need this
            switch (keyPressed->code) {
                case sf::Keyboard::Key::Backspace: io.AddKeyEvent(ImGuiKey_Backspace, true); break;
                case sf::Keyboard::Key::Delete: io.AddKeyEvent(ImGuiKey_Delete, true); break;
                case sf::Keyboard::Key::Tab: io.AddKeyEvent(ImGuiKey_Tab, true); break;
                case sf::Keyboard::Key::Left: io.AddKeyEvent(ImGuiKey_LeftArrow, true); break;
                case sf::Keyboard::Key::Right: io.AddKeyEvent(ImGuiKey_RightArrow, true); break;
                case sf::Keyboard::Key::Up: io.AddKeyEvent(ImGuiKey_UpArrow, true); break;
                case sf::Keyboard::Key::Down: io.AddKeyEvent(ImGuiKey_DownArrow, true); break;
                case sf::Keyboard::Key::Enter: io.AddKeyEvent(ImGuiKey_Enter, true); break;
                case sf::Keyboard::Key::Escape: io.AddKeyEvent(ImGuiKey_Escape, true); break;
                case sf::Keyboard::Key::A: 
                    io.AddKeyEvent(ImGuiKey_A, true);
                    if (io.KeyCtrl) io.AddKeyEvent(ImGuiKey_A, true); 
                    break; // Ctrl+A: Select All
                case sf::Keyboard::Key::C: 
                    io.AddKeyEvent(ImGuiKey_C, true);
                    if (io.KeyCtrl) io.AddKeyEvent(ImGuiKey_C, true); 
                    break; // Ctrl+C: Copy
                case sf::Keyboard::Key::V: 
                    io.AddKeyEvent(ImGuiKey_V, true);
                    if (io.KeyCtrl) io.AddKeyEvent(ImGuiKey_V, true); 
                    break; // Ctrl+V: Paste
                case sf::Keyboard::Key::X: 
                    io.AddKeyEvent(ImGuiKey_X, true);
                    if (io.KeyCtrl) io.AddKeyEvent(ImGuiKey_X, true); 
                    break; // Ctrl+X: Cut
                case sf::Keyboard::Key::Y: 
                    io.AddKeyEvent(ImGuiKey_Y, true);
                    if (io.KeyCtrl) io.AddKeyEvent(ImGuiKey_Y, true); 
                    break; // Ctrl+Y: Redo
                case sf::Keyboard::Key::Z: 
                    io.AddKeyEvent(ImGuiKey_Z, true);
                    if (io.KeyCtrl) io.AddKeyEvent(ImGuiKey_Z, true); 
                    break; // Ctrl+Z: Undo
                default: break;
            }
        }
    }
    else if (event.is<sf::Event::KeyReleased>()) {
        if (const auto* keyReleased = event.getIf<sf::Event::KeyReleased>()) {
            // Set key modifier states
            io.KeyCtrl = keyReleased->control;
            io.KeyShift = keyReleased->shift;
            io.KeyAlt = keyReleased->alt;
            io.KeySuper = keyReleased->system;
            
            // Manually map keys for key releases
            switch (keyReleased->code) {
                case sf::Keyboard::Key::Backspace: io.AddKeyEvent(ImGuiKey_Backspace, false); break;
                case sf::Keyboard::Key::Delete: io.AddKeyEvent(ImGuiKey_Delete, false); break;
                case sf::Keyboard::Key::Tab: io.AddKeyEvent(ImGuiKey_Tab, false); break;
                case sf::Keyboard::Key::Left: io.AddKeyEvent(ImGuiKey_LeftArrow, false); break;
                case sf::Keyboard::Key::Right: io.AddKeyEvent(ImGuiKey_RightArrow, false); break;
                case sf::Keyboard::Key::Up: io.AddKeyEvent(ImGuiKey_UpArrow, false); break;
                case sf::Keyboard::Key::Down: io.AddKeyEvent(ImGuiKey_DownArrow, false); break;
                case sf::Keyboard::Key::Enter: io.AddKeyEvent(ImGuiKey_Enter, false); break;
                case sf::Keyboard::Key::Escape: io.AddKeyEvent(ImGuiKey_Escape, false); break;
                case sf::Keyboard::Key::A: 
                    io.AddKeyEvent(ImGuiKey_A, false);
                    if (io.KeyCtrl) io.AddKeyEvent(ImGuiKey_A, false); 
                    break;
                case sf::Keyboard::Key::C: 
                    io.AddKeyEvent(ImGuiKey_C, false);
                    if (io.KeyCtrl) io.AddKeyEvent(ImGuiKey_C, false); 
                    break;
                case sf::Keyboard::Key::V: 
                    io.AddKeyEvent(ImGuiKey_V, false);
                    if (io.KeyCtrl) io.AddKeyEvent(ImGuiKey_V, false); 
                    break;
                case sf::Keyboard::Key::X: 
                    io.AddKeyEvent(ImGuiKey_X, false);
                    if (io.KeyCtrl) io.AddKeyEvent(ImGuiKey_X, false); 
                    break;
                case sf::Keyboard::Key::Y: 
                    io.AddKeyEvent(ImGuiKey_Y, false);
                    if (io.KeyCtrl) io.AddKeyEvent(ImGuiKey_Y, false); 
                    break;
                case sf::Keyboard::Key::Z: 
                    io.AddKeyEvent(ImGuiKey_Z, false);
                    if (io.KeyCtrl) io.AddKeyEvent(ImGuiKey_Z, false); 
                    break;
                default: break;
            }
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
    // Note: io.MouseWheelH is already set in the ProcessEvent function
    
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