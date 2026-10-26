#include <iostream>
using namespace std;

// Closed hashing (open addressing) using only fixed-size arrays.
class HashTable {
    static const int SIZE = 13;
    int keys[SIZE];
    int state[SIZE];               // 0 = empty, 1 = occupied, 2 = deleted

    int hashKey(int key) const {
        int r = key % SIZE;
        return r < 0 ? r + SIZE : r;
    }

public:
    HashTable() {
        for (int i = 0; i < SIZE; ++i) state[i] = 0;
    }

    int find(int key) const {
        int start = hashKey(key);
        for (int step = 0; step < SIZE; ++step) {
            int i = (start + step) % SIZE;
            if (state[i] == 0) return -1;
            if (state[i] == 1 && keys[i] == key) return i;
        }
        return -1;
    }

    bool insert(int key) {
        int start = hashKey(key), firstDeleted = -1;
        for (int step = 0; step < SIZE; ++step) {
            int i = (start + step) % SIZE;
            if (state[i] == 1 && keys[i] == key) return false;
            if (state[i] == 2 && firstDeleted == -1) firstDeleted = i;
            if (state[i] == 0) {
                int target = (firstDeleted == -1 ? i : firstDeleted);
                keys[target] = key;
                state[target] = 1;
                return true;
            }
        }
        if (firstDeleted != -1) {
            keys[firstDeleted] = key;
            state[firstDeleted] = 1;
            return true;
        }
        return false; // Table full.
    }

    bool erase(int key) {
        int i = find(key);
        if (i == -1) return false;
        state[i] = 2; // Tombstone keeps the probing chain intact.
        return true;
    }

    void print() const {
        cout << "Index | Value\n";
        for (int i = 0; i < SIZE; ++i) {
            cout << "  " << i << "   | ";
            if (state[i] == 1) cout << keys[i];
            else if (state[i] == 2) cout << "DELETED";
            else cout << "EMPTY";
            cout << '\n';
        }
    }
};

int main() {
    HashTable table;
    const int data[] = {18, 31, 44, 57, 8, 21, 34};
    bool ok = true;
    for (int key : data) {
        bool inserted = table.insert(key);
        cout << "Insert " << key << ": " << (inserted ? "YES" : "NO") << '\n';
        ok = inserted && ok;
    }
    cout << "Search 44: " << (table.find(44) != -1 ? "FOUND" : "NOT FOUND") << '\n';
    cout << "Search 99: " << (table.find(99) != -1 ? "FOUND" : "NOT FOUND") << '\n';
    ok = (table.find(44) != -1 && table.find(99) == -1) && ok;

    bool removed = table.erase(31);
    cout << "Delete 31: " << (removed ? "YES" : "NO") << '\n';
    bool stillFound = (table.find(44) != -1);
    cout << "Search 44 after deletion: " << (stillFound ? "FOUND" : "NOT FOUND") << '\n';
    bool reused = table.insert(70);
    cout << "Insert 70: " << (reused ? "YES" : "NO") << '\n';
    ok = removed && stillFound && reused && table.find(31) == -1 && table.find(70) != -1 && ok;
    table.print();
    cout << "All checks: " << (ok ? "PASSED" : "FAILED") << '\n';
    return ok ? 0 : 1;
}
