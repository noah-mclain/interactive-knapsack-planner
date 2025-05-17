#pragma once

#include <vector>
#include <chrono>
#include <string>
#include <tuple>
namespace knapsack {

// Structure to represent an item with weight and profit
struct Item {
    int id;
    double weight;
    double profit;
    double ratio; // profit-to-weight ratio

    Item(int id, double weight, double profit);
};

class GreedyKnapsack {
private:
    std::vector<Item> items;
    double capacity;
    
    // Performance metrics
    std::chrono::nanoseconds executionTime;
    size_t memoryUsage;

public:
    GreedyKnapsack();
    
    // Add a single item to the knapsack
    void addItem(double weight, double profit);
    
    // Set the capacity of the knapsack
    void setCapacity(double capacity);
    
    // Clear all items
    void clearItems();
    
    // Solve the knapsack problem using the greedy approach
    std::tuple<std::vector<Item>, std::chrono::nanoseconds, size_t, double> solve();
    
    // Get all items
    std::vector<Item> getItems() const;
    
    // Get performance metrics
    std::chrono::nanoseconds getExecutionTime() const;
    std::string getTimeComplexity() const;
    size_t getMemoryUsage() const;
    std::string getSpaceComplexity() const;
};

} // namespace knapsack
