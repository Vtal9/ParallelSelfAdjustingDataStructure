#ifndef DIPLOM_PATREE1_H
#define DIPLOM_PATREE1_H

#include <algorithm>
#include "TreeNode.h"
#include "Action.h"
#include "spgranularity.hpp"
#include <iostream>
#include <cmath>

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
    performActions(TreeNode<T> *curRoot, ActionsIterator firstAction, ActionsIterator lastAction,
                   AnswerIterator retFirst,
                   AnswerIterator retLast) {
        if (firstAction == lastAction) return curRoot;
        if (curRoot == nullptr) return createTree(firstAction, lastAction, retFirst, retLast);

        T curValue = curRoot->value;

        //divide actions array to actions on left subtree and actions on right subtree
        ActionsIterator leftFirst = firstAction;
        ActionsIterator rightFirst = std::lower_bound(firstAction, lastAction, Action<T>(curValue, ActionType::INSERT));

        //checking if we need any rotations
        double deltaPhiLeft = calculateDeltaPhiLeft(curRoot, firstAction, rightFirst,
                                                    rightFirst + ((*rightFirst).value == curValue ? 1 : 0), lastAction);
        double deltaPhiRight = calculateDeltaPhiRight(curRoot, firstAction, rightFirst,
                                                      rightFirst + ((*rightFirst).value == curValue ? 1 : 0),
                                                      lastAction);

        if (deltaPhiLeft > 0) {
            curRoot = performLeftRotate(curRoot);
            return performActions(curRoot, firstAction, lastAction, retFirst, retLast);
        }
        if (deltaPhiRight > 0) {
            curRoot = performRightRotate(curRoot);
            return performActions(curRoot, firstAction, lastAction, retFirst, retLast);
        }

        curRoot->weight += lastAction - firstAction;

        //try to perform action in current root
        ActionsIterator leftLast = rightFirst;
        if (rightFirst != lastAction) {
            Action<T> a = *rightFirst;
            AnswerIterator it = retFirst + (rightFirst - firstAction);
            if (a.value == curValue) {
                performAction(a, it, curRoot);
                if (rightFirst == firstAction && std::next(rightFirst) == lastAction) {
                    return curRoot;
                }
                rightFirst = std::next(rightFirst);
            }
        }


        sptl::spguard(
                [&] {
                    int m = lastAction - firstAction;
                    int n = curRoot->size;
                    return log(m) * log(n) + log(n);
                },
                [&] {
                    //parallel body
                    sptl::fork2([this, &curRoot, &leftFirst, &leftLast, &retFirst] {
                        curRoot->left = performActions(curRoot->left, leftFirst, leftLast, retFirst,
                                                       retFirst + (leftLast - leftFirst));
                    }, [this, &curRoot, &rightFirst, &lastAction, &retFirst, &firstAction, &retLast] {
                        curRoot->right = performActions(curRoot->right, rightFirst, lastAction,
                                                        retFirst + (rightFirst - firstAction), retLast);
                    });
                },
                [&] {
                    //sequential body
                    curRoot->left = performActions(curRoot->left, leftFirst, leftLast, retFirst,
                                                   retFirst + (leftLast - leftFirst));
                    curRoot->right = performActions(curRoot->right, rightFirst, lastAction,
                                                    retFirst + (rightFirst - firstAction), retLast);
                }
        );

        curRoot->size = 1 + (curRoot->left == nullptr ? 0 : curRoot->left->size) +
                        (curRoot->right == nullptr ? 0 : curRoot->right->size);

        curRoot->deleted = (curRoot->isDeleted ? 1 : 0) + (curRoot->left == nullptr ? 0 : curRoot->left->deleted) +
                           (curRoot->right == nullptr ? 0 : curRoot->right->deleted);

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

        //update deleted and size of old root
        curRoot->size = 1 + (curRoot->left == nullptr ? 0 : curRoot->left->size) +
                        (curRoot->right == nullptr ? 0 : curRoot->right->size);
        curRoot->deleted = (curRoot->isDeleted ? 1 : 0) + (curRoot->left == nullptr ? 0 : curRoot->left->deleted) +
                           (curRoot->right == nullptr ? 0 : curRoot->right->deleted);

        //update deleted and size of right
        right->size = 1 + (right->right == nullptr ? 0 : right->right->size) + right->left->size;
        right->deleted = (right->isDeleted ? 1 : 0) + (right->right == nullptr ? 0 : right->right->deleted) +
                         right->left->deleted;

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

        //update deleted and size of old root
        curRoot->size = 1 + (curRoot->left == nullptr ? 0 : curRoot->left->size) +
                        (curRoot->right == nullptr ? 0 : curRoot->right->size);
        curRoot->deleted = (curRoot->isDeleted ? 1 : 0) + (curRoot->left == nullptr ? 0 : curRoot->left->deleted) +
                           (curRoot->right == nullptr ? 0 : curRoot->right->deleted);

        //update deleted and size of left
        left->size = 1 + (left->left == nullptr ? 0 : left->left->size) + left->right->size;
        left->deleted = (left->isDeleted ? 1 : 0) + (left->left == nullptr ? 0 : left->left->deleted) +
                        left->right->size;

        return left;
    }


    template<typename ActionsIterator, typename AnswerIterator>
    TreeNode<T> *
    createTree(ActionsIterator first, ActionsIterator last, AnswerIterator retFirst, AnswerIterator retLast) {
        if (first == last) return nullptr;
        ActionsIterator mid = first + (last - first) / 2;
        auto *newRoot = new TreeNode<T>((*mid).value);

        *(retFirst + (mid - first)) = (*mid).actionType == ActionType::INSERT;

        newRoot->weight += last - first - 1;

        int n = last - first;
        sptl::spguard([&] { return n; },
                      [&] {
                          //parallel body
                          sptl::fork2([this, &newRoot, &first, &mid, &retFirst] {
                              newRoot->left = createTree(first, mid, retFirst, (retFirst + (mid - first)));
                          }, [this, &newRoot, &mid, &last, &first, &retFirst, &retLast] {
                              newRoot->right = createTree(mid + 1, last, retFirst + (mid - first) + 1, retLast);
                          });
                      },
                      [&] {
                          //sequential body
                          newRoot->left = createTree(first, mid, retFirst, (retFirst + (mid - first)));
                          newRoot->right = createTree(mid + 1, last, retFirst + (mid - first) + 1, retLast);
                      });
        newRoot->size = 1 + (newRoot->left == nullptr ? 0 : newRoot->left->size) +
                        (newRoot->right == nullptr ? 0 : newRoot->right->size);

        newRoot->deleted = (newRoot->isDeleted ? 1 : 0) + (newRoot->left == nullptr ? 0 : newRoot->left->deleted) +
                           (newRoot->right == nullptr ? 0 : newRoot->right->deleted);
        return newRoot;
    }

    template<typename Iterator>
    void performAction(Action<T> a, Iterator it, TreeNode<T> *curRoot) {
        switch (a.actionType) {
            case ActionType::INSERT:
                *it = curRoot->isDeleted;
                curRoot->deleted -= curRoot->isDeleted ? 1 : 0;
                curRoot->isDeleted = false;
                break;
            case ActionType::LOOKUP:
                *it = !curRoot->isDeleted;
                break;
            case ActionType::REMOVE:
                *it = !curRoot->isDeleted;
                curRoot->deleteVertex();
                break;
        }
    }

    template<typename Iterator>
    double calculateDeltaPhiLeft(TreeNode<T> *curRoot, Iterator leftFirst, Iterator leftLast, Iterator rightFirst,
                                 Iterator rightLast) {
        // deltaPhi = r'(z) - r(y)
        if (curRoot->left == nullptr) return 0;

        double r_y = log(curRoot->left->weight + (leftLast - leftFirst));

        double r_z = curRoot->weight - curRoot->left->weight + (rightLast - rightFirst);

        T leftValue = curRoot->left->value;

        Iterator cFirst = std::lower_bound(leftFirst, leftLast, Action<T>(leftValue, ActionType::INSERT));
        if ((*cFirst).value == leftValue) {
            cFirst++;
        }

        r_z += (curRoot->left->right == nullptr ? 0 : curRoot->left->right->weight) + (leftLast - cFirst);
        r_z = log(r_z);
        return r_z - r_y;
    }

    template<typename Iterator>
    double calculateDeltaPhiRight(TreeNode<T> *curRoot, Iterator leftFirst, Iterator leftLast, Iterator rightFirst,
                                  Iterator rightLast) {
        //deltaPhi = r'(z) - r(y)
        if (curRoot->right == nullptr) return 0;

        double r_y = log(curRoot->right->weight + (rightLast - rightFirst));

        double r_z = curRoot->weight - curRoot->right->weight + (leftLast - leftFirst);

        T rightValue = curRoot->right->value;

        Iterator cLast = std::lower_bound(rightFirst, rightLast, Action<T>(rightValue, ActionType::INSERT));

        r_z += (curRoot->right->left == nullptr ? 0 : curRoot->right->left->weight) + (cLast - rightFirst);
        r_z = log(r_z);
        return r_z - r_y;

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
