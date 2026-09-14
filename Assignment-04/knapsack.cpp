
#include <iostream>
#include <vector>
#include <random>
#include <numeric>
#include <iomanip>

using namespace std;

// --------------------------------------------------
// Structure to store the result
// --------------------------------------------------
struct KnapsackResult
{
    int maxValue;
    int totalWeight;
    vector<int> selectedItems;
};

// --------------------------------------------------
// Exhaustive Search for 0/1 Knapsack
// --------------------------------------------------
KnapsackResult exhaustiveKnapsack(
    const vector<int>& weights,
    const vector<int>& values,
    int capacity)
{
    int n = weights.size();

    int bestValue = 0;
    int bestWeight = 0;
    vector<int> bestItems;

    unsigned long long totalSubsets = 1ULL << n;

    for (unsigned long long mask = 0; mask < totalSubsets; mask++)
    {
        int currentWeight = 0;
        int currentValue = 0;
        vector<int> currentItems;

        for (int i = 0; i < n; i++)
        {
            if (mask & (1ULL << i))
            {
                currentWeight += weights[i];
                currentValue += values[i];
                currentItems.push_back(i);
            }
        }

        if (currentWeight <= capacity &&
            currentValue > bestValue)
        {
            bestValue = currentValue;
            bestWeight = currentWeight;
            bestItems = currentItems;
        }
    }

    return {bestValue, bestWeight, bestItems};
}

// --------------------------------------------------
// Generate a random problem instance
// --------------------------------------------------
void generateRandomInstance(
    int n,
    vector<int>& weights,
    vector<int>& values,
    int& capacity,
    mt19937& generator)
{
    uniform_int_distribution<int> weightDistribution(1, 20);
    uniform_int_distribution<int> valueDistribution(1, 100);

    weights.clear();
    values.clear();

    for (int i = 0; i < n; i++)
    {
        weights.push_back(weightDistribution(generator));
        values.push_back(valueDistribution(generator));
    }

    int totalWeight =
        accumulate(weights.begin(), weights.end(), 0);

    uniform_int_distribution<int> capacityDistribution(
        max(1, totalWeight / 4),
        max(1, totalWeight / 2)
    );

    capacity = capacityDistribution(generator);
}

// --------------------------------------------------
// Main
// --------------------------------------------------
int main()
{
    const int n = 5;

    vector<int> weights;
    vector<int> values;
    int capacity;

    mt19937 generator(42);

    generateRandomInstance(
        n,
        weights,
        values,
        capacity,
        generator
    );

    cout << "0/1 KNAPSACK - EXHAUSTIVE SEARCH\n\n";

    cout << "Number of items: " << n << endl;
    cout << "Capacity: " << capacity << "\n\n";

    cout << left
         << setw(8) << "Item"
         << setw(10) << "Weight"
         << setw(10) << "Value"
         << endl;

    for (int i = 0; i < n; i++)
    {
        cout << left
             << setw(8) << i
             << setw(10) << weights[i]
             << setw(10) << values[i]
             << endl;
    }

    KnapsackResult result =
        exhaustiveKnapsack(weights, values, capacity);

    cout << "\nBest value: "
         << result.maxValue << endl;

    cout << "Total weight: "
         << result.totalWeight << endl;

    cout << "Selected items: ";

    for (int item : result.selectedItems)
        cout << item << " ";

    cout << endl;

    return 0;
}
