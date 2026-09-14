
#include <iostream>
#include <vector>
#include <random>
#include <numeric>
#include <chrono>
#include <iomanip>
#include <algorithm>

using namespace std;
using namespace chrono;

// --------------------------------------------------
// Exhaustive Search for 0/1 Knapsack
// --------------------------------------------------
int exhaustiveKnapsack(
    const vector<int>& weights,
    const vector<int>& values,
    int capacity,
    int index,
    int currentWeight,
    int currentValue)
{
    // A complete subset has been generated
    if (index == static_cast<int>(weights.size()))
    {
        if (currentWeight <= capacity)
            return currentValue;

        return 0;
    }

    // Option 1: do not include the current item
    int withoutItem = exhaustiveKnapsack(
        weights,
        values,
        capacity,
        index + 1,
        currentWeight,
        currentValue
    );

    // Option 2: include the current item
    int withItem = exhaustiveKnapsack(
        weights,
        values,
        capacity,
        index + 1,
        currentWeight + weights[index],
        currentValue + values[index]
    );

    return max(withoutItem, withItem);
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
// Main empirical experiment
// --------------------------------------------------
int main()
{
    const double TIME_LIMIT = 30.0;

    mt19937 generator(42);

    int largestN = 0;

    cout << "0/1 KNAPSACK - EMPIRICAL ANALYSIS\n\n";

    cout << left
         << setw(6)  << "n"
         << setw(20) << "Subsets"
         << setw(18) << "Best Value"
         << setw(22) << "Execution Time (s)"
         << endl;

    cout << fixed << setprecision(8);

    for (int n = 3; ; n++)
    {
        vector<int> weights;
        vector<int> values;
        int capacity;

        generateRandomInstance(
            n,
            weights,
            values,
            capacity,
            generator
        );

        auto start = high_resolution_clock::now();

        int bestValue = exhaustiveKnapsack(
            weights,
            values,
            capacity,
            0,
            0,
            0
        );

        auto finish = high_resolution_clock::now();

        double executionTime =
            duration<double>(finish - start).count();

        unsigned long long subsets = 1ULL << n;

        cout << left
             << setw(6)  << n
             << setw(20) << subsets
             << setw(18) << bestValue
             << setw(22) << executionTime
             << endl;

        if (executionTime <= TIME_LIMIT)
        {
            largestN = n;
        }
        else
        {
            break;
        }
    }

    cout << "\nLargest problem size solved within 30 seconds: n = "
         << largestN << endl;

    return 0;
}
