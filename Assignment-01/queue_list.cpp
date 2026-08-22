
#include <iostream>
#include <list>
using namespace std;

int main() {
    list<int> queue;

    // Enqueue elements
    queue.push_back(10);
    queue.push_back(20);
    queue.push_back(30);

    cout << "Queue after enqueue operations: ";

    for (int value : queue) {
        cout << value << " ";
    }

    cout << endl;

    // Front element
    cout << "Front element: " << queue.front() << endl;

    // Dequeue element
    queue.pop_front();

    cout << "Queue after dequeue: ";

    for (int value : queue) {
        cout << value << " ";
    }

    cout << endl;

    cout << "New front element: " << queue.front() << endl;

    return 0;
}
