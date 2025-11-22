#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define ORDER 3


typedef struct Node {
    bool isLeaf;
    int keyCount;
    int keys[ORDER+1];

    struct Node* parent;
    struct Node*children[ORDER+2];

    struct Node*next;
} Node;

Node *root = NULL;

Node*createNode(bool isLeaf) {
    Node*newNode = (Node*)malloc(sizeof(Node));
    newNode->isLeaf = isLeaf;
    newNode->keyCount = 0;
    newNode->parent=NULL;
    newNode->next=NULL;
    return newNode;
}


void insertIntoArray(Node*node, int key, Node*rightChild) {
    int i = node->keyCount;

    while(i>0 && node->keys[i-1] >key) {
        node->keys[i] = node->keys[i-1];
        if(!node->isLeaf) {
            node->children[i+1]=node->children[i];
        }
        i--;
    }

    node->keys[i] = key;
    if(!node->isLeaf){
        node->children[i+1]=rightChild;
    }
    node->keyCount++;
}


void insertIntoParent(Node*leftChild, int key, Node*rightChild);

void splitInternal(Node*node) {
    Node* newInternal = createNode(false);
    int mid = (ORDER + 1) / 2;
    int upKey = node->keys[mid];

    int j = 0;
    for (int i = mid+1; i < node->keyCount;i++) {
        newInternal->keys[j] = node->keys[i];
        newInternal->children[j] = node->children[i];
        if(newInternal->children[j]) newInternal->children[j]->parent=newInternal;
        j++;
    }
    newInternal->children[j] = node->children[node->keyCount];
    if(newInternal->children[j]) newInternal->children[j]->parent = newInternal;

    node->keyCount = mid;
    newInternal->keyCount =j;

    if(node->parent) {
        insertIntoParent(node, upKey, newInternal);
    } else {
        Node *newRoot = createNode(false);
        newRoot->keys[0] = upKey;
        newRoot->children[0] = node;
        newRoot->children[1] = newInternal;
        newRoot->keyCount=1;
        node->parent = newRoot;
        newInternal->parent=newRoot;
        root=newRoot;
    }

}

void insertIntoParent(Node* leftChild, int key, Node* rightChild) {
    Node* parent = leftChild->parent;
    
    // Use Block 2 logic to insert the key into the Parent
    insertIntoArray(parent, key, rightChild);
    rightChild->parent = parent;
    
    // If Parent gets too full -> Block 4 Logic (Recursion)
    if (parent->keyCount > ORDER) {
        splitInternal(parent);
    }
}

// ==========================================================
// [BLOCK 3] THE SPLITTER (Leaf Logic)
// ==========================================================
void splitLeaf(Node* leaf) {
    Node* newLeaf = createNode(true);
    int mid = (ORDER + 1) / 2;
    
    // Copy Right Half
    int j = 0;
    for (int i = mid; i < leaf->keyCount; i++) {
        newLeaf->keys[j] = leaf->keys[i];
        j++;
    }
    
    leaf->keyCount = mid;
    newLeaf->keyCount = j;
    
    // Link Neighbors
    newLeaf->next = leaf->next;
    leaf->next = newLeaf;
    newLeaf->parent = leaf->parent;
    
    // Tell the Manager: "Here is the key (first of new leaf), and the new box"
    int newKey = newLeaf->keys[0];
    
    if (leaf->parent) {
        insertIntoParent(leaf, newKey, newLeaf);
    } else {
        // Create New Root (First split ever)
        Node* newRoot = createNode(false);
        newRoot->keys[0] = newKey;
        newRoot->children[0] = leaf;
        newRoot->children[1] = newLeaf;
        newRoot->keyCount = 1;
        leaf->parent = newRoot;
        newLeaf->parent = newRoot;
        root = newRoot;
    }
}

// ==========================================================
// MAIN DRIVER (The Traffic Cop)
// ==========================================================
void insert(int key) {
    if (!root) {
        root = createNode(true);
        root->keys[0] = key;
        root->keyCount = 1;
        return;
    }

    // 1. Find the correct leaf (Drill down)
    Node* curr = root;
    while (!curr->isLeaf) {
        int i = 0;
        while (i < curr->keyCount && key >= curr->keys[i]) i++;
        curr = curr->children[i];
    }

    // 2. Insert into Leaf (Block 2)
    insertIntoArray(curr, key, NULL);

    // 3. Split if full (Block 3)
    if (curr->keyCount > ORDER) {
        splitLeaf(curr);
    }
}

void printLeaves() {
    Node* curr = root;
    // Go all the way left
    while (curr && !curr->isLeaf) curr = curr->children[0];
    
    printf("Leaves: ");
    while (curr) {
        printf("[");
        for(int i=0; i<curr->keyCount; i++) printf("%d", curr->keys[i]);
        printf("] -> ");
        curr = curr->next;
    }
    printf("END\n");
}

int main() {
    printf("--- B+ TREE ASSEMBLY ---\n");
    
    insert(10); insert(20); insert(30);
    printf("Step 1 (Full Box): "); printLeaves();
    
    insert(40); 
    printf("Step 2 (Split!):   "); printLeaves();
    
    insert(50); insert(60);
    printf("Step 3 (Split Again): "); printLeaves();

    // This forces the MANAGER to split (grow taller)
    insert(70);
    printf("Step 4 (Manager Split): "); printLeaves();
    
    return 0;
}