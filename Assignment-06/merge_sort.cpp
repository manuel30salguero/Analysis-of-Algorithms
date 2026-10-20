#include <algorithm>
#include <iostream>
#include <random>
#include <vector>
using namespace std;

void merge(vector<int>& A, int left, int mid, int right) {
    vector<int> temp;
    int i = left, j = mid + 1;
    while (i <= mid && j <= right) {
        if (A[i] <= A[j]) temp.push_back(A[i++]);
        else temp.push_back(A[j++]);
    }
    while (i <= mid) temp.push_back(A[i++]);
    while (j <= right) temp.push_back(A[j++]);
    for (int k = 0; k < (int)temp.size(); ++k)
        A[left + k] = temp[k];
}

void mergeSort(vector<int>& A, int left, int right) {
    if (left >= right) return;
    int mid = left + (right - left) / 2;
    mergeSort(A, left, mid);
    mergeSort(A, mid + 1, right);
    merge(A, left, mid, right);
}

void printArray(const vector<int>& A) {
    for (int x : A) cout << x << " ";
    cout << "\n";
}

int main() {
    mt19937 rng(2026);
    uniform_int_distribution<int> dist(1, 99);
    vector<int> A(20);
    for (int& x : A) x = dist(rng);
    cout << "Original list: "; printArray(A);
    mergeSort(A, 0, (int)A.size() - 1);
    cout << "Sorted list:   "; printArray(A);
    cout << "Correctly sorted: "
         << (is_sorted(A.begin(), A.end()) ? "YES" : "NO") << "\n";
}
