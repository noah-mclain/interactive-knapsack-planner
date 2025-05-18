#pragma once
#include <vector>
#include <queue>
#include <chrono>
#include <string>

struct BBItem {
    int weight, value, id;
    double ratio;
};
struct BBNode {
    int level, profit, weight;
    double bound;
    std::vector<int> items;
    bool operator<(const BBNode& other) const {
        return bound < other.bound;
    }
};

class BranchBoundKnapsack {
private:
    int capacity, item_count;
    std::vector<BBItem> items;
    int maxProfit;
    std::vector<int> selected_items;
    
    // Performance metrics
    std::chrono::nanoseconds executionTime;
    size_t memoryUsage;

    double bound(const BBNode& node);

public:
    BranchBoundKnapsack();
    void setCapacity(int capacity);
    void addItem(int weight, int value);
    void clearItems();
    std::tuple<std::vector<BBItem>, std::chrono::nanoseconds, size_t> solve();
    
    // Getters
    int getMaxProfit() const;
    std::chrono::nanoseconds getExecutionTime() const;
    size_t getMemoryUsage() const;
    std::string getTimeComplexity() const;
    std::string getSpaceComplexity() const;
    std::vector<int> getSelectedItems() const;
};