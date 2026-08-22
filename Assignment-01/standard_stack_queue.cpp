
#include <iostream>
#include <stack>
#include <queue>
#include <deque>
#include <vector>
#include <list>

using namespace std;

int main() {

    // ------------------------------------------------
    // STACK ALTERNATIVES
    // ------------------------------------------------

    // Default stack uses deque
    stack<int> stackDeque;

    // Stack using vector
    stack<int, vector<int>> stackVector;

    // Stack using list
    stack<int, list<int>> stackList;


    stackDeque.push(10);
    stackDeque.push(20);
    stackDeque.push(30);

    cout << "STACK using deque (LIFO)" << endl;
    cout << "Top element: " << stackDeque.top() << endl;

    stackDeque.pop();

    cout << "Top after pop: " << stackDeque.top() << endl;


    stackVector.push(100);
    stackVector.push(200);

    cout << endl;
    cout << "STACK using vector" << endl;
    cout << "Top element: " << stackVector.top() << endl;


    stackList.push(1000);
    stackList.push(2000);

    cout << endl;
    cout << "STACK using list" << endl;
    cout << "Top element: " << stackList.top() << endl;


    // ------------------------------------------------
    // QUEUE ALTERNATIVES
    // ------------------------------------------------

    // Default queue uses deque
    queue<int> queueDeque;

    // Queue using list
    queue<int, list<int>> queueList;


    queueDeque.push(10);
    queueDeque.push(20);
    queueDeque.push(30);

    cout << endl;
    cout << "QUEUE using deque (FIFO)" << endl;
    cout << "Front element: " << queueDeque.front() << endl;

    queueDeque.pop();

    cout << "Front after dequeue: " << queueDeque.front() << endl;


    queueList.push(100);
    queueList.push(200);

    cout << endl;
    cout << "QUEUE using list" << endl;
    cout << "Front element: " << queueList.front() << endl;

    return 0;
}
