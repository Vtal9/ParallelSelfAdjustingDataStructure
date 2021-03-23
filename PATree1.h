#ifndef DIPLOM_PATREE1_H
#define DIPLOM_PATREE1_H

#include <algorithm>
#include "PATree.h"
#include "TreeNode.h"
#include "Action.h"
#include "spgranularity.hpp"
#include <iostream>

using std::cout;

template<typename T>
class PATree1 {
private:
    /*
     curRoot - current root of the tree
     first - begin of the actions list
     last - end of the actions list
     retFirst - begin of the returning list
     retLast - begin of the returning list
     **/
    template<typename ActionsIterator, typename AnswerIterator>
    TreeNode<T> *
    performActions(TreeNode<T> *curRoot, ActionsIterator firstAction, ActionsIterator lastAction, AnswerIterator retFirst,
                   AnswerIterator retLast) {
        if(firstAction == lastAction) return curRoot;
        if (curRoot == nullptr) return createTree(firstAction, lastAction, retFirst, retLast);


        T curValue = curRoot->value;
        double noRotate = 0;   //TODO(compute new weights)
        double leftRotate = 0;  //TODO(compute new weights)
        double rightRotate = 0; //TODO(compute new weights)

        if (leftRotate < noRotate || rightRotate < noRotate) {
            if (leftRotate < rightRotate) {
                curRoot = performLeftRotate(curRoot);
            } else {
                curRoot = performRightRotate(curRoot);
            }
            return performActions(curRoot, firstAction, lastAction, retFirst, retLast);
        }

        ActionsIterator leftFirst = firstAction;
        ActionsIterator rightFirst = std::partition(firstAction, lastAction, [&curValue](const auto &action) {
            return action.value < curValue;
        });

        ActionsIterator leftLast = rightFirst;
        if (rightFirst != lastAction) {
            Action<T> a = *rightFirst;
            AnswerIterator it = retFirst + (rightFirst - firstAction);
            if (a.value == curValue) {
                performAction(a, it, curRoot);
                curRoot->weight++;
                if (rightFirst == firstAction && std::next(rightFirst) == lastAction) {
                    cout << (*rightFirst).value << "\n";
                    return curRoot;
                }
                rightFirst = std::next(rightFirst);
            }
        }

        TreeNode<T> *leftChild = curRoot->left;
        TreeNode<T> *rightChild = curRoot->right;

        int oldCurWeight = curRoot->getVertexWeight();
        sptl::fork2([this, &curRoot, &leftFirst, &leftLast, &retFirst] {
            curRoot->left = performActions(curRoot->left, leftFirst, leftLast, retFirst,
                                           retFirst + (leftLast - leftFirst));
        }, [this, &curRoot, &rightFirst, &lastAction, &retFirst, &firstAction, &retLast] {
            curRoot->right = performActions(curRoot->right, rightFirst, lastAction, retFirst + (rightFirst - firstAction), retLast);
        });
        curRoot->weight = oldCurWeight + (curRoot->left == nullptr ? 0 : curRoot->left->weight) +
                          (curRoot->right == nullptr ? 0 : curRoot->right->weight);
        return curRoot;
    }

    TreeNode<T> *performLeftRotate(TreeNode<T> *curRoot) {
        if (!curRoot || !curRoot->right) return curRoot;
        int oldRootWeight = curRoot->getVertexWeight();

        TreeNode<T> *right = curRoot->right;

        int oldRightChildWeight = right->getVertexWeight();


        curRoot->right = right->left;
        curRoot->weight = oldRootWeight + (curRoot->left == nullptr ? 0 : curRoot->left->weight) +
                          (curRoot->right == nullptr ? 0 : curRoot->right->weight);


        right->left = curRoot;
        right->weight =
                oldRightChildWeight + right->left->weight + (right->right == nullptr ? 0 : right->right->weight);
        return right;
    }

    TreeNode<T> *performRightRotate(TreeNode<T> *curRoot) {
        if (!curRoot || !curRoot->left) return curRoot;
        int oldRootWeight = curRoot->getVertexWeight();

        TreeNode<T> *left = curRoot->left;

        int oldLeftChildWeight = left->getVertexWeight();


        curRoot->left = left->right;
        curRoot->weight = oldRootWeight + (curRoot->left == nullptr ? 0 : curRoot->left->weight) +
                          (curRoot->right == nullptr ? 0 : curRoot->right->weight);


        left->right = curRoot;
        left->weight =
                oldLeftChildWeight + left->right->weight + (left->left == nullptr ? 0 : left->left->weight);

        return left;
    }


    template<typename ActionsIterator, typename AnswerIterator>
    TreeNode<T> *
    createTree(ActionsIterator first, ActionsIterator last, AnswerIterator retFirst, AnswerIterator retLast) {
        if (first == last) return nullptr;
        ActionsIterator mid = first + (last - first) / 2;
        *(retFirst + (mid - first)) = true;
        auto *newRoot = new TreeNode<T>((*mid).value);
        sptl::fork2([this, &newRoot, &first, &mid, &retFirst] {
            newRoot->left = createTree(first, mid, retFirst, (retFirst + (mid - first)));
        }, [this, &newRoot, &mid, &last, &first, &retFirst, &retLast] {
            newRoot->right = createTree(mid + 1, last, retFirst + (mid - first) + 1, retLast);
        });
        return newRoot;
    }

    template<typename Iterator>
    void performAction(Action<T> a, Iterator it, TreeNode<T> *curRoot) {
        switch (a.actionType) {
            case ActionType::INSERT:
                *it = curRoot->isDeleted;
                curRoot->isDeleted = false;
                break;
            case ActionType::LOOKUP:
                *it = !curRoot->isDeleted;
                break;
            case ActionType::REMOVE:
                *it = !curRoot->isDeleted;
                curRoot->isDeleted = true;
                break;
        }
    }

public:
    TreeNode<T> *root;

    template<typename Iterator>
    std::vector<bool> performActionsInParallel(Iterator actionsBegin, Iterator actionsEnd) {
        std::vector<bool> answers(actionsEnd - actionsBegin);
        root = performActions(root, actionsBegin, actionsEnd, answers.begin(), answers.end());
        return answers;
    }

    PATree1() = default;
};

#endif //DIPLOM_PATREE1_H
