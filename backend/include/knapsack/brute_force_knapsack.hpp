#pragma once

#include <vector>
#include <chrono>
#include <string>
#include "greedy_knapsack.hpp"  // Reuse the Item struct

namespace knapsack {

// Class implementing the brute force approach to the 0/1 Knapsack problem
class BruteForceKnapsack {
private:
    std::vector<Item> items;
    double capacity;
    
    // Helper function to generate all possible subsets
    std::vector<std::vector<bool>> generateSubsets(size_t n);
    
    // Performance metrics
    std::chrono::nanoseconds executionTime;
    size_t memoryUsage;

public:
    BruteForceKnapsack();
    
    // Add a single item to the knapsack
    void addItem(double weight, double profit);
    
    // Set the capacity of the knapsack
    void setCapacity(double capacity);
    
    // Clear all items
    void clearItems();
    
    // Solve the knapsack problem using the brute force approach
    std::vector<Item> solve();
    
    // Get all items
    std::vector<Item> getItems() const;
    
    // Get performance metrics
    std::chrono::nanoseconds getExecutionTime() const;
    std::string getTimeComplexity() const;
    size_t getMemoryUsage() const;
    std::string getSpaceComplexity() const;
};

} // namespace knapsack 