#include <cmath>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <vector>
using namespace std;

using Matrix = vector<vector<double>>;

// Forward elimination with partial pivoting.
void forwardElimination(Matrix& A) {
    int n = (int)A.size();
    for (int k = 0; k < n - 1; ++k) {
        int p = k;
        for (int i = k + 1; i < n; ++i)
            if (abs(A[i][k]) > abs(A[p][k])) p = i;
        if (abs(A[p][k]) < 1e-12) throw runtime_error("Singular matrix");
        swap(A[k], A[p]);

        for (int i = k + 1; i < n; ++i) {
            double factor = A[i][k] / A[k][k];
            A[i][k] = 0.0;
            for (int j = k + 1; j <= n; ++j)
                A[i][j] -= factor * A[k][j];
        }
    }
    if (abs(A[n - 1][n - 1]) < 1e-12)
        throw runtime_error("Singular matrix");
}

void printMatrix(const Matrix& A) {
    for (const auto& row : A) {
        for (int j = 0; j < (int)row.size(); ++j) {
            if (j == (int)row.size() - 1) cout << " |";
            cout << setw(9) << row[j];
        }
        cout << '\n';
    }
}

int main() {
    Matrix A = {{2, -4, 1, 6}, {3, -1, 1, 11}, {1, 1, -1, -3}};
    cout << fixed << setprecision(3);
    cout << "Original augmented matrix:\n";
    printMatrix(A);
    forwardElimination(A);
    cout << "\nUpper-triangular augmented matrix:\n";
    printMatrix(A);
    return 0;
}
