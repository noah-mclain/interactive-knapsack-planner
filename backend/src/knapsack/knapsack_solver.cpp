#include "../../include/knapsack/knapsack_solver.hpp"
#include "../../include/knapsack/branchbound.hpp"
#include <iostream>
#include <chrono>
#include <sstream>

using namespace std;
using namespace knapsack;



KnapsackSolver::KnapsackSolver(int n, vector<int>& weights, vector<int>& values, int capacity, bool algorithms[4]) {
    this->weights = weights;
    this->values = values;
    this->capacity = capacity;
    this->item_count = n;

    this->algorithms[0] = algorithms[0]; // Greedy
    this->algorithms[1] = algorithms[1]; // Brute Force
    this->algorithms[2] = algorithms[2]; // Dynamic Programming
    this->algorithms[3] = algorithms[3]; // Branch and Bound
}

void KnapsackSolver::solve() {
    // Clear previous results
    results.clear();
    
    if (algorithms[0]){
        //greedy
        cout<<"Greedy Knapsack Algorithm"<<endl;
        cout<<"========================="<<endl;
        GreedyKnapsack greedy = GreedyKnapsack();

        for (int i = 0; i < item_count; i++) {
            greedy.addItem(weights[i], values[i]);
        }

        greedy.setCapacity(capacity);
        auto [selected, execTime, memUsage, maxProfit] = greedy.solve();
        double executionTime = execTime.count() / 1000000.0; // ns to ms
        
        cout << "Execution Time: " << executionTime << " ms" << endl;
        cout << "Memory Usage: " << memUsage << " bytes" << endl;
        cout << "Max Profit: " << maxProfit << endl;
        cout << "Selected items: ";
        for (const auto& item : selected) {
            cout << item.id << " ";
        }
        cout << endl;
        
        // Store results for GUI
        SolverResult result;
        result.algorithm_name = "Greedy";
        for (const auto& item : selected) {
            result.selected_items.push_back(item.id);
        }
        result.execution_time_ms = executionTime;
        result.memory_usage = memUsage;
        result.max_profit = maxProfit;
        results.push_back(result);
    }
    
    if(algorithms[1]){
        //brute force
        cout<<"Brute Force Knapsack Algorithm"<<endl;
        cout<<"============================="<<endl;
        BruteForceKnapsack bf = BruteForceKnapsack();

        bf.setCapacity(capacity);
        for (int i = 0; i < item_count; i++) {
            bf.addItem(weights[i], values[i]);
        }

        auto [selected, execTime, memUsage] = bf.solve();
        int maxProfit = bf.getMaxProfit();
        double executionTime = execTime.count() / 1000000.0;
        
        cout << "Execution Time: " << executionTime << " ms" << endl;
        cout << "Memory Usage: " << memUsage << " bytes" << endl;
        cout << "Max Profit: " << maxProfit << endl;
        cout << "Selected items: ";
        for (const auto& item : selected) {
            cout << item.id << " ";
        }
        cout << endl;
        
        // Store results for GUI
        SolverResult result;
        result.algorithm_name = "Brute Force";
        for (const auto& item : selected) {
            result.selected_items.push_back(item.id);
        }
        result.execution_time_ms = executionTime;
        result.memory_usage = memUsage;
        result.max_profit = maxProfit;
        results.push_back(result);
    }
    
    if (algorithms[2]){
        //dynamic programming
        cout<<"Dynamic Programming Knapsack Algorithm"<<endl;
        cout<<"====================================="<<endl;
        double_knapsack dp = double_knapsack(item_count, weights, values, capacity);
        
        // Capture stdout to avoid double printing
        std::stringstream buffer;
        std::streambuf* old = std::cout.rdbuf(buffer.rdbuf());
        
        dp.solve();
        
        // Restore stdout
        std::cout.rdbuf(old);
        std::string output = buffer.str();
        std::cout << output;  // Print the captured output
        
        // Store results for GUI - using the execution time calculated within dp_knapsack
        SolverResult result;
        result.algorithm_name = "Dynamic Programming";
        result.selected_items = dp.get_selected_items();
        result.max_profit = dp.get_max_profit();
        result.execution_time_ms = dp.get_execution_time_ms(); // Get execution time from dp_knapsack
        result.memory_usage = static_cast<size_t>(dp.get_space_complexity());
        
        // Store the DP table (copy needed since dp is local)
        std::vector<std::vector<int>>* dp_table = new std::vector<std::vector<int>>(dp.get_dp_table());
        result.algorithm_data = dp_table;
        
        results.push_back(result);
    }
    
    if (algorithms[3]) {
        cout << "Branch and Bound Knapsack Algorithm" << endl;
        cout << "===================================" << endl;
        BranchBoundKnapsack bb;
        bb.setCapacity(capacity);
        
        for (int i = 0; i < item_count; i++) {
            bb.addItem(weights[i], values[i]);
        }

        auto [selected, execTime, memUsage] = bb.solve();
        double executionTime = execTime.count() / 1000000.0; // ns to ms
        int maxProfit = bb.getMaxProfit();
        
        cout << "Execution Time: " << executionTime << " ms" << endl;
        cout << "Memory Usage: " << memUsage << " bytes" << endl;
        cout << "Max Profit: " << maxProfit << endl;
        cout << "Selected items: ";
        for (const auto& item : selected) {
            cout << item.id << " ";
        }
        cout << endl;
        
        // Store results for GUI
        SolverResult result;
        result.algorithm_name = "Branch and Bound";
        for (const auto& item : selected) {
            result.selected_items.push_back(item.id);
        }
        result.execution_time_ms = executionTime;
        result.memory_usage = memUsage;
        result.max_profit = maxProfit;
        results.push_back(result);
    }
}

double KnapsackSolver::get_execution_time() {
    return execution_time;
}

double KnapsackSolver::get_space_complexity() {
    return space_complexity;
}

