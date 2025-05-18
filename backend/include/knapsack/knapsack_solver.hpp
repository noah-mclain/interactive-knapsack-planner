#pragma once
#include "brute_force_knapsack.hpp"
#include "greedy_knapsack.hpp"
#include "dp_knapsack.hpp"
#include "branchbound.hpp"
#include <iostream>
#include <vector>
#include <string>

// Struct to store results that can be directly used by the GUI
struct SolverResult {
    std::string algorithm_name;
    std::vector<int> selected_items;
    double execution_time_ms;
    size_t memory_usage;
    double max_profit;
    void* algorithm_data; // Pointer to algorithm-specific data for visualization
    
    SolverResult() : algorithm_data(nullptr) {} // Initialize to nullptr
};

using namespace std;

class KnapsackSolver {
    private:
        vector<int> weights;
        vector<int> values;
        int capacity;
        int item_count;
        bool algorithms[4] = {false, false, false, false}; // 0: Greedy, 1: Brute Force, 2: Dynamic Programming, 3: Branch and Bound
        double execution_time;
        double space_complexity;
        std::vector<SolverResult> results;

    public:
        KnapsackSolver(int n, vector<int>& weights, vector<int>& values, int capacity, bool algorithms[4]);
        void solve();
        double get_execution_time();
        double get_space_complexity();
        int get_item_count(){return item_count;};
        vector<int> get_weights(){return weights;};
        vector<int> get_values(){return values;};
        int get_capacity(){return capacity;};
        const std::vector<SolverResult>& get_results() const { return results; }
};