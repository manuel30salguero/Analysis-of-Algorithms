#include <algorithm>
#include <iostream>
#include <map>
#include <string>
#include <vector>
using namespace std;

struct Item {
    int key;
    string id;
};

void merge(vector<Item>& A, int left, int mid, int right) {
    vector<Item> temp;
    int i = left, j = mid + 1;
    while (i <= mid && j <= right) {
        if (A[i].key <= A[j].key) temp.push_back(A[i++]);
        else temp.push_back(A[j++]);
    }
    while (i <= mid) temp.push_back(A[i++]);
    while (j <= right) temp.push_back(A[j++]);
    for (int k = 0; k < (int)temp.size(); ++k)
        A[left + k] = temp[k];
}

void mergeSort(vector<Item>& A, int left, int right) {
    if (left >= right) return;
    int mid = left + (right - left) / 2;
    mergeSort(A, left, mid);
    mergeSort(A, mid + 1, right);
    merge(A, left, mid, right);
}

void printRecords(const vector<Item>& A) {
    for (const Item& x : A)
        cout << "(" << x.key << "," << x.id << ") ";
    cout << "\n";
}

map<int, vector<string>> labelsByKey(const vector<Item>& A) {
    map<int, vector<string>> result;
    for (const Item& x : A) result[x.key].push_back(x.id);
    return result;
}

int main() {
    vector<Item> A = {
        {4,"A"}, {2,"B"}, {4,"C"}, {3,"D"},
        {2,"E"}, {4,"F"}, {2,"G"}
    };
    auto before = labelsByKey(A);
    cout << "Original records: "; printRecords(A);
    mergeSort(A, 0, (int)A.size() - 1);
    cout << "Sorted records:   "; printRecords(A);
    bool stable = is_sorted(A.begin(), A.end(),
        [](const Item& x, const Item& y) { return x.key < y.key; })
        && before == labelsByKey(A);
    cout << "Stable: " << (stable ? "YES" : "NO") << "\n";
    return stable ? 0 : 1;
}
