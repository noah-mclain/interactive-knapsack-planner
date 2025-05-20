#include "../../include/knapsack/dp_knapsack.hpp"
#include <iostream>

using namespace std;


double_knapsack::double_knapsack(int n, vector<int>& weights, vector<int>& values, int capacity) {
    this->weights = weights;
    this ->values = values;
    this->capacity = capacity;
    this->item_count = n;
    max_profit = 0;
    execution_time = std::chrono::nanoseconds(0);
}

void double_knapsack::solve() {
    auto startTime = chrono::high_resolution_clock::now();
    knapsack();
    backtrack();
    auto endTime = chrono::high_resolution_clock::now();
    execution_time = chrono::duration_cast<chrono::nanoseconds>(endTime - startTime);
    double spaceComplexity = get_space_complexity();
    cout << "Execution Time: " << get_execution_time_ms() << " ms" << endl;
    cout << "Space Complexity: " << spaceComplexity << " bytes" << endl;
    cout << "Max Profit: " << max_profit << endl;
    cout << "Selected items: ";
    for (const auto& item : selected_items) {
        cout << item << " ";
    }
    cout << endl;
    cout << "Selected items count: " << selected_items.size() << endl;
    
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
    int current_profit = max_profit;
    for (int i = item_count; i > 0 && current_profit > 0; i--) {
        if (current_profit != dp[i - 1][w]) {
            selected_items.push_back(i - 1);
            current_profit -= values[i - 1];
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
