#include <algorithm>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <stdexcept>
#include <vector>
using namespace std;

using Matrix = vector<vector<double>>;

struct LUResult {
    Matrix L, U;
    vector<int> permutation;
};

// Gaussian elimination stores each multiplier in L and produces U.
// Row pivoting gives P*A = L*U.
LUResult luDecomposition(const Matrix& A) {
    int n = (int)A.size();
    Matrix L(n, vector<double>(n, 0.0)), U = A;
    vector<int> p(n);
    iota(p.begin(), p.end(), 0);
    for (int i = 0; i < n; ++i) L[i][i] = 1.0;

    for (int k = 0; k < n; ++k) {
        int pivot = k;
        for (int i = k + 1; i < n; ++i)
            if (abs(U[i][k]) > abs(U[pivot][k])) pivot = i;
        if (abs(U[pivot][k]) < 1e-12) throw runtime_error("Singular matrix");
        if (pivot != k) {
            swap(U[pivot], U[k]);
            swap(p[pivot], p[k]);
            for (int j = 0; j < k; ++j) swap(L[pivot][j], L[k][j]);
        }
        for (int i = k + 1; i < n; ++i) {
            double factor = U[i][k] / U[k][k];
            L[i][k] = factor;
            U[i][k] = 0.0;
            for (int j = k + 1; j < n; ++j)
                U[i][j] -= factor * U[k][j];
        }
    }
    return {L, U, p};
}

vector<double> solveLU(const LUResult& lu, const vector<double>& b) {
    int n = (int)b.size();
    vector<double> y(n), x(n);
    // Forward substitution: L*y = P*b.
    for (int i = 0; i < n; ++i) {
        double sum = 0.0;
        for (int j = 0; j < i; ++j) sum += lu.L[i][j] * y[j];
        y[i] = b[lu.permutation[i]] - sum;
    }
    // Backward substitution: U*x = y.
    for (int i = n - 1; i >= 0; --i) {
        double sum = 0.0;
        for (int j = i + 1; j < n; ++j) sum += lu.U[i][j] * x[j];
        x[i] = (y[i] - sum) / lu.U[i][i];
    }
    return x;
}

void printMatrix(const Matrix& A) {
    for (const auto& row : A) {
        for (double x : row) cout << setw(10) << x;
        cout << '\n';
    }
}

int main() {
    Matrix A = {{2, -4, 1}, {3, -1, 1}, {1, 1, -1}};
    vector<double> b = {6, 11, -3};
    LUResult lu = luDecomposition(A);
    vector<double> x = solveLU(lu, b);
    double maxResidual = 0.0, maxFactorizationError = 0.0;
    for (int i = 0; i < (int)b.size(); ++i) {
        double ax = 0.0;
        for (int j = 0; j < (int)b.size(); ++j) {
            ax += A[i][j] * x[j];
            double product = 0.0;
            for (int k = 0; k < (int)b.size(); ++k)
                product += lu.L[i][k] * lu.U[k][j];
            maxFactorizationError = max(maxFactorizationError,
                abs(A[lu.permutation[i]][j] - product));
        }
        maxResidual = max(maxResidual, abs(ax - b[i]));
    }
    cout << fixed << setprecision(3);
    cout << "L:\n"; printMatrix(lu.L);
    cout << "U:\n"; printMatrix(lu.U);
    cout << "Solution of Ax = b:\n";
    for (int i = 0; i < (int)x.size(); ++i)
        cout << "x" << i + 1 << " = " << x[i] << '\n';
    cout << "Maximum |PA - LU|: " << maxFactorizationError << '\n';
    cout << "Maximum |Ax - b|: " << maxResidual << '\n';
    bool correct = maxResidual < 1e-9 && maxFactorizationError < 1e-9;
    cout << "Verified: " << (correct ? "YES" : "NO") << '\n';
    return correct ? 0 : 1;
}
