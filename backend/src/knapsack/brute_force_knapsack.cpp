#include "../../include/knapsack/brute_force_knapsack.hpp"
#include <algorithm>
#include <numeric>
#include <cmath>

namespace knapsack {

BruteForceKnapsack::BruteForceKnapsack() 
    : capacity(0), executionTime(std::chrono::nanoseconds(0)), memoryUsage(0) {
}

void BruteForceKnapsack::addItem(double weight, double profit) {
    int id = static_cast<int>(items.size());
    items.emplace_back(id, weight, profit);
}

void BruteForceKnapsack::setCapacity(double cap) {
    capacity = cap;
}

void BruteForceKnapsack::clearItems() {
    items.clear();
}

// Helper function to generate all possible subsets (combinations of items)
std::vector<std::vector<bool>> BruteForceKnapsack::generateSubsets(size_t n) {
    // Total number of possible subsets is 2^n (each item is either included or not)
    size_t totalSubsets = static_cast<size_t>(std::pow(2, n));
    std::vector<std::vector<bool>> subsets(totalSubsets);
    
    for (size_t i = 0; i < totalSubsets; ++i) {
        std::vector<bool> subset(n);
        for (size_t j = 0; j < n; ++j) {
            // Check if the j-th bit is set in i
            subset[j] = (i & (1 << j)) != 0;
        }
        subsets[i] = subset;
    }
    
    return subsets;
}

std::tuple<std::vector<Item>, std::chrono::nanoseconds, size_t> BruteForceKnapsack::solve() {
    // Track memory usage before operation
    size_t initialMemory = items.capacity() * sizeof(Item);
    
    // Start timing
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Get all possible subsets (combinations of items)
    std::vector<std::vector<bool>> subsets = generateSubsets(items.size());
    
    // Initialize variables for the best solution
    double maxProfit = 0.0;
    std::vector<bool> bestSubset;
    
    // Evaluate each subset
    for (const auto& subset : subsets) {
        double totalWeight = 0.0;
        double totalProfit = 0.0;
        
        // Calculate weight and profit for this subset
        for (size_t i = 0; i < subset.size(); ++i) {
            if (subset[i]) {
                totalWeight += items[i].weight;
                totalProfit += items[i].profit;
            }
        }
        
        // Check if this subset is valid and better than the current best
        if (totalWeight <= capacity && totalProfit > maxProfit) {
            maxProfit = totalProfit;
            bestSubset = subset;
            max_profit = totalProfit;
        }
    }
    
    // Create the result vector with selected items
    std::vector<Item> selectedItems;
    for (size_t i = 0; i < bestSubset.size(); ++i) {
        if (bestSubset[i]) {
            selectedItems.push_back(items[i]);
        }
    }
    
    // Stop timing
    auto endTime = std::chrono::high_resolution_clock::now();
    executionTime = std::chrono::duration_cast<std::chrono::nanoseconds>(endTime - startTime);
    
    // Calculate memory usage (include space for subsets)
    size_t subsetsMemory = subsets.size() * subsets[0].size() * sizeof(bool);
    memoryUsage = initialMemory + subsetsMemory + selectedItems.capacity() * sizeof(Item);
    
    // Return tuple with results
    return std::make_tuple(selectedItems, executionTime, memoryUsage);
}

std::vector<Item> BruteForceKnapsack::getItems() const {
    return items;
}

std::chrono::nanoseconds BruteForceKnapsack::getExecutionTime() const {
    return executionTime;
}

std::string BruteForceKnapsack::getTimeComplexity() const {
    return "O(2^n) - we examine all possible combinations of items";
}

size_t BruteForceKnapsack::getMemoryUsage() const {
    return memoryUsage;
}

std::string BruteForceKnapsack::getSpaceComplexity() const {
    return "O(2^n) - we store all possible combinations of items";
}

int BruteForceKnapsack::getMaxProfit() const {
    return max_profit;
} // namespace knapsack
}
