#include <cmath>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <vector>
using namespace std;

vector<double> backwardSubstitution(const vector<vector<double>>& U,
                                    const vector<double>& b) {
    int n = (int)b.size();
    vector<double> x(n, 0.0);
    for (int i = n - 1; i >= 0; --i) {
        if (abs(U[i][i]) < 1e-12) throw runtime_error("Zero pivot");
        double sum = 0.0;
        for (int j = i + 1; j < n; ++j)
            sum += U[i][j] * x[j];
        x[i] = (b[i] - sum) / U[i][i];
    }
    return x;
}

int main() {
    // Upper-triangular system equivalent to the example in L13.
    vector<vector<double>> U = {{2, -4, 1}, {0, 5, -0.5}, {0, 0, -1.2}};
    vector<double> b = {6, 2, -7.2};
    vector<double> x = backwardSubstitution(U, b);
    cout << fixed << setprecision(3);
    cout << "Solution by backward substitution:\n";
    for (int i = 0; i < (int)x.size(); ++i)
        cout << "x" << i + 1 << " = " << x[i] << '\n';
    bool correct = abs(x[0] - 2) < 1e-9 && abs(x[1] - 1) < 1e-9
                   && abs(x[2] - 6) < 1e-9;
    cout << "Correct solution: " << (correct ? "YES" : "NO") << '\n';
    return correct ? 0 : 1;
}
