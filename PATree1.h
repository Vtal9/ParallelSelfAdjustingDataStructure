#ifndef DIPLOM_PATREE1_H
#define DIPLOM_PATREE1_H

#include <algorithm>
#include "TreeNode.h"
#include "Action.h"
#include "granularity.hpp"
#include <iostream>
#include <cmath>

using std::cout;

template<typename T>
class PATree1 {
private:
    const double EPS = 1e-5;
    pasl::pctl::granularity::control_by_prediction controlByPrediction;

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
        int n = lastAction - firstAction;
        //checking if we need any rotations
        double deltaPhiLeft = calculateDeltaPhiLeft(curRoot, firstAction, rightFirst,
                                                    rightFirst, lastAction);
        double deltaPhiRight = calculateDeltaPhiRight(curRoot, firstAction, rightFirst,
                                                      rightFirst,
                                                      lastAction);
        // perform rotation if needed
        if (deltaPhiRight < -EPS && deltaPhiRight < deltaPhiLeft) {
            if (curRoot == this->root) {
                curRoot = performRightRotate(curRoot);
                this->root = curRoot;
            } else {
                curRoot = performRightRotate(curRoot);
            }
            return performActions(curRoot, firstAction, lastAction, retFirst, retLast);
        }
        if (deltaPhiLeft < -EPS) {
            if (curRoot == this->root) {
                curRoot = performLeftRotate(curRoot);
                this->root = curRoot;
            } else {
                curRoot = performLeftRotate(curRoot);
            }
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

        pasl::pctl::granularity::cstmt(controlByPrediction,
                                       [&] {
                                           int m = lastAction - firstAction;
                                           int n = curRoot->size;
                                           return log(m) * log(n) + log(n);
                                       },
                                       [&] {
                                           //parallel body
                                           pasl::pctl::granularity::fork2(
                                                   [this, &curRoot, &leftFirst, &leftLast, &retFirst] {
                                                       curRoot->left = performActions(curRoot->left,
                                                                                      leftFirst,
                                                                                      leftLast,
                                                                                      retFirst,
                                                                                      retFirst +
                                                                                      (leftLast - leftFirst));
                                                   },
                                                   [this, &curRoot, &rightFirst, &lastAction, &retFirst, &firstAction, &retLast] {
                                                       curRoot->right = performActions(curRoot->right,
                                                                                       rightFirst,
                                                                                       lastAction,
                                                                                       retFirst +
                                                                                       (rightFirst - firstAction),
                                                                                       retLast);
                                                   });
                                       },
                                       [&] {
                                           //sequential body
                                           curRoot->left = performActions(curRoot->left, leftFirst, leftLast, retFirst,
                                                                          retFirst + (leftLast - leftFirst));
                                           curRoot->right = performActions(curRoot->right, rightFirst, lastAction,
                                                                           retFirst + (rightFirst - firstAction),
                                                                           retLast);
                                       }
        );

        curRoot->size = 1 + (curRoot->left == nullptr ? 0 : curRoot->left->size) +
                        (curRoot->right == nullptr ? 0 : curRoot->right->size);

        curRoot->deleted = (curRoot->isDeleted ? 1 : 0) + (curRoot->left == nullptr ? 0 : curRoot->left->deleted) +
                           (curRoot->right == nullptr ? 0 : curRoot->right->deleted);

        return curRoot;
    }

    /**
     * current root becomes left child
     */
    TreeNode<T> *performLeftRotate(TreeNode<T> *curRoot) {
        // right child becomes new root so its must be not null
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

    /**
     * current root becomes right child
     */
    TreeNode<T> *performRightRotate(TreeNode<T> *curRoot) {
        // left child becomes new root so its must be not null
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
        pasl::pctl::granularity::cstmt(controlByPrediction, [&] { return n; },
                                       [&] {
                                           //parallel body
                                           pasl::pctl::granularity::fork2([this, &newRoot, &first, &mid, &retFirst] {
                                               newRoot->left = createTree(first, mid, retFirst,
                                                                          (retFirst + (mid - first)));
                                           }, [this, &newRoot, &mid, &last, &first, &retFirst, &retLast] {
                                               newRoot->right = createTree(mid + 1, last, retFirst + (mid - first) + 1,
                                                                           retLast);
                                           });
                                       },
                                       [&] {
                                           //sequential body
                                           newRoot->left = createTree(first, mid, retFirst, (retFirst + (mid - first)));
                                           newRoot->right = createTree(mid + 1, last, retFirst + (mid - first) + 1,
                                                                       retLast);
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

    /**
    * calculate difference of potentials after left rotation
    * left rotate := current root becomes left child
    *             y
    *           /   \
    *         x      z
    *       /  \   /   \
    *     A     B C     D
    * deltaPhi = r'(y) - r(z)
    */
    template<typename Iterator>
    double calculateDeltaPhiLeft(TreeNode<T> *curRoot, Iterator leftFirst, Iterator leftLast, Iterator rightFirst,
                                 Iterator rightLast) {
        // right child becomes root, so its must be not null
        if (curRoot->right == nullptr) return 0;

        // find W'(C)
        int W_C = (curRoot->right->left == nullptr ? 0 : curRoot->right->left->weight);
        Iterator cActionsLast = std::lower_bound(rightFirst, rightLast,
                                                 Action<T>(curRoot->right->value, ActionType::INSERT));
        int sizeOfCSubtreeActions = cActionsLast - rightFirst;
        int W_C_new = W_C + sizeOfCSubtreeActions;

        // find w'(y)
        int w_y = curRoot->getVertexWeight();
        int w_y_new = w_y + ((*rightFirst).value == curRoot->value);

        // find W'(x)
        int W_left_new = (curRoot->left == nullptr ? 0 : curRoot->left->weight) + (leftLast - leftFirst);

        // r'(y) = log(W'(x) + w'(y) + W'(C))
        double r_y = log2(W_left_new + W_C_new + w_y_new);

        // r(z)
        double r_z = log2(curRoot->right->weight + (rightLast - rightFirst));

        return r_y - r_z;
    }

/**
    * calculate difference of potentials after right rotation
    * right rotate := current root becomes right child
    *             y
    *           /   \
    *         x      z
    *       /  \   /   \
    *     A     B C     D
    * deltaPhi = r'(y) - r(x)
    */    template<typename Iterator>
    double calculateDeltaPhiRight(TreeNode<T> *curRoot, Iterator leftFirst, Iterator leftLast, Iterator rightFirst,
                                  Iterator rightLast) {
        // left child becomes root, so its must be not null
        if (curRoot->left == nullptr) return 0;

        // find W'(B)
        int W_B = (curRoot->left->right == nullptr ? 0 : curRoot->left->right->weight);
        Iterator bActionsFirst = std::lower_bound(leftFirst, leftLast,
                                                  Action<T>(curRoot->left->value, ActionType::INSERT));
        if ((*bActionsFirst).value == curRoot->left->value) bActionsFirst++;
        int sizeOfBSubtreeActions = leftLast - bActionsFirst;
        int W_B_new = W_B + sizeOfBSubtreeActions;

        // find w'(y)
        int w_y = curRoot->getVertexWeight();
        int w_y_new = w_y + ((*rightFirst).value == curRoot->value);

        // find W'(z)
        int W_right_new = (curRoot->right == nullptr ? 0 : curRoot->right->weight) + (rightLast - rightFirst);

        // r'(y) = log(W'(z) + w'(y) + W'(B))
        double r_y_new = log2(W_right_new + w_y_new + W_B_new);

        // r(x)
        double r_x = log2(curRoot->left->weight + (leftLast - leftFirst));

        return r_y_new - r_x;
    }

public:
    TreeNode<T> *root;

    template<typename Iterator, typename answersIterator>
    void performActionsInParallel(Iterator actionsBegin, Iterator actionsEnd, answersIterator answersBegin,
                                               answersIterator answersEnd) {
        root = performActions(root, actionsBegin, actionsEnd, answersBegin, answersEnd);
    }

    PATree1() = default;

    ~PATree1() {
        delete root;
    }
};

#endif //DIPLOM_PATREE1_H
