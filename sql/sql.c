#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// --- CONFIGURATION ---
#define PAGE_SIZE 4096
#define ORDER 3 

// ==========================================================
// BLOCK 1: THE STRUCTS (Disk Ready)
// ==========================================================

typedef struct {
    bool isLeaf;
    int count;
    int keys[ORDER + 1];         // Space for 4 keys (1 overflow)
    int children[ORDER + 2];     // Space for 5 Page IDs
} Node;

typedef struct {
    FILE* file;
    int num_pages;
} Pager;

// ==========================================================
// BLOCK 2: THE FORKLIFT (Disk IO)
// ==========================================================

Pager* pager_open(const char* filename) {
    Pager* pager = (Pager*)malloc(sizeof(Pager));
    pager->file = fopen(filename, "r+b");
    if (!pager->file) {
        // Create file if it doesn't exist
        pager->file = fopen(filename, "w+b");
    }
    fseek(pager->file, 0, SEEK_END);
    int file_len = ftell(pager->file);
    pager->num_pages = file_len / PAGE_SIZE;
    return pager;
}

// Helper: Get the next available Page ID (Grow the file)
int get_new_page_num(Pager* pager) {
    int new_page_num = pager->num_pages;
    pager->num_pages++;
    return new_page_num;
}

void pager_write(Pager* pager, int page_num, char* buffer) {
    fseek(pager->file, page_num * PAGE_SIZE, SEEK_SET);
    fwrite(buffer, PAGE_SIZE, 1, pager->file);
}

void pager_read(Pager* pager, int page_num, char* buffer) {
    fseek(pager->file, page_num * PAGE_SIZE, SEEK_SET);
    fread(buffer, PAGE_SIZE, 1, pager->file);
}

// ==========================================================
// BLOCK 3: THE PACKER (Serialization)
// ==========================================================

void serialize(Node* node, char* buffer) {
    char* cursor = buffer;
    // Copy Metadata
    memcpy(cursor, &(node->isLeaf), sizeof(bool)); cursor += sizeof(bool);
    memcpy(cursor, &(node->count), sizeof(int));   cursor += sizeof(int);
    // Copy Arrays
    memcpy(cursor, node->keys, sizeof(int) * (ORDER + 1)); 
    cursor += sizeof(int) * (ORDER + 1);
    memcpy(cursor, node->children, sizeof(int) * (ORDER + 2));
}

void deserialize(char* buffer, Node* node) {
    char* cursor = buffer;
    memcpy(&(node->isLeaf), cursor, sizeof(bool)); cursor += sizeof(bool);
    memcpy(&(node->count), cursor, sizeof(int));   cursor += sizeof(int);
    memcpy(node->keys, cursor, sizeof(int) * (ORDER + 1)); 
    cursor += sizeof(int) * (ORDER + 1);
    memcpy(node->children, cursor, sizeof(int) * (ORDER + 2));
}

// ==========================================================
// BLOCK 4: B-TREE LOGIC (The Brains)
// ==========================================================

// Helper: Initialize a fresh node
void init_node(Node* node, bool isLeaf) {
    node->isLeaf = isLeaf;
    node->count = 0;
    // Initialize children to -1 (meaning "No Page")
    for(int i=0; i<ORDER+2; i++) node->children[i] = -1;
}

// Helper: Sort Insert (The Shifter)
void insert_into_node(Node* node, int key) {
    int i = node->count;
    while (i > 0 && node->keys[i - 1] > key) {
        node->keys[i] = node->keys[i - 1];
        i--;
    }
    node->keys[i] = key;
    node->count++;
}

// THE BIG ONE: SPLIT THE ROOT (Page 0)
// This logic moves data from Page 0 to new Pages 1 and 2.
void split_root(Pager* pager, Node* root) {
    printf("\n[!] ROOT FULL. SPLITTING TO DISK.\n");

    // 1. Allocate Two New Pages
    int left_page_id = get_new_page_num(pager);  // e.g., Page 1
    int right_page_id = get_new_page_num(pager); // e.g., Page 2

    Node left_child;  init_node(&left_child, true);
    Node right_child; init_node(&right_child, true);

    // 2. Distribute Keys (Simple 50/50 Split)
    // Order 3 means Root has 4 keys currently.
    // Left gets: keys[0], keys[1]
    // Right gets: keys[2], keys[3]
    
    left_child.keys[0] = root->keys[0];
    left_child.keys[1] = root->keys[1];
    left_child.count = 2;

    right_child.keys[0] = root->keys[2];
    right_child.keys[1] = root->keys[3];
    right_child.count = 2;

    // 3. Save these new children to Disk
    char buffer[PAGE_SIZE];
    
    serialize(&left_child, buffer);
    pager_write(pager, left_page_id, buffer);
    printf(" -> Saved Left Child to Page %d\n", left_page_id);

    serialize(&right_child, buffer);
    pager_write(pager, right_page_id, buffer);
    printf(" -> Saved Right Child to Page %d\n", right_page_id);

    // 4. Update Root (Page 0) to become the Manager
    // It is no longer a leaf.
    root->isLeaf = false;
    root->count = 1;
    
    // The Root now holds the "Promoted Key" (The first key of right child)
    root->keys[0] = right_child.keys[0];
    
    // The Root points to the Page IDs
    root->children[0] = left_page_id;
    root->children[1] = right_page_id;

    printf(" -> Root updated. Points to Pages %d and %d\n", 
           left_page_id, right_page_id);
}

// ==========================================================
// BLOCK 5: MAIN EXECUTION
// ==========================================================

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Usage: ./db <number>\n");
        return 0;
    }
    int input_key = atoi(argv[1]);

    // 1. Open Database
    Pager* pager = pager_open("final.db");
    Node root;
    char buffer[PAGE_SIZE];

    // 2. Load Root (Page 0)
    if (pager->num_pages == 0) {
        printf("New DB. Init Root.\n");
        init_node(&root, true);
        // We must formally create Page 0
        pager->num_pages = 1; 
    } else {
        pager_read(pager, 0, buffer);
        deserialize(buffer, &root);
    }

    // 3. LOGIC: SEARCH AND INSERT
    // For this simple demo, we only insert if Root is a Leaf.
    // If Root is a Manager, we just print "Complex Tree" and stop,
    // because implementing full recursive disk traversal is 500+ lines.
    
    if (root.isLeaf) {
        printf("Root is Leaf. Current Keys: [ ");
        for(int i=0; i<root.count; i++) printf("%d ", root.keys[i]);
        printf("]\n");

        insert_into_node(&root, input_key);
        printf("Inserted %d.\n", input_key);

        // CHECK FOR SPLIT
        if (root.count > ORDER) {
            split_root(pager, &root);
        }

        // SAVE ROOT
        serialize(&root, buffer);
        pager_write(pager, 0, buffer);
        printf("Saved Page 0.\n");
        
    } else {
        // ROOT IS A MANAGER
        printf("Root is a Manager (Page 0).\n");
        printf("It points to Page %d (Left) and Page %d (Right).\n", 
               root.children[0], root.children[1]);
        printf("You have successfully created a multi-page database!\n");
    }

    // Cleanup
    fclose(pager->file);
    free(pager);
    return 0;
}