#include <iostream>
#include <vector>
#include "PATree1.h"

using std::vector;
using std::cout;


int main() {
    vector<bool> answers(3);

    PATree1<int> tree{};
    vector<Action<int>> actions = {
    Action<int>(3, ActionType::INSERT),
    Action<int>(2, ActionType::INSERT),
    Action<int>(1, ActionType::INSERT)};
    cout << "start performing actions \n";
    std::sort(actions.begin(), actions.end());
    answers = tree.performActionsInParallel(actions.begin(), actions.end());
    cout << "root = " << tree.root->value << "\n left = " << tree.root->left->value << "\n right = " << tree.root->right->value;
    return 0;
}