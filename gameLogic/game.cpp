#include <iostream>
#include <vector>

using namespace std;

vector<int> arr = {1, 2, 3, 4, 5};

bool calc(vector<int> &res, int x, int s, int i, vector<int> &finalRes) {
    if (s == x) {
        finalRes = res;  // Store the valid subset
        return true;
    }
    if (i == arr.size() || s > x) return false;

    // Include current element
    res.push_back(arr[i]);
    if (calc(res, x, s + arr[i], i + 1, finalRes)) return true;
    res.pop_back(); // Backtrack

    // Exclude current element
    return calc(res, x, s, i + 1, finalRes);
}

int main() {
    int x = 7;
    vector<int> res, finalRes;

    if (calc(res, x, 0, 0, finalRes)) {
        cout << "Subset found: ";
        for (int i : finalRes) cout << i << ", ";
        cout << endl;
    } else {
        cout << "No subset found" << endl;
    }
    return 0;
}
