#pragma once
#include <vector>
#include <chrono>

using namespace std;

class double_knapsack {
    private:
        vector<int> weights;
        vector<int> values;
        int capacity;
        vector<vector<int>> dp;
        int item_count;
        vector<int> selected_items;
        int max_profit;
    public:
        void solve();
        double_knapsack(int n, vector<int>& weights, vector<int>& values, int capacity);
        void backtrack();
        void knapsack();
        vector<int> get_selected_items();
        int get_max_profit();
        double get_space_complexity() {
            return sizeof(weights) + sizeof(values) + sizeof(dp) + sizeof(item_count) + sizeof(selected_items) + sizeof(max_profit);
        }
};