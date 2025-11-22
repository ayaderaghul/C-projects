#define ORDER 4 // max 3 keys per node

typedef struct BNode {
    int keys[ORDER-1];
    struct BNode *children[ORDER];
    int num_keys;
    int is_leaf;
} BNode;

