#include <stdio.h>
#include <stdlib.h>

typedef struct Node {
    int key;
    struct Node *left, *right;
} Node;

Node* new_node(int key) {
    Node *n = malloc(sizeof(Node));
    n->key=key;
    n->left=n->right =NULL;
    return n;
}

Node *bst_insert(Node*root, int key) {
    if(root==NULL) return new_node(key);
    if(key<root->key) root->left = bst_insert(root->left, key);
    if(key>root->key) root->right = bst_insert(root->right,key);
    else {
        // if duplicates
    }
    return root;
}

Node* bst_search(Node*root, int key) {
    if(!root) return NULL;
    if(key==root->key) return root;
    if(key<root->key) return bst_search(root->left,key);
    return bst_search(root->right,key);
}

Node*bst_min_value_node(Node*node) {
    Node*curr=node;
    while(curr && curr->left != NULL) curr = curr->left;
    return curr;
}

Node*find_max(Node*root){
    if(root==NULL) return NULL;
    while(root->right) root = root->right;
    return root;
}

Node*bst_delete(Node*root, int key) {
    if(root==NULL) return root;
    if(key <root->key) {
        root->left = bst_delete(root->left, key);
    } else if (key >root->key) {
        root->right = bst_delete(root->right, key);
    } else {

        if (root->left == NULL) {
            Node *tmp = root->right;
            free(root);
            return tmp;
        } else if (root->right == NULL) {
            Node *tmp = root->left;
            free(root);
            return tmp;
        }

        Node *tmp = bst_min_value_node(root->right);
        root->key = tmp->key;
        root->right = bst_delete(root->right, tmp->key);
    }
    return root;
}

void inorder(Node*root) {
    if(!root) return;
    inorder(root->left);
    printf("%d ", root->key);
    inorder(root->right);
}

void print_tree(Node*root, int space) {
    if(root== NULL) return;

    space+=5;
    print_tree(root->right, space);

    printf("\n");

    for(int i= 5; i< space;i++) printf(" ");

    printf("%d\n", root->key);

    print_tree(root->left, space);
}

int main() {
    Node*root=NULL;
    int choice,value;

    while(1){
        printf("BST operation\n1. Insert 2 Delete 3 Search 4 Inorder 5 Print tree 6 Min max 7 Exit\n");
        scanf("%d", &choice);

        switch(choice){
            case 1:
                printf("enter value to insert: ");
                scanf("%d", &value);
                root=bst_insert(root, value);
                printf("inserted %d\n", value);
                break;

            case 2:
                printf("enter value to delete: ");
                scanf("%d", &value);
                root=bst_delete(root,value);
                printf("deleted %d\n", value);
                break;

            case 3:
                printf("enter value to search: ");
                scanf("%d", &value);
                if(bst_search(root,value)) {
                    printf("%d found\n", value);
                } else printf("%d not found\n", value);
                break;

            case 4: 
                printf("inorder: ");
                inorder(root);
                printf("\n");
                break;

            case 5:
                printf("print tree\n");
                print_tree(root, 0);
                break;

            case 6: 
                if(root) {
                    Node*min=bst_min_value_node(root);
                    Node*max=find_max(root);
                    printf("min: %d, max %d\n", min->key, max->key);
                } else{
                    printf("tree is empty\n");
                }
                break;
            case 7:
                printf("goodbye\n");
                return 0;
            default:
                printf("invalid choice\n");

        }
    }
    return 0;
}


#ifdef TEST_BST
int main() {
    Node*root = NULL;
    int keys[] = {50,30,20,40,70,60,80};
    for (int i=0; i< sizeof(keys)/sizeof(keys[0]);++i)
        root = bst_insert(root, keys[i]);

    printf("inorder: "); inorder(root); printf("\n");

    int q = 40;
    Node *found=bst_search(root,q);
    printf("search %d: %s\n", q, found ? "found" : "not found");
    
    
    root = bst_delete(root,20);
    root = bst_delete(root, 30);
    root=bst_delete(root,50);

    inorder(root);
    
    return 0;

}
#endif

// gcc -DTEST_BST bst.c -o bst