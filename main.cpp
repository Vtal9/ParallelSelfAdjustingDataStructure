#include <iostream>
#include <vector>
#include "PATree1.h"
#include<random>
#include<set>
#include <memory>
//#include "bench.hpp"

void simpleVisualTest();
void testCorrectness();

using std::vector;
using std::cout;

template<typename  T>
void printTree(TreeNode<T>* root, std::string path){
    if(root == nullptr) return;
    cout << path << ": " << root->value << " (" << root->weight << ")\n";
    printTree(root->left, path + "->left");
    printTree(root->right, path + "->right");
}


int main(int argc, char **argv) {

//    pbbs::launch(argc, argv, [&] {
//        simpleVisualTest();
//        testCorrectness();
//
//    });

        simpleVisualTest();
        testCorrectness();
    return 0;
}

void testCorrectness() {
    auto tree = std::make_shared<PATree1<int>>();
    std::set<int> targetSet;

    for (int j = 0; j < 10; ++j) {
        //emplace
        cout << "emplace \n";
        for (int i = 0; i < 100; ++i) {
            vector<Action<int>> actions;
            for (int k = 0; k < 10; ++k) {
                int it = rand() % 10000;
//                cout << it << "\n";
                actions.emplace_back(it, ActionType::INSERT);
                targetSet.insert(it);
            }
            tree->performActionsInParallel(actions.begin(), actions.end());
        }

        cout << "emplace ended\n";
//        printTree(tree->root, "");

        //lookup
        cout << "lookup\n";
        int k = 0;
        vector<Action<int>> actions;
        for (auto it: targetSet) {
            actions.emplace_back(it, ActionType::LOOKUP);
            k++;
            if (k % 10 == 0) {
                auto answers = tree->performActionsInParallel(actions.begin(), actions.end());

                for (auto answ : answers) {
                    if(!answ){
                        cout << "???";
                    }
                    assert(answ);
                }
//                k = 0;
                actions.clear();
            }
        }
        auto answers = tree->performActionsInParallel(actions.begin(), actions.end());
        for (auto answ : answers) {
            assert(answ);
        }
        cout << "lookup ended\n";
        //delete
        cout << "delete\n";
        actions.clear();
        k = 0;
        std::set<int> removeSet;
        for (auto it : targetSet) {
            if (k % 2 == 0) {
                actions.emplace_back(Action<int>(it, ActionType::REMOVE));
            }
            removeSet.insert(it);
            k++;
            if (k % 10 == 0) {
                answers = tree->performActionsInParallel(actions.begin(), actions.end());
                for (auto answ : answers) {
                    assert(answ);
                }
                actions.clear();
            }
        }
        answers = tree->performActionsInParallel(actions.begin(), actions.end());
        for (auto answ : answers) {
            assert(answ);
        }
        for(auto it : removeSet){
            targetSet.erase(it);
        }
        cout << "delete ended\n";
        cout << "iteration " << j << " ended==============\n";
    }


}

void simpleVisualTest() {
    vector<bool> answers(3);

    auto tree = std::make_shared<PATree1<int>>();
    std::vector<Action<int>> actions = {
            Action<int>(3, ActionType::INSERT),
            Action<int>(2, ActionType::INSERT),
            Action<int>(1, ActionType::INSERT)};
    answers = tree->performActionsInParallel(actions.begin(), actions.end());
    printTree(tree->root, "root");
    cout << "---------------------------------\n";

    std::vector<Action<int>> actions1 = {Action<int>(4, ActionType::INSERT)};
    answers = tree->performActionsInParallel(actions1.begin(), actions1.end());
    printTree(tree->root, "root");

    cout << "---------------------------------\n";


    std::vector<Action<int>> actions2 = {Action<int>(4, ActionType::LOOKUP)};
    answers = tree->performActionsInParallel(actions2.begin(), actions2.end());
    printTree(tree->root, "root");

    cout << "---------------------------------\n";


    std::vector<Action<int>> actions3 = {Action<int>(4, ActionType::LOOKUP),
                                        Action<int>(-1, ActionType::INSERT),
                                         Action<int>(0, ActionType::INSERT),
                                         Action<int>(7, ActionType::INSERT)};
    answers = tree->performActionsInParallel(actions3.begin(), actions3.end());
    cout << "---------------------------------\n";
    printTree(tree->root, "root");
}
