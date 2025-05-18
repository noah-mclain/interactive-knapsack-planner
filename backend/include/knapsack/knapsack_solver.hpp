#pragma once
#include "brute_force_knapsack.hpp"
#include "greedy_knapsack.hpp"
#include "dp_knapsack.hpp"
#include <iostream>
#include <vector>

using namespace std;

class KnapsackSolver {
    private:
        vector<int> weights;
        vector<int> values;
        int capacity;
        int item_count;
        bool algorithms[4]= {false, false, false, false}; // 0: Greedy, 1: Brute Force, 2: Dynamic Programming
        double execution_time;
        double space_complexity;

    public:
        KnapsackSolver(int n, vector<int>& weights, vector<int>& values, int capacity, bool algorithms[3]);
        void solve();
        double get_execution_time();
        double get_space_complexity();
        int get_item_count(){return item_count;};
        vector<int> get_weights(){return weights;};
        vector<int> get_values(){return values;};
        int get_capacity(){return capacity;};
};