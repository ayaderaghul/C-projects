#include <stdio.h>
#include <stdlib.h>

typedef struct Node {
    int data;
    struct Node *left;
    struct Node *right;
} Node;

Node*create_node(int data){
    Node *node=malloc(sizeof(Node));
    if(node == NULL) {
        printf("malloc failed");
        return NULL;
    }
    node->data=data;
    node->left=NULL;
    node->right=NULL;
    return node;
}

void inorder(Node*root) {
    if(root==NULL) return;
    inorder(root->left);
    printf("%d ", root->data);
    inorder(root->right);
}

void preorder(Node*root) {
    if(root== NULL) return;
    printf("%d ", root->data);
    preorder(root->left);
    preorder(root->right);
}

void postorder(Node*root){
    if(root==NULL) return;
    postorder(root->left);
    postorder(root->right);
    printf("%d ", root->data);
}



// insert

Node *insert(Node*root, int data) {
    if(root==NULL)
        return create_node(data);
    if(data < root->data) root->left =insert(root->left, data);
    if(data > root->data) root->right=insert(root->right,data);
    return root;
}

int count_nodes(Node*root) {
    if(root==NULL) return 0;
    return 1+count_nodes(root->left)+count_nodes(root->right);
}

int height(Node*root){
    if (root == NULL) return 0;
    int left_height = height(root->left);
    int right_height = height(root->right);

    return 1 + (left_height > right_height ? left_height : right_height);
}

int main() {

    Node*root=create_node(1);
    root->left=create_node(2);
    root->right=create_node(3);
    root->left->left=create_node(4);

    printf("inorder traversal: ");
    inorder(root);
    printf("\n");

    printf("preorder traversal: ");
    preorder(root);
    printf("\n");

    printf("postorder traversal: ");
    postorder(root);
    printf("\n");


    printf("total nodes: %d, height: %d\n", count_nodes(root), height(root));
    return 0;

}