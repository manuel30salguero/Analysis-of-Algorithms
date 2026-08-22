
#include <iostream>
#include <algorithm>
using namespace std;

int gcdConsecutive(int m, int n) {
    int t = min(m, n);

    while (true) {
        if (m % t == 0) {
            if (n % t == 0) {
                return t;
            }
        }

        t--;
    }
}

int main() {
    int m, n;

    cout << "Enter two positive integers: ";
    cin >> m >> n;

    cout << "GCD = " << gcdConsecutive(m, n) << endl;

    return 0;
}
