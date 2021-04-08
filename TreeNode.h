#ifndef DIPLOM_TREENODE_H
#define DIPLOM_TREENODE_H

template <typename T>
struct TreeNode {
    T value;

    int weight;
    bool isDeleted;
    TreeNode* left;
    TreeNode* right;

    explicit TreeNode(int val){
        value = val;
        weight = 1;
        isDeleted = false;
        left = nullptr;
        right = nullptr;
    }

    TreeNode(int val, TreeNode* Left, TreeNode* Right){
        value = val;
        weight = 1 + Left->weight + Right->weight;
        isDeleted = false;
        left = Left;
        right = Right;
    }

    int getVertexWeight() const {
        return weight - (left == nullptr ? 0 : left->weight) -
                        (right == nullptr ? 0 : right->weight);
    }
};


#endif //DIPLOM_TREENODE_H
