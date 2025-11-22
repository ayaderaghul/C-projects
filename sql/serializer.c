#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// --- CONFIGURATION ---
#define ORDER 3
#define PAGE_SIZE 4096 // A standard hard drive block is 4KB

// 1. THE DISK-READY NODE
typedef struct Node {
    bool isLeaf;
    int count;
    int keys[ORDER];
    int children[ORDER + 1]; // Now storing Integers (Page IDs), not Pointers!
} Node;

// 2. THE PACKER (Serialize)
// Goal: Turn the Struct into a generic byte array (buffer)
void serialize(Node* node, char* buffer) {
    // We use a 'cursor' to track where we are writing in the buffer
    char* cursor = buffer;

    // A. Pack the Metadata
    // Copy the 'isLeaf' boolean (1 byte)
    memcpy(cursor, &(node->isLeaf), sizeof(bool));
    cursor += sizeof(bool); // Move cursor forward

    // Copy the 'count' (4 bytes)
    memcpy(cursor, &(node->count), sizeof(int));
    cursor += sizeof(int);

    // B. Pack the Keys
    // We copy the whole array at once
    memcpy(cursor, node->keys, sizeof(int) * ORDER);
    cursor += sizeof(int) * ORDER;

    // C. Pack the Children (Page IDs)
    memcpy(cursor, node->children, sizeof(int) * (ORDER + 1));
    
    printf("[Packer] Node flattened into bytes.\n");
}

// 3. THE UNPACKER (Deserialize)
// Goal: Turn raw bytes back into a usable Struct
void deserialize(char* buffer, Node* node) {
    char* cursor = buffer;

    // A. Unpack Metadata
    memcpy(&(node->isLeaf), cursor, sizeof(bool));
    cursor += sizeof(bool);

    memcpy(&(node->count), cursor, sizeof(int));
    cursor += sizeof(int);

    // B. Unpack Keys
    memcpy(node->keys, cursor, sizeof(int) * ORDER);
    cursor += sizeof(int) * ORDER;

    // C. Unpack Children
    memcpy(node->children, cursor, sizeof(int) * (ORDER + 1));
    
    printf("[Unpacker] Bytes restored into Node.\n");
}

// --- MAIN TEST ---
int main() {
    // 1. Create a Node and fill it with data
    Node original;
    original.isLeaf = false; // It's a Manager
    original.count = 2;
    original.keys[0] = 10; 
    original.keys[1] = 20;
    // These are "Page IDs" now (e.g., Page 5, Page 8, Page 12)
    original.children[0] = 5; 
    original.children[1] = 8;
    original.children[2] = 12;

    printf("Original Node: Keys [%d, %d], Children Points to Pages [%d, %d, %d]\n", 
           original.keys[0], original.keys[1], 
           original.children[0], original.children[1], original.children[2]);

    // 2. THE FLAT PACKING (Simulate writing to Disk)
    // We create a 4KB buffer (like a page on a hard drive)
    char diskPage[PAGE_SIZE]; 
    
    // Squash the node into the buffer
    serialize(&original, diskPage);

    // ---------------------------------------------------------
    // IMAGINE WE TURNED OFF THE COMPUTER HERE
    // The 'original' struct is gone. Only 'diskPage' remains.
    // ---------------------------------------------------------

    // 3. THE RESTORATION (Simulate reading from Disk)
    Node restored;
    
    // Read from the buffer back into the struct
    deserialize(diskPage, &restored);

    // 4. Verify
    printf("\nRestored Node: Keys [%d, %d], Children Points to Pages [%d, %d, %d]\n", 
           restored.keys[0], restored.keys[1], 
           restored.children[0], restored.children[1], restored.children[2]);

    return 0;
}