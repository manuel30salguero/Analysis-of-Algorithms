
#include <iostream>
#include <vector>
#include <chrono>
#include <iomanip>
#include <algorithm>

using namespace std;
using namespace chrono;


// --------------------------------------------------
// Linear Congruential Method
// --------------------------------------------------
vector<long long> linearCongruential(
    long long n,
    long long seed,
    long long a,
    long long b,
    long long m)
{
    vector<long long> numbers;
    numbers.reserve(n);

    long long r = seed;

    for (long long i = 0; i < n; i++)
    {
        r = (a * r + b) % m;
        numbers.push_back(r);
    }

    return numbers;
}


// --------------------------------------------------
// Count basic operations
// --------------------------------------------------
long long countBasicOperations(
    long long n,
    long long seed,
    long long a,
    long long b,
    long long m)
{
    long long r = seed;
    long long counter = 0;

    for (long long i = 0; i < n; i++)
    {
        r = (a * r + b) % m;
        counter++;
    }

    return counter;
}


// --------------------------------------------------
// Median execution time
// --------------------------------------------------
double medianExecutionTime(
    long long n,
    long long seed,
    long long a,
    long long b,
    long long m,
    int repetitions = 7)
{
    vector<double> times;

    // Warm-up
    linearCongruential(n, seed, a, b, m);

    for (int i = 0; i < repetitions; i++)
    {
        auto start = high_resolution_clock::now();

        linearCongruential(n, seed, a, b, m);

        auto finish = high_resolution_clock::now();

        double elapsed =
            duration<double>(finish - start).count();

        times.push_back(elapsed);
    }

    sort(times.begin(), times.end());

    return times[times.size() / 2];
}


// --------------------------------------------------
// Main empirical experiment
// --------------------------------------------------
int main()
{
    long long seed = 7;
    long long a = 17;
    long long b = 1;
    long long m = 100;

    // First, show a sample sequence
    cout << "LINEAR CONGRUENTIAL METHOD\n\n";

    cout << "Sample sequence (n = 10):\n";

    vector<long long> sample =
        linearCongruential(10, seed, a, b, m);

    for (long long value : sample)
        cout << value << " ";

    cout << "\n\n";

    // Empirical analysis
    vector<long long> nValues = {
    100000,
    200000,
    400000,
    800000,
    1600000,
    3200000,
    6400000
};

    cout << "EMPIRICAL ANALYSIS\n\n";

    cout << fixed << setprecision(8);

    cout << left
         << setw(12) << "n"
         << setw(22) << "Basic Operations"
         << setw(24) << "Execution Time (s)"
         << setw(22) << "T(2n)/T(n)"
         << endl;

    double previousTime = 0.0;

    for (long long n : nValues)
    {
        long long operations =
            countBasicOperations(
                n, seed, a, b, m
            );

        double executionTime =
            medianExecutionTime(
                n, seed, a, b, m
            );

        cout << left
             << setw(12) << n
             << setw(22) << operations
             << setw(24) << executionTime;

        if (previousTime == 0.0)
        {
            cout << setw(22) << "N/A";
        }
        else
        {
            double ratio =
                executionTime / previousTime;

            cout << setw(22) << ratio;
        }

        cout << endl;

        previousTime = executionTime;
    }

    return 0;
}
