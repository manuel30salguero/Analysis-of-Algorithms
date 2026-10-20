#include <iostream>
#include <random>
#include <utility>
#include <vector>
using namespace std;

void siftDown(vector<int>& A, int i, int n) {
    while (2 * i + 1 < n) {
        int child = 2 * i + 1;
        if (child + 1 < n && A[child + 1] > A[child]) ++child;
        if (A[i] >= A[child]) break;
        swap(A[i], A[child]);
        i = child;
    }
}

void buildMaxHeap(vector<int>& A) {
    for (int i = (int)A.size() / 2 - 1; i >= 0; --i)
        siftDown(A, i, (int)A.size());
}

bool isMaxHeap(const vector<int>& A) {
    for (int i = 1; i < (int)A.size(); ++i)
        if (A[(i - 1) / 2] < A[i]) return false;
    return true;
}

void printArray(const vector<int>& A) {
    for (int x : A) cout << x << ' ';
    cout << '\n';
}

int main() {
    mt19937 rng(2026);
    uniform_int_distribution<int> dist(1, 99);
    vector<int> A(20);
    for (int& x : A) x = dist(rng);
    cout << "Original random array:\n";
    printArray(A);
    buildMaxHeap(A);
    cout << "Max-heap array:\n";
    printArray(A);
    cout << "Valid max-heap: " << (isMaxHeap(A) ? "YES" : "NO") << '\n';
    return isMaxHeap(A) ? 0 : 1;
}
