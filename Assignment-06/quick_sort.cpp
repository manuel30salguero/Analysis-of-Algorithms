#include <algorithm>
#include <iostream>
#include <random>
#include <vector>
#include "quick_sort.hpp"
using namespace std;

void printArray(const vector<int>& A) {
    for (int x : A) cout << x << " ";
    cout << "\n";
}

int main() {
    mt19937 rng(2026);
    uniform_int_distribution<int> dist(1, 99);
    for (int test = 1; test <= 3; ++test) {
        vector<int> A(15);
        for (int& x : A) x = dist(rng);
        vector<int> expected = A;
        sort(expected.begin(), expected.end());
        cout << "Test " << test << "\nOriginal: "; printArray(A);
        quickSort(A, 0, (int)A.size() - 1);
        cout << "Sorted:   "; printArray(A);
        cout << "Correct: " << (A == expected ? "YES" : "NO") << "\n\n";
        if (A != expected) return 1;
    }
}
