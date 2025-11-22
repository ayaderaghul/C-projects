#include <stdio.h>
#include <stdlib.h>
#include "avl_index.h"

int height(AVLNode*n) {return (n ? n->height : 0);}

int max(int a, int b) {return (a>b) ? a : b;}

AVLNode*newNode(int key, int row_id) {
    AVLNode *n = malloc(sizeof(AVLNode));
    n->key=key;
    n->row_id=row_id;
    n->left=n->right=NULL;
    n->height=1;
    return n;
}

int getBalanceFactor(AVLNode *n) {
    return n ? height(n->left) - height(n->right) : 0;
}

AVLNode*rightRotate(AVLNode*y) {
    AVLNode *x = y->left;
    AVLNode* T2 = x->right;

    x->right=y;
    y->left=T2;

    y->height = max(height(y->left), height(y->right)) + 1;
    x->height = max(height(x->left), height(x->right)) + 1;

    return x;
}

AVLNode* leftRotate(AVLNode *x) {
    AVLNode *y = x->right;
    AVLNode *T2 = y->left;

    y->left = x;
    x->right = T2;

    x->height = max(height(x->left), height(x->right)) +1 ;
    y->height = max(height(y->left), height(y->right)) +1;
    return y;
}

AVLNode*avl_insert(AVLNode*node, int key, int row_id) {
    if(!node) return newNode(key,row_id);
    if(key<node->key)
        node->left=avl_insert(node->left, key, row_id);
    else if(key> node->key)
        node->right=avl_insert(node->right,key, row_id);
    else    
        return node; // duplicate keys not inserted
    
        node->height = 1 + max(height(node->left), height(node->right));
        int balance = getBalanceFactor(node);

        // LL 
        if(balance>1 && key < node->left->key)
            return rightRotate(node);
        // RR
        if(balance < -1 && key > node->right->key)
            return leftRotate(node);

        // LR
        if(balance>1 && key>node->left->key) {
            node->left = leftRotate(node->left);
            return rightRotate(node);
        }

        // RL
        if(balance<-1 && key<node->right->key){
            node->right=rightRotate(node->right);
            return leftRotate(node);
        }
        return node;
}

AVLNode*avl_search(AVLNode*node, int key) {
    if(!node) return NULL;
    if(key==node->key)return node;
    if(key<node->key)
        return avl_search(node->left,key);
    return avl_search(node->right,key);
}

char *names[] = {"Alice", "Bob", "Charlie"};

int main(){
    AVLNode*index=NULL;
    index=avl_insert(index,10,0);
    index=avl_insert(index,20,1);
    index=avl_insert(index,15,2);

    printf("index built\n");

    int id=15;
    AVLNode*result=avl_search(index,id);

    if(result){
        printf("found key %d, row id %d, name %s\n", 
        result->key, result->row_id, names[result->row_id]);
    } else {
        printf("key not found");
    }
    return 0;
}