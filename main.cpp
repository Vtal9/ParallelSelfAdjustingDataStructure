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
    std::sort(actions.begin(), actions.end());
    answers = tree.performActionsInParallel(actions.begin(), actions.end());
    cout << "root = " << tree.root->value << "\n left = " << tree.root->left->value << "\n right = " << tree.root->right->value << "\n";
    vector<Action<int>> actions1 = {Action<int>(4, ActionType::INSERT)};
    answers = tree.performActionsInParallel(actions1.begin(), actions1.end());
    cout << "root = " << tree.root->value << "\n left = " << tree.root->left->value << "\n right = " << tree.root->right->value << "\n";
    cout << "right->right = " << tree.root->right->right->value;
    return 0;
}