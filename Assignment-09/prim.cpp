#include <iostream>
#include <vector>
#include <limits>
using namespace std;

const int INF = numeric_limits<int>::max() / 4;

void printQueue(const vector<int>& key, const vector<bool>& inTree) {
    cout << "Q = {";
    bool first = true;
    for (int v = 0; v < (int)key.size(); ++v) {
        if (inTree[v]) continue;
        if (!first) cout << ", ";
        cout << char('a' + v) << ":";
        if (key[v] == INF) cout << "INF";
        else cout << key[v];
        first = false;
    }
    cout << "}\n";
}

int main() {
    const int n = 5;
    vector<vector<int>> w(n, vector<int>(n, INF));
    auto addEdge = [&](int u, int v, int cost) {
        w[u][v] = w[v][u] = cost;
    };
    addEdge(0, 1, 5); // a-b
    addEdge(0, 2, 7); // a-c
    addEdge(0, 4, 2); // a-e
    addEdge(1, 3, 6); // b-d
    addEdge(1, 4, 3); // b-e
    addEdge(2, 3, 4); // c-d
    addEdge(2, 4, 4); // c-e
    addEdge(3, 4, 5); // d-e

    // Array-based priority queue: every vertex not in the tree is present.
    vector<int> key(n, INF), parent(n, -1);
    vector<bool> inTree(n, false);
    key[0] = 0; // start at a
    cout << "Initial "; printQueue(key, inTree);

    int total = 0;
    for (int step = 0; step < n; ++step) {
        int u = -1;
        for (int v = 0; v < n; ++v)
            if (!inTree[v] && (u == -1 || key[v] < key[u])) u = v;
        if (u == -1 || key[u] == INF) return 1;
        inTree[u] = true;
        if (parent[u] != -1) {
            total += key[u];
            cout << "Select " << char('a' + u) << ": "
                 << char('a' + parent[u]) << "-" << char('a' + u)
                 << " (" << key[u] << ")\n";
        } else cout << "Select a (root)\n";
        for (int v = 0; v < n; ++v)
            if (!inTree[v] && w[u][v] < key[v]) {
                key[v] = w[u][v];
                parent[v] = u;
            }
        printQueue(key, inTree);
    }
    cout << "MST weight: " << total << "\n";
    cout << "Correct: " << (total == 13 ? "YES" : "NO") << "\n";
    return total == 13 ? 0 : 1;
}
