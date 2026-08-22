
#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

// Sieve of Eratosthenes
vector<int> sieve(int n) {
    vector<bool> isPrime(n + 1, true);
    vector<int> primes;

    if (n >= 0) isPrime[0] = false;
    if (n >= 1) isPrime[1] = false;

    for (int p = 2; p * p <= n; p++) {
        if (isPrime[p]) {
            for (int j = p * p; j <= n; j += p) {
                isPrime[j] = false;
            }
        }
    }

    for (int p = 2; p <= n; p++) {
        if (isPrime[p]) {
            primes.push_back(p);
        }
    }

    return primes;
}

// Prime factorization
vector<int> primeFactors(int n, const vector<int>& primes) {
    vector<int> factors;

    for (int p : primes) {
        while (n % p == 0) {
            factors.push_back(p);
            n /= p;
        }

        if (n == 1)
            break;
    }

    return factors;
}

// Middle-School Procedure
int gcdMiddleSchool(int m, int n) {
    if (m == 1 || n == 1)
        return 1;

    int limit = max(m, n);

    vector<int> primes = sieve(limit);
    vector<int> factorsM = primeFactors(m, primes);
    vector<int> factorsN = primeFactors(n, primes);

    int gcd = 1;
    int i = 0;
    int j = 0;

    while (i < factorsM.size() && j < factorsN.size()) {
        if (factorsM[i] == factorsN[j]) {
            gcd *= factorsM[i];
            i++;
            j++;
        }
        else if (factorsM[i] < factorsN[j]) {
            i++;
        }
        else {
            j++;
        }
    }

    return gcd;
}

int main() {
    int m, n;

    cout << "Enter two positive integers: ";
    cin >> m >> n;

    cout << "GCD = " << gcdMiddleSchool(m, n) << endl;

    return 0;
}
