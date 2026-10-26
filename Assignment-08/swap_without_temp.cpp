#include <iostream>
using namespace std;

// XOR exchanges two integer values without a temporary variable.
void swapWithoutTemp(int& u, int& v) {
    if (&u == &v) return;
    u ^= v;
    v ^= u;
    u ^= v;
}

int main() {
    int u = 12, v = -7;
    cout << "Before: u = " << u << ", v = " << v << '\n';
    swapWithoutTemp(u, v);
    cout << "After:  u = " << u << ", v = " << v << '\n';

    int a = 0, b = 45;
    swapWithoutTemp(a, b);
    int c = 8, d = 8;
    swapWithoutTemp(c, d);
    bool correct = (u == -7 && v == 12 && a == 45 && b == 0 && c == 8 && d == 8);
    cout << "Additional tests: " << (correct ? "PASSED" : "FAILED") << '\n';
    return correct ? 0 : 1;
}
