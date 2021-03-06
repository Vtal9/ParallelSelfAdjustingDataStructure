#include <iostream>
#include <vector>
#include "PATree1.h"
#include<random>
#include<set>
#include <memory>
#include "bench.hpp"

void simpleVisualTest();

void timeTest(int dev);

void testCorrectness();


using std::vector;
using std::cout;

template<typename T>
void printTree(TreeNode<T> *root, const std::string &path) {
    if (root == nullptr) return;
    cout << path << ": " << root->value << " (" << root->weight << ")\n";
    printTree(root->left, path + "->left");
    printTree(root->right, path + "->right");
}

int main(int argc, char **argv) {



    pbbs::launch(argc, argv, [&](const pbbs::measured_type &measure) {
//        simpleVisualTest();
//        testCorrectness();
//        timeTest(10000);
        timeTest(100000);
	cout << "\nbuild_graphs(dev, ns, durations)\n";
        timeTest(1000000);
	cout << "\nbuild_graphs(dev, ns, durations)\n";    
});
//        simpleVisualTest();
//        testCorrectness();


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
            std::sort(actions.begin(), actions.end());
            std::vector<bool> answers(actions.size());
            tree->performActionsInParallel(actions.begin(), actions.end(), answers.begin(), answers.end());
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
                std::sort(actions.begin(), actions.end());
                std::vector<bool> answers(actions.size());
                tree->performActionsInParallel(actions.begin(), actions.end(), answers.begin(), answers.end());

                for (auto answ : answers) {
                    if (!answ) {
                        cout << "???";
                    }
                    assert(answ);
                }
//                k = 0;
                actions.clear();
            }
        }
        std::sort(actions.begin(), actions.end());
        std::vector<bool> answers(actions.size());
        tree->performActionsInParallel(actions.begin(), actions.end(), answers.begin(), answers.end());
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
                std::sort(actions.begin(), actions.end());
                std::vector<bool> answers(actions.size());
                tree->performActionsInParallel(actions.begin(), actions.end(), answers.begin(), answers.end());
                for (auto answ : answers) {
                    assert(answ);
                }
                actions.clear();
            }
        }
        std::sort(actions.begin(), actions.end());
        answers.clear();
        answers.resize(actions.size());
        tree->performActionsInParallel(actions.begin(), actions.end(), answers.begin(), answers.end());
        for (auto answ : answers) {
            assert(answ);
        }
        for (auto it : removeSet) {
            targetSet.erase(it);
        }
        cout << "delete ended\n";
        cout << "iteration " << j << " ended==============\n";
    }


}

vector<Action<int>> generateActions(int m, std::normal_distribution<double> &distribution) {
    std::random_device rd;
    std::mt19937 gen(rd());
    vector<Action<int>> actions;
    std::set<int> numbers;
    while (numbers.size() < m) {
        int number = distribution(gen);
        numbers.insert(number);
    }
    for (const auto &number : numbers) {
        actions.emplace_back(number, ActionType::LOOKUP);
    }
    return actions;
}

void timeTest(int dev) {
	srand(9);
    int N = 10000;
    int countPoints = 100;
    int m = 1000;
    std::normal_distribution<double> distribution(0, dev);
    auto tree = std::make_shared<PATree1<int>>();
    vector<long> ns;
    vector<long long> durations;

    for (int i = 0; i < N; ++i) {
        vector<Action<int>> actions = generateActions(m, distribution);
        std::sort(actions.begin(), actions.end()); //O(nlog(n))
        std::vector<bool> answers(actions.size());
        if (i % (N / countPoints) == 0) {
            auto start = std::chrono::high_resolution_clock::now();
            tree->performActionsInParallel(actions.begin(), actions.end(), answers.begin(), answers.end());
            auto end = std::chrono::high_resolution_clock::now();
            auto evaluatedTime = end - start;
            durations.push_back(evaluatedTime.count());
            ns.push_back(tree->root->size);
        } else {
            tree->performActionsInParallel(actions.begin(), actions.end(), answers.begin(), answers.end());
        }
    }
    cout << "dev = " << dev << std::endl;
    cout << "ns = [" << ns[0];
    for (int i = 1; i < ns.size(); ++i) {
        cout << " ," << ns[i];
    }
    cout << "]\n";

    cout << "durations = [" << durations[0];
    for (int i = 1; i < durations.size(); ++i) {
        cout << " ," << durations[i];
    }
    cout << "]\n";
}


void simpleVisualTest() {
    (vector<bool>(3));

    auto tree = std::make_shared<PATree1<int>>();
    std::vector<Action<int>> actions = {
            Action<int>(3, ActionType::INSERT),
            Action<int>(2, ActionType::INSERT),
            Action<int>(1, ActionType::INSERT)};
    std::sort(actions.begin(), actions.end());
    std::vector<bool> answers(actions.size());
    tree->performActionsInParallel(actions.begin(), actions.end(), answers.begin(), answers.end());
    printTree(tree->root, "root");
    cout << "---------------------------------\n";

    std::vector<Action<int>> actions1 = {Action<int>(4, ActionType::INSERT)};
    std::sort(actions1.begin(), actions1.end());
    answers.clear();
    answers.resize(actions1.size());
    tree->performActionsInParallel(actions1.begin(), actions1.end(), answers.begin(), answers.end());
    printTree(tree->root, "root");

    cout << "---------------------------------\n";


    std::vector<Action<int>> actions2 = {Action<int>(4, ActionType::LOOKUP)};
    std::sort(actions2.begin(), actions2.end());
    answers.clear();
    answers.resize(actions2.size());
    tree->performActionsInParallel(actions2.begin(), actions2.end(), answers.begin(), answers.end());
    printTree(tree->root, "root");

    cout << "---------------------------------\n";


    std::vector<Action<int>> actions3 = {Action<int>(4, ActionType::LOOKUP),
                                         Action<int>(-1, ActionType::INSERT),
                                         Action<int>(0, ActionType::INSERT),
                                         Action<int>(7, ActionType::INSERT)};
    std::sort(actions3.begin(), actions3.end());
    answers.clear();
    answers.resize(actions3.size());
    tree->performActionsInParallel(actions3.begin(), actions3.end(), answers.begin(), answers.end());
    cout << "---------------------------------\n";
    printTree(tree->root, "root");
}
