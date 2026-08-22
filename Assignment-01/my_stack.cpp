
#include <iostream>
#include <list>
using namespace std;

class MyStack {
private:
    list<int> data;

public:
    void push(int value) {
        data.push_back(value);
    }

    void pop() {
        if (!data.empty()) {
            data.pop_back();
        }
    }

    int top() {
        return data.back();
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
    MyStack stack;

    stack.push(10);
    stack.push(20);
    stack.push(30);

    cout << "Stack after push operations: ";
    stack.display();

    cout << "Top element: " << stack.top() << endl;

    stack.pop();

    cout << "Stack after pop: ";
    stack.display();

    cout << "New top element: " << stack.top() << endl;

    return 0;
}
