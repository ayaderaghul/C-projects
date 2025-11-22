#ifndef AVL_INDEX_H
#define AVL_INDEX_H

typedef struct AVLNode {
    int key;
    int row_id;
    int height;
    struct AVLNode *left, *right;
} AVLNode;

AVLNode* avl_insert(AVLNode *node,int key, int row_id);
AVLNode* avl_search(AVLNode* node, int key);

#endif