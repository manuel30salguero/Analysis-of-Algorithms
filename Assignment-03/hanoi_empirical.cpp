
#include <iostream>
#include <vector>
#include <chrono>
#include <iomanip>
#include <algorithm>

using namespace std;
using namespace chrono;


// --------------------------------------------------
// Recursive Hanoi without printing
// --------------------------------------------------
long long hanoiRecursiveCount(int n)
{
    if (n == 1)
        return 1;

    long long moves = 0;

    moves += hanoiRecursiveCount(n - 1);
    moves += 1;
    moves += hanoiRecursiveCount(n - 1);

    return moves;
}


// --------------------------------------------------
// Legal move between two rods without printing
// --------------------------------------------------
void moveBetweenRodsSilent(vector<int>& rod1,
                           vector<int>& rod2)
{
    if (rod1.empty())
    {
        rod1.push_back(rod2.back());
        rod2.pop_back();
    }
    else if (rod2.empty())
    {
        rod2.push_back(rod1.back());
        rod1.pop_back();
    }
    else if (rod1.back() < rod2.back())
    {
        rod2.push_back(rod1.back());
        rod1.pop_back();
    }
    else
    {
        rod1.push_back(rod2.back());
        rod2.pop_back();
    }
}


// --------------------------------------------------
// Iterative Hanoi without printing
// --------------------------------------------------
long long hanoiIterativeCount(int n)
{
    vector<int> source;
    vector<int> auxiliary;
    vector<int> target;

    for (int disk = n; disk >= 1; disk--)
        source.push_back(disk);

    long long totalMoves = (1LL << n) - 1;

    vector<int>* aux = &auxiliary;
    vector<int>* dest = &target;

    if (n % 2 == 0)
        swap(aux, dest);

    for (long long move = 1; move <= totalMoves; move++)
    {
        if (move % 3 == 1)
            moveBetweenRodsSilent(source, *dest);

        else if (move % 3 == 2)
            moveBetweenRodsSilent(source, *aux);

        else
            moveBetweenRodsSilent(*aux, *dest);
    }

    return totalMoves;
}


// --------------------------------------------------
// Median recursive execution time
// --------------------------------------------------
double recursiveMedianTime(int n, int repetitions = 7)
{
    vector<double> times;

    // Warm-up
    hanoiRecursiveCount(n);

    for (int i = 0; i < repetitions; i++)
    {
        auto start = high_resolution_clock::now();

        hanoiRecursiveCount(n);

        auto finish = high_resolution_clock::now();

        double elapsed =
            duration<double>(finish - start).count();

        times.push_back(elapsed);
    }

    sort(times.begin(), times.end());

    return times[times.size() / 2];
}


// --------------------------------------------------
// Median iterative execution time
// --------------------------------------------------
double iterativeMedianTime(int n, int repetitions = 7)
{
    vector<double> times;

    // Warm-up
    hanoiIterativeCount(n);

    for (int i = 0; i < repetitions; i++)
    {
        auto start = high_resolution_clock::now();

        hanoiIterativeCount(n);

        auto finish = high_resolution_clock::now();

        double elapsed =
            duration<double>(finish - start).count();

        times.push_back(elapsed);
    }

    sort(times.begin(), times.end());

    return times[times.size() / 2];
}


// --------------------------------------------------
// Main empirical experiment
// --------------------------------------------------
int main()
{
    cout << fixed << setprecision(8);

    cout << "TOWERS OF HANOI - EMPIRICAL ANALYSIS\n\n";

    cout << left
         << setw(5)  << "n"
         << setw(18) << "Theoretical"
         << setw(18) << "Recursive"
         << setw(18) << "Iterative"
         << setw(22) << "Rec. Time (s)"
         << setw(22) << "Iter. Time (s)"
         << endl;

    for (int n = 5; n <= 18; n++)
    {
        long long theoretical = (1LL << n) - 1;

        long long recursiveMoves =
            hanoiRecursiveCount(n);

        long long iterativeMoves =
            hanoiIterativeCount(n);

        double recursiveTime =
            recursiveMedianTime(n);

        double iterativeTime =
            iterativeMedianTime(n);

        cout << left
             << setw(5)  << n
             << setw(18) << theoretical
             << setw(18) << recursiveMoves
             << setw(18) << iterativeMoves
             << setw(22) << recursiveTime
             << setw(22) << iterativeTime
             << endl;
    }

    return 0;
}
