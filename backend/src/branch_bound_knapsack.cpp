#include "../include/knapsack/branchbound.hpp"
#include <algorithm>
#include <iostream>
#include <stack>

BranchBoundKnapsack::BranchBoundKnapsack() {
    capacity = 0;
    maxProfit = 0;
    item_count = 0;
    executionTime = std::chrono::nanoseconds(0);
    memoryUsage = 0;
}

void BranchBoundKnapsack::setCapacity(int c) {
    capacity = c;
}

void BranchBoundKnapsack::addItem(int weight, int value) {
    items.push_back({weight, value, item_count++, (double)value / weight});
}

void BranchBoundKnapsack::clearItems() {
    items.clear();
    item_count = 0;
    selected_items.clear();
    maxProfit = 0;
}

double BranchBoundKnapsack::bound(const BBNode& node) {
    if (node.weight >= capacity) return 0;
    double profit_bound = node.profit;
    int w = node.weight;
    int i = node.level + 1;
    while (i < item_count && w + items[i].weight <= capacity) {
        w += items[i].weight;
        profit_bound += items[i].value;
        i++;
    }
    if (i < item_count)
        profit_bound += (capacity - w) * items[i].ratio;
    return profit_bound;
}

std::tuple<std::vector<BBItem>, std::chrono::nanoseconds, size_t> BranchBoundKnapsack::solve() {
    // Track initial memory usage
    size_t initialMemory = items.capacity() * sizeof(BBItem);
    
    // Start timing
    auto start = std::chrono::high_resolution_clock::now();

    std::sort(items.begin(), items.end(), [](const BBItem& a, const BBItem& b) {
        return a.ratio > b.ratio;
    });

    std::priority_queue<BBNode> pq;
    BBNode root{ -1, 0, 0, 0.0, {} };
    root.bound = bound(root);
    pq.push(root);
    maxProfit = 0;
    selected_items.clear();

    while (!pq.empty()) {
        BBNode node = pq.top(); pq.pop();
        if (node.bound <= maxProfit) continue;

        // Skip if we're at the last item
        if (node.level == item_count - 1)
            continue;

        // Include current item
        BBNode next;
        next.level = node.level + 1;
        next.weight = node.weight + items[next.level].weight;
        next.profit = node.profit + items[next.level].value;
        next.items = node.items;

        if (next.weight <= capacity && next.profit > maxProfit) {
            maxProfit = next.profit;
            next.items.push_back(items[next.level].id);
            selected_items = next.items;
        }

        next.bound = bound(next);
        if (next.bound > maxProfit) {
            pq.push(next);
        }

        // Exclude item
        BBNode exclude = node;
        exclude.level = next.level;
        exclude.bound = bound(exclude);
        if (exclude.bound > maxProfit) {
            pq.push(exclude);
        }
    }

    std::vector<BBItem> selected;
    for (int id : selected_items) {
        selected.push_back(items[id]);
    }

    // Stop timing and calculate memory usage
    auto end = std::chrono::high_resolution_clock::now();
    executionTime = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
    memoryUsage = initialMemory + (pq.size() * sizeof(BBNode)) + (selected.capacity() * sizeof(BBItem));
    
    return {selected, executionTime, memoryUsage};
}

int BranchBoundKnapsack::getMaxProfit() const {
    return maxProfit;
}

std::chrono::nanoseconds BranchBoundKnapsack::getExecutionTime() const {
    return executionTime;
}

size_t BranchBoundKnapsack::getMemoryUsage() const {
    return memoryUsage;
}

std::string BranchBoundKnapsack::getTimeComplexity() const {
    return "O(2^n) - worst case, but often performs much better in practice";
}

std::string BranchBoundKnapsack::getSpaceComplexity() const {
    return "O(2^n) - due to priority queue storage, but often much less in practice";
}

std::vector<int> BranchBoundKnapsack::getSelectedItems() const {
    return selected_items;
}
