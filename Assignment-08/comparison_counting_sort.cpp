#include <algorithm>
#include <iostream>
#include <random>
#include <vector>
using namespace std;

// Comparison Counting Sort from L15 (ties follow the else branch).
vector<int> comparisonCountingSort(const vector<int>& A) {
    int n = (int)A.size();
    vector<int> count(n, 0), sorted(n);
    for (int i = 0; i < n - 1; ++i) {
        for (int j = i + 1; j < n; ++j) {
            if (A[i] < A[j]) ++count[j];  // Same tie rule as the lecture.
            else ++count[i];
        }
    }
    for (int i = 0; i < n; ++i) sorted[count[i]] = A[i];
    return sorted;
}

void printArray(const vector<int>& A) {
    for (int x : A) cout << x << ' ';
    cout << '\n';
}

bool test(const vector<int>& A, const char* name) {
    vector<int> result = comparisonCountingSort(A);
    vector<int> expected = A;
    sort(expected.begin(), expected.end());
    cout << name << "\nOriginal: "; printArray(A);
    cout << "Sorted:   "; printArray(result);
    cout << "Correct: " << (result == expected ? "YES" : "NO") << "\n\n";
    return result == expected;
}

int main() {
    mt19937 rng(2026);
    uniform_int_distribution<int> dist(-20, 20);
    vector<int> randomList(15);
    for (int& x : randomList) x = dist(rng);

    bool ok = test({62, 31, 84, 96, 19, 47}, "Test 1: textbook example");
    ok = test({4, 2, 4, 3, 2, -1, 4}, "Test 2: repeated and negative keys") && ok;
    ok = test(randomList, "Test 3: random keys") && ok;
    return ok ? 0 : 1;
}
