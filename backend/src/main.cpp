#include "../include/knapsack/knapsack_solver.hpp"
#include <iostream>
#include <vector>

using namespace std;
using namespace knapsack;

int main() {
    // Example input
    int capacity = 50;
    vector<int> weights = {10, 20, 30};
    vector<int> values = {60, 100, 120};
    int n = weights.size();

    // Algorithms to run:
    // greedy, brute-force, dynamic programming, branch & bound
    bool algorithms[4] = {true, true, true, true};

    KnapsackSolver solver(n, weights, values, capacity, algorithms);
    solver.solve();

    return 0;
}