#include "../backend/include/knapsack/dp_knapsack.hpp"

using namespace std;


double_knapsack::double_knapsack(int n, vector<int>& weights, vector<int>& values, int capacity) {
    this->weights = weights;
    this ->values = values;
    this->capacity = capacity;
    this->item_count = n;
    max_profit = 0;
}

void double_knapsack::solve() {
    auto startTime = chrono::high_resolution_clock::now();
    knapsack();
    backtrack();
    auto endTime = chrono::high_resolution_clock::now();
    double executionTime = chrono::duration_cast<chrono::nanoseconds>(endTime - startTime).count();
    double spaceComplexity = get_space_complexity();
}

void double_knapsack::knapsack() {
    dp.resize(item_count + 1, vector<int>(capacity + 1, 0));
    for (int i = 1; i <= item_count; i++) {
        for (int w = 0; w <= capacity; w++) {
            if (weights[i - 1] <= w) {
                dp[i][w] = max(dp[i - 1][w], dp[i - 1][w - weights[i - 1]] + values[i - 1]);
            } else {
                dp[i][w] = dp[i - 1][w];
            }
        }
    }
    max_profit = dp[item_count][capacity];
}

void double_knapsack::backtrack() {
    int w = capacity;
    for (int i = item_count; i > 0 && max_profit > 0; i--) {
        if (max_profit != dp[i - 1][w]) {
            selected_items.push_back(i - 1);
            max_profit -= values[i - 1];
            w -= weights[i - 1];
        }
    }
}

vector<int> double_knapsack::get_selected_items() {
    return selected_items;
}

int double_knapsack::get_max_profit() {
    return max_profit;
}
