
#include <iostream>
#include <vector>
#include <cmath>

using namespace std;

// --------------------------------------------------
// Recursive Towers of Hanoi
// --------------------------------------------------
long long hanoiRecursive(int n, char source, char auxiliary, char target)
{
    if (n == 1)
    {
        cout << "Move disk 1 from " << source
             << " to " << target << endl;

        return 1;
    }

    long long moves = 0;

    moves += hanoiRecursive(n - 1, source, target, auxiliary);

    cout << "Move disk " << n
         << " from " << source
         << " to " << target << endl;

    moves += 1;

    moves += hanoiRecursive(n - 1, auxiliary, source, target);

    return moves;
}


// --------------------------------------------------
// Perform one legal move between two rods
// --------------------------------------------------
void moveBetweenRods(vector<int>& rod1,
                     vector<int>& rod2,
                     char name1,
                     char name2)
{
    // rod1 is empty
    if (rod1.empty())
    {
        int disk = rod2.back();
        rod2.pop_back();
        rod1.push_back(disk);

        cout << "Move disk " << disk
             << " from " << name2
             << " to " << name1 << endl;
    }

    // rod2 is empty
    else if (rod2.empty())
    {
        int disk = rod1.back();
        rod1.pop_back();
        rod2.push_back(disk);

        cout << "Move disk " << disk
             << " from " << name1
             << " to " << name2 << endl;
    }

    // Top disk of rod1 is smaller
    else if (rod1.back() < rod2.back())
    {
        int disk = rod1.back();
        rod1.pop_back();
        rod2.push_back(disk);

        cout << "Move disk " << disk
             << " from " << name1
             << " to " << name2 << endl;
    }

    // Top disk of rod2 is smaller
    else
    {
        int disk = rod2.back();
        rod2.pop_back();
        rod1.push_back(disk);

        cout << "Move disk " << disk
             << " from " << name2
             << " to " << name1 << endl;
    }
}


// --------------------------------------------------
// Iterative Towers of Hanoi
// --------------------------------------------------
long long hanoiIterative(int n)
{
    vector<int> source;
    vector<int> auxiliary;
    vector<int> target;

    for (int disk = n; disk >= 1; disk--)
        source.push_back(disk);

    long long totalMoves =
        static_cast<long long>(pow(2, n)) - 1;

    char sourceName = 'A';
    char auxiliaryName = 'B';
    char targetName = 'C';

    // For even n, interchange auxiliary and target
    if (n % 2 == 0)
    {
        swap(auxiliary, target);
        swap(auxiliaryName, targetName);
    }

    for (long long move = 1; move <= totalMoves; move++)
    {
        if (move % 3 == 1)
        {
            moveBetweenRods(
                source,
                target,
                sourceName,
                targetName
            );
        }
        else if (move % 3 == 2)
        {
            moveBetweenRods(
                source,
                auxiliary,
                sourceName,
                auxiliaryName
            );
        }
        else
        {
            moveBetweenRods(
                auxiliary,
                target,
                auxiliaryName,
                targetName
            );
        }
    }

    return totalMoves;
}


// --------------------------------------------------
// Main
// --------------------------------------------------
int main()
{
    int n = 3;

    cout << "===== RECURSIVE TOWERS OF HANOI =====" << endl;
    cout << "Number of disks: " << n << endl << endl;

    long long recursiveMoves =
        hanoiRecursive(n, 'A', 'B', 'C');

    cout << endl;
    cout << "Recursive total moves: "
         << recursiveMoves << endl;


    cout << "\n===== ITERATIVE TOWERS OF HANOI =====" << endl;
    cout << "Number of disks: " << n << endl << endl;

    long long iterativeMoves =
        hanoiIterative(n);

    cout << endl;
    cout << "Iterative total moves: "
         << iterativeMoves << endl;


    cout << "\n===== THEORETICAL RESULT =====" << endl;

    long long theoretical =
        static_cast<long long>(pow(2, n)) - 1;

    cout << "2^n - 1 = "
         << theoretical << endl;

    return 0;
}
