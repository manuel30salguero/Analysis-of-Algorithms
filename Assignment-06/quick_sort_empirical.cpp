#include <algorithm>
#include <chrono>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <random>
#include <vector>
#include "quick_sort.hpp"
using namespace std;
using namespace chrono;

struct Result { double ms = 0, comparisons = 0; };

Result measure(int n, bool sortedInput, mt19937& rng) {
    const int REPEATS = 5;
    uniform_int_distribution<int> dist(1, 1000000000);
    Result res;
    for (int rep = 0; rep < REPEATS; ++rep) {
        vector<int> A(n);
        for (int& x : A) x = dist(rng);
        if (sortedInput) sort(A.begin(), A.end());

        long long c = 0;
        auto start = steady_clock::now();
        quickSort(A, 0, n - 1, &c);
        auto end = steady_clock::now();
        if (!is_sorted(A.begin(), A.end())) return {-1, -1};
        res.ms += duration<double, milli>(end - start).count();
        res.comparisons += c;
    }
    res.ms /= REPEATS;
    res.comparisons /= REPEATS;
    return res;
}

int main() {
    mt19937 rng(2026);
    cout << fixed << setprecision(3);
    cout << "Five repetitions per size and input type.\n";
    cout << "n    | Random: comp  C/(nlog2n)   ms | Sorted: comp   C/n^2    ms\n";
    cout << string(73, '-') << "\n";
    for (int n : {500, 1000, 2000, 4000, 8000}) {
        Result random = measure(n, false, rng);
        Result sorted = measure(n, true, rng);
        if (random.ms < 0 || sorted.ms < 0) return 1;
        cout << setw(5) << n << " | "
             << setw(12) << (long long)llround(random.comparisons) << " "
             << setw(10) << random.comparisons / (n * log2(n)) << " "
             << setw(6) << random.ms << " | "
             << setw(12) << (long long)llround(sorted.comparisons) << " "
             << setw(7) << sorted.comparisons / (double(n) * n) << " "
             << setw(6) << sorted.ms << "\n";
    }
}
