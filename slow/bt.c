#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// --- CONFIGURATION ---
// ORDER 3: Max 3 Keys. Splits on the 4th.
#define ORDER 3

// ==========================================================
// [BLOCK 1] THE VAULT (The Container)
// ==========================================================
typedef struct Node {
    bool isLeaf;           // True = Just Treasure. False = Treasure + Doors.
    int count;             // Number of keys currently inside
    int keys[ORDER + 1];   // The Treasure
    struct Node* children[ORDER + 2]; // The Doors
    struct Node* parent;   // The Boss
} Node;

Node* root = NULL;

// Helper: Build a new Vault
Node* createNode(bool isLeaf) {
    Node* n = (Node*)malloc(sizeof(Node));
    n->isLeaf = isLeaf;
    n->count = 0;
    n->parent = NULL;
    for(int i=0; i<ORDER+2; i++) n->children[i] = NULL; // Clear doors
    return n;
}

// ==========================================================
// [BLOCK 2] THE SHIFTER (Insert into Array)
// ==========================================================
// This puts a key (and its right-hand door) into a sorted node
void insertIntoArray(Node* node, int key, Node* rightChild) {
    int i = node->count;
    
    // "Scoot Over" Loop
    while (i > 0 && node->keys[i - 1] > key) {
        node->keys[i] = node->keys[i - 1];
        node->children[i + 1] = node->children[i]; // Move the door too
        i--;
    }
    
    // Place the Key
    node->keys[i] = key;
    // Place the Door (The Right Child)
    node->children[i + 1] = rightChild;
    
    node->count++;
}

// ==========================================================
// [BLOCK 3] THE PROMOTION (The Split Logic)
// ==========================================================

// Forward Declaration
void insertIntoParent(Node* leftChild, int key, Node* rightChild);

void splitNode(Node* oldNode) {
    // 1. Create the Right Sibling
    Node* newNode = createNode(oldNode->isLeaf);
    
    // 2. Find the Middle (The Divider)
    int mid = (ORDER + 1) / 2;
    int upKey = oldNode->keys[mid]; // THIS KEY IS LEAVING THE ROOM!

    // 3. Move the Right Half to the New Node
    // Note: We start at mid + 1. We SKIP the middle key.
    int j = 0;
    for (int i = mid + 1; i < oldNode->count; i++) {
        newNode->keys[j] = oldNode->keys[i];
        newNode->children[j] = oldNode->children[i];
        
        // Update parent pointer for the children moving over
        if (newNode->children[j]) {
            newNode->children[j]->parent = newNode;
        }
        j++;
    }
    
    // Move the final door (The far right child)
    newNode->children[j] = oldNode->children[oldNode->count];
    if (newNode->children[j]) {
        newNode->children[j]->parent = newNode;
    }

    // 4. Update Counts
    // Old node is cut off AT the middle. 
    // 'upKey' is effectively erased from here.
    oldNode->count = mid; 
    newNode->count = j;

    // 5. Send 'upKey' upstairs
    if (oldNode->parent) {
        insertIntoParent(oldNode, upKey, newNode);
    } else {
        // No Boss? Create a New Root.
        Node* newRoot = createNode(false);
        newRoot->keys[0] = upKey;
        newRoot->children[0] = oldNode;
        newRoot->children[1] = newNode;
        newRoot->count = 1;
        
        oldNode->parent = newRoot;
        newNode->parent = newRoot;
        root = newRoot;
    }
}

// ==========================================================
// [BLOCK 4] THE MANAGER (Parent Logic)
// ==========================================================
void insertIntoParent(Node* leftChild, int key, Node* rightChild) {
    Node* parent = leftChild->parent;
    
    // Use the Shifter to put the promoted key into the parent
    insertIntoArray(parent, key, rightChild);
    rightChild->parent = parent;
    
    // If the Parent gets too full, Split the Parent! (Recursion)
    if (parent->count > ORDER) {
        splitNode(parent);
    }
}

// ==========================================================
// [BLOCK 5] THE DRIVER (Main Logic)
// ==========================================================

void insert(int key) {
    if (!root) {
        root = createNode(true);
        root->keys[0] = key;
        root->count = 1;
        return;
    }
    
    // 1. Drill Down to find the correct Leaf
    Node* curr = root;
    while (!curr->isLeaf) {
        int i = 0;
        while (i < curr->count && key > curr->keys[i]) {
            i++;
        }
        curr = curr->children[i];
    }
    
    // 2. Insert into the Leaf
    insertIntoArray(curr, key, NULL);
    
    // 3. Check for Overflow
    if (curr->count > ORDER) {
        splitNode(curr);
    }
}

// --- VISUALIZATION ---
void printTree(Node* node, int level) {
    if (!node) return;
    
    // Print Right Half first (so it looks correct sideways)
    int i;
    for (i = node->count; i > 0; i--) {
        printTree(node->children[i], level + 1);
        
        // Indent
        for (int k = 0; k < level; k++) printf("      ");
        printf("[%d]\n", node->keys[i - 1]);
    }
    // Print Leftmost child
    printTree(node->children[0], level + 1);
}

int main() {
    printf("--- B-TREE DEMO ---\n");
    
    // 1. Fill the root
    insert(10); insert(20); insert(30);
    printf("Step 1: Root filled [10, 20, 30]\n");
    printTree(root, 0);
    printf("----------------\n");

    // 2. Split (Promote 20)
    // 20 goes UP. 10 stays Left. 30 stays Right. 40 goes Right.
    insert(40);
    printf("Step 2: Split (20 promoted)\n");
    printTree(root, 0);
    printf("----------------\n");

    // 3. Add more (Fill right side)
    insert(50); insert(60);
    printf("Step 3: Right side filled\n");
    printTree(root, 0);
    printf("----------------\n");
    
    // 4. Split Right Side (Promote 50)
    // 50 goes up to join 20.
    insert(70);
    printf("Step 4: Double Split (50 joins 20)\n");
    printTree(root, 0);

    return 0;
}