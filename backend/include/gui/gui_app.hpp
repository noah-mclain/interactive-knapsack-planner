#pragma once

#include <SFML/Graphics.hpp>
#include "imgui.h"
#include "knapsack/knapsack_solver.hpp"
#include <vector>
#include <string>

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
    // GUI tab rendering methods
    void renderKnapsackTab();
    void renderHuffmanTab();
    
    // Helper methods
    void solveKnapsack();
    void renderResults();
    void renderVisualRepresentation(const SolverResult& result);

    // References and state
    sf::RenderWindow& m_window;
    bool m_shouldClose;
    int m_currentTab;

    // Knapsack data
    std::vector<int> m_weights;
    std::vector<int> m_values;
    int m_capacity;
    
    // Item being edited
    int m_editIndex;
    int m_editWeight;
    int m_editValue;
    
    // New item data
    int m_newWeight;
    int m_newValue;
    
    // Algorithm selection flags
    bool m_useGreedy;
    bool m_useBruteForce;
    bool m_useDynamicProgramming;
    bool m_useBranchAndBound;
    
    // Results storage
    std::vector<SolverResult> m_results;
    bool m_hasResults;
}; 