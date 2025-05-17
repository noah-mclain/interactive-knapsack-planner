#include "include/knapsack/greedy_knapsack.hpp"
#include <iostream>
#include <iomanip>
#include <chrono>

void printItem(const knapsack::Item& item) {
    std::cout << "Item " << item.id 
              << ": Weight=" << item.weight 
              << ", Profit=" << item.profit 
              << ", Ratio=" << item.ratio << std::endl;
}

int main() {
    // Create a knapsack solver instance
    knapsack::GreedyKnapsack solver;
    
    // Set capacity
    double capacity = 8.0;
    solver.setCapacity(capacity);
    
    // Add items (weight, profit)
    solver.addItem(2.0, 10.0);  // Item 1: ratio = 5.0
    solver.addItem(3.0, 5.0);   // Item 2: ratio = 1.67
    solver.addItem(5.0, 15.0);  // Item 3: ratio = 3.0
    solver.addItem(1.0, 7.0);   // Item 4: ratio = 7.0
    solver.addItem(4.0, 8.0);   // Item 5: ratio = 2.0
    
    std::cout << "0/1 Knapsack Problem - Greedy Approach" << std::endl;
    std::cout << "=======================================" << std::endl;
    std::cout << "Capacity: " << capacity << std::endl;
    std::cout << "\nItems available:" << std::endl;
    
    for (const auto& item : solver.getItems()) {
        printItem(item);
    }
    
    // Solve the problem
    std::vector<knapsack::Item> selectedItems = solver.solve();
    
    // Calculate total weight and profit
    double totalWeight = 0.0;
    double totalProfit = 0.0;
    
    for (const auto& item : selectedItems) {
        totalWeight += item.weight;
        totalProfit += item.profit;
    }
    
    // Display results
    std::cout << "\nSelected items:" << std::endl;
    for (const auto& item : selectedItems) {
        printItem(item);
    }
    
    std::cout << "\nTotal weight: " << totalWeight << " / " << capacity << std::endl;
    std::cout << "Total profit: " << totalProfit << std::endl;
    
    // Display performance metrics
    auto nanoseconds = solver.getExecutionTime().count();
    std::cout << "\nPerformance Metrics:" << std::endl;
    std::cout << "Execution time: " << std::fixed << std::setprecision(3) 
              << nanoseconds / 1000000.0 << " ms" << std::endl;
    std::cout << "Time complexity: " << solver.getTimeComplexity() << std::endl;
    std::cout << "Memory usage: " << solver.getMemoryUsage() << " bytes" << std::endl;
    std::cout << "Space complexity: " << solver.getSpaceComplexity() << std::endl;
    
    return 0;
}
