#include <iostream>
#include "include/knapsack/dp_knapsack.hpp"

using namespace std;

int main(){
    vector<int> weights = {2, 3, 4, 5, 9};
    vector<int> values = {5, 5, 5, 5, 10};
    int capacity = 10;
    double_knapsack knapsack(5, weights, values, capacity);
    knapsack.solve();
    cout << "Max Profit: " << knapsack.get_max_profit() << endl;
    cout << "Selected Items: ";
    vector<int> selected_items = knapsack.get_selected_items();
    for (int i : selected_items) {
        cout << i << " ";
    }
    cout << endl;
    cout << "Space Complexity: " << knapsack.get_space_complexity() << " bytes" << endl;
    return 0;
}