#ifndef HASHTABLE_H
#define HASHTABLE_H

// each entry has a key and a value, with pointer to the next one
// separate chaining (to address collision)
typedef struct Entry {
    char *key;
    char *value;
    struct Entry *next;
} Entry;

// table of different buckets (index)
// each bucket hold pointer(s) to (a linked list of) an entry
typedef struct HashTable {
    Entry **buckets;
    int size;
} HashTable;

typedef struct{
    char key[64];
    char value[64];
} DiskEntry;

unsigned int hash(char*key, int table_size);
HashTable* create_table(int size);
void destroy_table(HashTable*table);
void set(HashTable*table, char*key, char*value);
char*get(HashTable *table, char*key);
int delete(HashTable*table, char*key);

void save_to_disk();

#endif