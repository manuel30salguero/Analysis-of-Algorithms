
#include <iostream>
#include <vector>
#include <queue>

using namespace std;

// --------------------------------------------------
// Breadth First Search
// --------------------------------------------------
void bfs(
    int startVertex,
    const vector<vector<int>>& graph,
    vector<bool>& visited)
{
    queue<int> q;

    visited[startVertex] = true;
    q.push(startVertex);

    while (!q.empty())
    {
        int vertex = q.front();
        q.pop();

        cout << vertex << " ";

        for (int neighbor : graph[vertex])
        {
            if (!visited[neighbor])
            {
                visited[neighbor] = true;
                q.push(neighbor);
            }
        }
    }
}

// --------------------------------------------------
// Main
// --------------------------------------------------
int main()
{
    const int V = 7;

    vector<vector<int>> graph(V);

    // Undirected graph
    graph[0] = {1, 2};
    graph[1] = {0, 3, 4};
    graph[2] = {0, 5};
    graph[3] = {1, 5};
    graph[4] = {1, 6};
    graph[5] = {2, 3};
    graph[6] = {4};

    vector<bool> visited(V, false);

    cout << "BREADTH FIRST SEARCH\n\n";

    cout << "Adjacency List:\n";

    for (int i = 0; i < V; i++)
    {
        cout << i << ": ";

        for (int neighbor : graph[i])
        {
            cout << neighbor << " ";
        }

        cout << endl;
    }

    cout << "\nBFS Traversal: ";

    // Perform BFS for every unvisited vertex
    for (int vertex = 0; vertex < V; vertex++)
    {
        if (!visited[vertex])
        {
            bfs(
                vertex,
                graph,
                visited
            );
        }
    }

    cout << endl;

    return 0;
}
