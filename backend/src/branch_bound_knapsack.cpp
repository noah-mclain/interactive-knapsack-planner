#include "../include/knapsack/branchbound.hpp"
#include <algorithm>
#include <iostream>
#include <stack>

BranchBoundKnapsack::BranchBoundKnapsack() {
    capacity = 0;
    maxProfit = 0;
    item_count = 0;
}

void BranchBoundKnapsack::setCapacity(int c) {
    capacity = c;
}

void BranchBoundKnapsack::addItem(int weight, int value) {
    items.push_back({weight, value, item_count++, (double)value / weight});
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

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::nanoseconds execTime = end - start;
    size_t memory = sizeof(BBItem) * items.size() + sizeof(BBNode) * pq.size();
    return {selected, execTime, memory};
}

int BranchBoundKnapsack::getMaxProfit() {
    return maxProfit;
}
