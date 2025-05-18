#pragma once

#include <SFML/Graphics.hpp>
#include "imgui.h"
#include "gui/knapsack_gui.hpp"
#include "gui/huffman_gui.hpp"

class GuiApp {
public:
    GuiApp(sf::RenderWindow& window);
    ~GuiApp();

    // Main update and render methods
    void processEvents(const sf::Event& event);
    void update(sf::Time dt);
    void render();

    // Check if application should close
    bool shouldClose() const;

private:
    // References and state
    sf::RenderWindow& m_window;
    bool m_shouldClose;
    int m_currentTab;

    // GUI components
    KnapsackGUI m_knapsackGUI;
    HuffmanGUI m_huffmanGUI;
}; 