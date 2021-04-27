#ifndef DIPLOM_TREENODE_H
#define DIPLOM_TREENODE_H

template<typename T>
struct TreeNode {
    T value;

    int weight;
    bool isDeleted;
    TreeNode *left;
    TreeNode *right;

    int size;
    int deleted;

    explicit TreeNode(int val) {
        value = val;
        weight = 1;
        isDeleted = false;
        left = nullptr;
        right = nullptr;
        size = 1;
        deleted = 0;
    }

    TreeNode(int val, TreeNode *Left, TreeNode *Right) {
        value = val;
        weight = 1 + Left->weight + Right->weight;
        isDeleted = false;
        left = Left;
        right = Right;
        size = 1 + (left == nullptr ? 0 : left->size) + (right == nullptr ? 0 : right->size);
        deleted = (left == nullptr ? 0 : left->deleted) + (right == nullptr ? 0 : right->deleted);
    }

    void deleteVertex(){
        deleted += isDeleted ? 0 : 1;
        isDeleted = true;
    }

    int getVertexWeight() const {
        return weight - (left == nullptr ? 0 : left->weight) -
               (right == nullptr ? 0 : right->weight);
    }
};


#endif //DIPLOM_TREENODE_H
