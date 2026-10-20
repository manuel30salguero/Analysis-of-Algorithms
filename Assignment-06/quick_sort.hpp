#ifndef QUICK_SORT_HPP
#define QUICK_SORT_HPP

#include <algorithm>
#include <vector>

// First-element pivot; count key comparisons when requested.
inline int partition(std::vector<int>& A, int l, int r,
                     long long* comparisons = nullptr) {
    int pivot = A[l], i = l + 1, j = r;
    while (true) {
        while (i <= r) {
            if (comparisons) ++*comparisons;
            if (A[i] >= pivot) break;
            ++i;
        }
        while (j > l) {
            if (comparisons) ++*comparisons;
            if (A[j] <= pivot) break;
            --j;
        }
        if (i >= j) break;
        std::swap(A[i++], A[j--]);
    }
    std::swap(A[l], A[j]);
    return j;
}

inline void quickSort(std::vector<int>& A, int l, int r,
                      long long* comparisons = nullptr) {
    if (l >= r) return;
    int p = partition(A, l, r, comparisons);
    quickSort(A, l, p - 1, comparisons);
    quickSort(A, p + 1, r, comparisons);
}
#endif
