#pragma once

#include <SFML/Graphics.hpp>
#include "imgui.h"
#include "knapsack/knapsack_solver.hpp"
#include <vector>

class KnapsackGUI {
public:
    KnapsackGUI();
    ~KnapsackGUI();

    // Main render method
    void render();
    
    // Getters
    bool hasResults() const { return m_hasResults; }
    
private:
    // Helper methods
    void solveKnapsack();
    void renderResults();
    void renderVisualRepresentation(const SolverResult& result);
    
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