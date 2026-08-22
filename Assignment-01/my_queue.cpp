
#include <iostream>
#include <list>
using namespace std;

class MyQueue {
private:
    list<int> data;

public:
    void enqueue(int value) {
        data.push_back(value);
    }

    void dequeue() {
        if (!data.empty()) {
            data.pop_front();
        }
    }

    int front() {
        return data.front();
    }

    bool empty() {
        return data.empty();
    }

    void display() {
        for (int value : data) {
            cout << value << " ";
        }
        cout << endl;
    }
};

int main() {
    MyQueue queue;

    queue.enqueue(10);
    queue.enqueue(20);
    queue.enqueue(30);

    cout << "Queue after enqueue operations: ";
    queue.display();

    cout << "Front element: " << queue.front() << endl;

    queue.dequeue();

    cout << "Queue after dequeue: ";
    queue.display();

    cout << "New front element: " << queue.front() << endl;

    return 0;
}
