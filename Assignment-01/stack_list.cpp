
#include <iostream>
#include <list>
using namespace std;

int main() {
    list<int> stack;

    // Push elements
    stack.push_back(10);
    stack.push_back(20);
    stack.push_back(30);

    cout << "Stack after push operations: ";

    for (int value : stack) {
        cout << value << " ";
    }

    cout << endl;

    // Top element
    cout << "Top element: " << stack.back() << endl;

    // Pop element
    stack.pop_back();

    cout << "Stack after pop: ";

    for (int value : stack) {
        cout << value << " ";
    }

    cout << endl;

    cout << "New top element: " << stack.back() << endl;

    return 0;
}
