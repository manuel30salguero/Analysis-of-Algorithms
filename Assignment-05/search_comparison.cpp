
#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <chrono>
#include <iomanip>

using namespace std;
using namespace chrono;

// --------------------------------------------------
// Binary Search
// --------------------------------------------------
int binarySearch(const vector<int>& A, int key)
{
    int left = 0;
    int right = A.size() - 1;

    while (left <= right)
    {
        int middle = left + (right - left) / 2;

        if (A[middle] == key)
            return middle;

        if (key < A[middle])
            right = middle - 1;
        else
            left = middle + 1;
    }

    return -1;
}

// --------------------------------------------------
// Interpolation Search
// --------------------------------------------------
int interpolationSearch(const vector<int>& A, int key)
{
    int left = 0;
    int right = A.size() - 1;

    while (left <= right &&
           key >= A[left] &&
           key <= A[right])
    {
        if (A[left] == A[right])
        {
            if (A[left] == key)
                return left;

            return -1;
        }

        long long position =
            left +
            (static_cast<long long>(key - A[left]) *
             (right - left)) /
            (A[right] - A[left]);

        if (A[position] == key)
            return position;

        if (A[position] < key)
            left = position + 1;
        else
            right = position - 1;
    }

    return -1;
}

// --------------------------------------------------
// Main
// --------------------------------------------------
int main()
{
    const int N = 10000000;
    const int REPETITIONS = 100000;

    vector<int> numbers(N);

    mt19937 generator(42);
    uniform_int_distribution<int> distribution(1, 100000000);

    cout << "Generating " << N << " random numbers..." << endl;

    for (int i = 0; i < N; i++)
    {
        numbers[i] = distribution(generator);
    }

    cout << "Sorting the list..." << endl;
    sort(numbers.begin(), numbers.end());

    // Choose a key that is guaranteed to exist
    int keyIndex = 3 * N / 4;
    int key = numbers[keyIndex];

    cout << "\nKey to search: " << key << endl;
    cout << "Number of repetitions: " << REPETITIONS << endl;

    volatile int binaryIndex = -1;
    volatile int interpolationIndex = -1;

    // --------------------------------------------------
    // Binary Search timing
    // --------------------------------------------------
    auto startBinary = high_resolution_clock::now();

    for (int i = 0; i < REPETITIONS; i++)
    {
        binaryIndex = binarySearch(numbers, key);
    }

    auto endBinary = high_resolution_clock::now();

    // --------------------------------------------------
    // Interpolation Search timing
    // --------------------------------------------------
    auto startInterpolation = high_resolution_clock::now();

    for (int i = 0; i < REPETITIONS; i++)
    {
        interpolationIndex = interpolationSearch(numbers, key);
    }

    auto endInterpolation = high_resolution_clock::now();

    double binaryTotalTime =
        duration<double, nano>(endBinary - startBinary).count();

    double interpolationTotalTime =
        duration<double, nano>(
            endInterpolation - startInterpolation).count();

    double binaryAverage =
        binaryTotalTime / REPETITIONS;

    double interpolationAverage =
        interpolationTotalTime / REPETITIONS;

    cout << fixed << setprecision(2);

    cout << "\nBINARY SEARCH" << endl;
    cout << "Index found: " << binaryIndex << endl;
    cout << "Average execution time: "
         << binaryAverage << " ns" << endl;

    cout << "\nINTERPOLATION SEARCH" << endl;
    cout << "Index found: " << interpolationIndex << endl;
    cout << "Average execution time: "
         << interpolationAverage << " ns" << endl;

    return 0;
}
