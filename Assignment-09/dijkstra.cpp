#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <string>
#include <utility>
#include <vector>
using namespace std;

const int INF = numeric_limits<int>::max() / 4;
using Edge = pair<int, int>; // (neighbor, weight)
using Graph = vector<vector<Edge>>;

void addEdge(Graph& g, char from, char to, int weight, bool directed) {
    int u = from - 'a', v = to - 'a';
    g[u].push_back({v, weight});
    if (!directed) g[v].push_back({u, weight});
}

vector<int> dijkstra(const Graph& g, int source, vector<int>& parent) {
    int n = (int)g.size();
    vector<int> dist(n, INF);
    vector<bool> settled(n, false);
    parent.assign(n, -1);
    dist[source] = 0;

    // Array-based priority queue: all unsettled vertices are included.
    for (int step = 0; step < n; ++step) {
        int u = -1;
        for (int v = 0; v < n; ++v)
            if (!settled[v] && (u == -1 || dist[v] < dist[u])) u = v;
        if (u == -1 || dist[u] == INF) break;
        settled[u] = true;
        for (auto [v, w] : g[u]) {
            if (w < 0) { cerr << "Negative weights not supported\n"; exit(1); }
            if (!settled[v] && dist[u] + w < dist[v]) {
                dist[v] = dist[u] + w;
                parent[v] = u;
            }
        }
    }
    return dist;
}

void run(const string& name, const Graph& g, const vector<int>& expected) {
    cout << "\n" << name << " (source a)\n";
    vector<int> parent;
    vector<int> dist = dijkstra(g, 0, parent);
    bool correct = (dist == expected);
    for (int v = 0; v < (int)g.size(); ++v) {
        cout << char('a' + v) << ": ";
        if (dist[v] == INF) { cout << "INF\n"; continue; }
        cout << dist[v] << ", path ";
        vector<int> path;
        for (int u = v; u != -1; u = parent[u]) path.push_back(u);
        reverse(path.begin(), path.end());
        for (int i = 0; i < (int)path.size(); ++i) {
            if (i) cout << " -> ";
            cout << char('a' + path[i]);
        }
        cout << "\n";
    }
    cout << "Correct: " << (correct ? "YES" : "NO") << "\n";
    if (!correct) exit(1);
}

int main() {
    // Problem 3(a): directed graph; arrows must be respected.
    Graph a(5);
    addEdge(a, 'b', 'a', 3, true);
    addEdge(a, 'a', 'd', 7, true);
    addEdge(a, 'd', 'b', 2, true);
    addEdge(a, 'b', 'c', 4, true);
    addEdge(a, 'd', 'c', 5, true);
    addEdge(a, 'c', 'e', 6, true);
    addEdge(a, 'e', 'd', 4, true);
    run("Problem 3(a) - directed", a, {0, 9, 12, 7, 18});

    // Problem 3(b): undirected graph from the second page of Lab 09.
    Graph b(12);
    addEdge(b, 'a', 'b', 3, false);
    addEdge(b, 'a', 'c', 5, false);
    addEdge(b, 'a', 'd', 4, false);
    addEdge(b, 'b', 'e', 3, false);
    addEdge(b, 'b', 'f', 6, false);
    addEdge(b, 'c', 'd', 2, false);
    addEdge(b, 'c', 'g', 4, false);
    addEdge(b, 'd', 'e', 1, false);
    addEdge(b, 'd', 'h', 5, false);
    addEdge(b, 'e', 'f', 2, false);
    addEdge(b, 'e', 'i', 4, false);
    addEdge(b, 'f', 'j', 5, false);
    addEdge(b, 'g', 'h', 3, false);
    addEdge(b, 'g', 'k', 6, false);
    addEdge(b, 'h', 'i', 6, false);
    addEdge(b, 'h', 'k', 7, false);
    addEdge(b, 'i', 'j', 3, false);
    addEdge(b, 'i', 'l', 5, false);
    addEdge(b, 'j', 'l', 9, false);
    addEdge(b, 'k', 'l', 8, false);
    run("Problem 3(b) - undirected", b,
        {0, 3, 5, 4, 5, 7, 9, 9, 9, 12, 15, 14});
}
