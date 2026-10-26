#include <algorithm>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <random>
#include <vector>
using namespace std;
using namespace chrono;

// Same comparison-counting method as Problem 2.
vector<int> comparisonCountingSort(const vector<int>& A) {
    int n = (int)A.size();
    vector<int> count(n, 0), result(n);
    for (int i = 0; i < n - 1; ++i)
        for (int j = i + 1; j < n; ++j)
            if (A[i] < A[j]) ++count[j];
            else ++count[i];
    for (int i = 0; i < n; ++i) result[count[i]] = A[i];
    return result;
}

// Frequency counts and cumulative counts; also handles negative integers.
vector<int> distributionCountingSort(const vector<int>& A) {
    if (A.empty()) return {};
    auto bounds = minmax_element(A.begin(), A.end());
    int minimum = *bounds.first, maximum = *bounds.second;
    int k = maximum - minimum + 1;
    vector<int> count(k, 0), result(A.size());
    for (int x : A) ++count[x - minimum];
    for (int i = 1; i < k; ++i) count[i] += count[i - 1];
    for (int i = (int)A.size() - 1; i >= 0; --i)
        result[--count[A[i] - minimum]] = A[i];
    return result;
}

void printArray(const vector<int>& A) {
    for (int x : A) cout << x << ' ';
    cout << '\n';
}

bool test(const vector<int>& A, const char* name) {
    vector<int> expected = A;
    sort(expected.begin(), expected.end());
    vector<int> result = distributionCountingSort(A);
    cout << name << "\nOriginal: "; printArray(A);
    cout << "Sorted:   "; printArray(result);
    cout << "Correct: " << (result == expected ? "YES" : "NO") << "\n\n";
    return result == expected;
}

int main() {
    bool ok = test({62, 31, 84, 96, 19, 47}, "Test 1: textbook example");
    ok = test({4, 2, 4, 3, 2, -1, 4}, "Test 2: repeated and negative keys") && ok;

    mt19937 rng(2026);
    uniform_int_distribution<int> dist(-50, 50);
    const int repeats = 5;
    cout << "Average times (ms), five repetitions; key range [-50, 50]:\n";
    cout << setw(7) << "n" << setw(22) << "Comparison" << setw(22)
         << "Distribution" << "  Correct\n";
    cout << fixed << setprecision(4);
    for (int n : {250, 500, 1000, 2000, 4000}) {
        vector<int> A(n);
        for (int& x : A) x = dist(rng);
        double comparisonMs = 0, distributionMs = 0;
        for (int rep = 0; rep < repeats; ++rep) {
            auto begin = steady_clock::now();
            vector<int> B = comparisonCountingSort(A);
            auto end = steady_clock::now();
            comparisonMs += duration<double, milli>(end - begin).count();

            begin = steady_clock::now();
            vector<int> C = distributionCountingSort(A);
            end = steady_clock::now();
            distributionMs += duration<double, milli>(end - begin).count();
            if (B != C || !is_sorted(B.begin(), B.end())) ok = false;
        }
        cout << setw(7) << n << setw(22) << comparisonMs / repeats
             << setw(22) << distributionMs / repeats
             << "  " << (ok ? "YES" : "NO") << '\n';
    }
    return ok ? 0 : 1;
}
