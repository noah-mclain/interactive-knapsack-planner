#include "../include/knapsack/knapsack_solver.hpp"
#include "../include/knapsack/branchbound.hpp"
#include <iostream>
#include <chrono>

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
        double executionTime = execTime.count() /  1000000.0; // ns to ms
        cout << "Execution Time: " << executionTime<< " ms" << endl;
        cout << "Memory Usage: " << memUsage << " bytes" << endl;
        cout << "Max Profit: " << maxProfit << endl;
        cout << "Selected items: ";
        for (const auto& item : selected) {
            cout << item.id << " ";
        }
        cout << endl;
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
        double executionTime = execTime.count() /  1000000.0;
        cout << "Execution Time: " << executionTime << " ms" << endl;
        cout << "Memory Usage: " << memUsage << " bytes" << endl;
        cout << "Max Profit: " << maxProfit << endl;
        cout << "Selected items: ";
        for (const auto& item : selected) {
            cout << item.id << " ";
        }
        cout << endl;
    }
    if (algorithms[2]){
        //dynamic programming
        cout<<"Dynamic Programming Knapsack Algorithm"<<endl;
        cout<<"====================================="<<endl;
        double_knapsack dp = double_knapsack(item_count, weights, values, capacity);
        dp.solve();
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
        double executionTime = execTime.count() / 1000000.0;
        cout << "Execution Time: " << executionTime << " ms" << endl;
        cout << "Memory Usage: " << memUsage << " bytes" << endl;
        cout << "Max Profit: " << bb.getMaxProfit() << endl;
        cout << "Selected items: ";
        for (const auto& item : selected) {
            cout << item.id << " ";
        }
        cout << endl;
    }
}

