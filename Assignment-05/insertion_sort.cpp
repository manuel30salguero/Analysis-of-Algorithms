
#include <iostream>
#include <vector>
#include <random>

using namespace std;

// Insertion Sort
void insertionSort(vector<char>& A)
{
    int n = A.size();

    for (int i = 1; i < n; i++)
    {
        char v = A[i];
        int j = i - 1;

        while (j >= 0 && A[j] > v)
        {
            A[j + 1] = A[j];
            j--;
        }

        A[j + 1] = v;
    }
}

// Print array
void printArray(const vector<char>& A)
{
    for (char c : A)
    {
        cout << c << " ";
    }

    cout << endl;
}

int main()
{
    int n;

    cout << "Enter the number of characters: ";
    cin >> n;

    vector<char> A(n);

    random_device rd;
    mt19937 generator(rd());
    uniform_int_distribution<int> distribution('A', 'Z');

    // Generate random uppercase letters
    for (int i = 0; i < n; i++)
    {
        A[i] = static_cast<char>(distribution(generator));
    }

    cout << "\nOriginal array:\n";
    printArray(A);

    insertionSort(A);

    cout << "\nSorted array:\n";
    printArray(A);

    return 0;
}
