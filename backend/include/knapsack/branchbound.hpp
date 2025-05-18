#pragma once
#include <vector>
#include <queue>
#include <chrono>

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

    double bound(const BBNode& node);

public:
    BranchBoundKnapsack();
    void setCapacity(int capacity);
    void addItem(int weight, int value);
    std::tuple<std::vector<BBItem>, std::chrono::nanoseconds, size_t> solve();
    int getMaxProfit();
};