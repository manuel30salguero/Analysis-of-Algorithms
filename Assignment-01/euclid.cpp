
#include <iostream>
using namespace std;

int gcdEuclid(int m, int n) {
    while (n != 0) {
        int r = m % n;
        m = n;
        n = r;
    }

    return m;
}

int main() {
    int m, n;

    cout << "Enter two nonnegative integers: ";
    cin >> m >> n;

    cout << "GCD = " << gcdEuclid(m, n) << endl;

    return 0;
}
