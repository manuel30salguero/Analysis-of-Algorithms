
#include <iostream>
using namespace std;

class BinaryTree {
private:

    struct Node {
        int value;
        Node* left;
        Node* right;

        Node(int v) {
            value = v;
            left = nullptr;
            right = nullptr;
        }
    };

    Node* root;

    Node* insert(Node* node, int value) {

        if (node == nullptr) {
            return new Node(value);
        }

        if (value < node->value) {
            node->left = insert(node->left, value);
        }
        else if (value > node->value) {
            node->right = insert(node->right, value);
        }

        return node;
    }

    void inorder(Node* node) {

        if (node != nullptr) {
            inorder(node->left);
            cout << node->value << " ";
            inorder(node->right);
        }
    }

public:

    BinaryTree() {
        root = nullptr;
    }

    void insert(int value) {
        root = insert(root, value);
    }

    void display() {
        inorder(root);
        cout << endl;
    }
};

int main() {

    BinaryTree tree;

    tree.insert(9);
    tree.insert(4);
    tree.insert(12);
    tree.insert(1);
    tree.insert(7);
    tree.insert(10);

    cout << "Binary Search Tree (in-order): ";
    tree.display();

    return 0;
}
