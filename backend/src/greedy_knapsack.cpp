#include "../include/knapsack/greedy_knapsack.hpp"
#include <algorithm>
#include <numeric>
#include <sstream>

namespace knapsack {

Item::Item(int id, double weight, double profit) 
    : id(id), weight(weight), profit(profit) {
    ratio = (weight != 0) ? profit / weight : 0;
}

GreedyKnapsack::GreedyKnapsack() 
    : capacity(0), executionTime(std::chrono::nanoseconds(0)), memoryUsage(0) {
}

void GreedyKnapsack::addItem(double weight, double profit) {
    int id = static_cast<int>(items.size());
    items.emplace_back(id, weight, profit);
}

void GreedyKnapsack::setCapacity(double cap) {
    capacity = cap;
}

void GreedyKnapsack::clearItems() {
    items.clear();
}

std::tuple<std::vector<Item>, std::chrono::nanoseconds, size_t, double> GreedyKnapsack::solve() {
    // Track memory usage before operation
    size_t initialMemory = items.capacity() * sizeof(Item);
    
    // Start timing
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Create a copy of the items to sort
    std::vector<Item> sortedItems = items;
    
    // Sort items by profit-to-weight ratio in descending order (greedy approach)
    std::sort(sortedItems.begin(), sortedItems.end(), 
              [](const Item& a, const Item& b) {
                  return a.ratio > b.ratio;
              });
    
    // Apply the greedy approach - select items in order of highest ratio first
    std::vector<Item> selectedItems;
    double currentWeight = 0;
    double maxProfit = 0;
    
    for (const auto& item : sortedItems) {
        // For 0/1 knapsack, we can either take an item or not, no fractional selection
        if (currentWeight + item.weight <= capacity) {
            selectedItems.push_back(item);
            currentWeight += item.weight;
            maxProfit += item.profit;
        }
    }
    
    // Stop timing
    auto endTime = std::chrono::high_resolution_clock::now();
    executionTime = std::chrono::duration_cast<std::chrono::nanoseconds>(endTime - startTime);
    
    // Calculate memory usage
    memoryUsage = initialMemory + (sortedItems.capacity() + selectedItems.capacity()) * sizeof(Item);
    
    // Return tuple with results
    return std::make_tuple(selectedItems, executionTime, memoryUsage, maxProfit);
}

std::vector<Item> GreedyKnapsack::getItems() const {
    return items;
}

std::chrono::nanoseconds GreedyKnapsack::getExecutionTime() const {
    return executionTime;
}

std::string GreedyKnapsack::getTimeComplexity() const {
    // The time complexity is dominated by the sorting operation
    return "O(n log n) - dominated by the sorting operation";
}

size_t GreedyKnapsack::getMemoryUsage() const {
    return memoryUsage;
}

std::string GreedyKnapsack::getSpaceComplexity() const {
    return "O(n) - we store at most 2n items (original and sorted lists)";
}

} // namespace knapsack
