#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#include "hashmap.h"

int safe_mod(int key, int size) {
    return (key % size + size) % size;
}

HashMap *createMap(int size) {
    HashMap * map = malloc(sizeof(HashMap));
    map->size = size;
    map->table = calloc(size,sizeof(Entry));
    for(int i = 0; i < size; i++) map->table[i].key = INT_MAX; // sentinel
    return map;
}

void put(HashMap*map, int key, int val) {
    int idx = safe_mod(key, map->size);
    while(map->table[idx].key != INT_MAX) idx = (idx + 1) % map->size;
    map->table[idx].key=key;
    map->table[idx].val=val;
}

int get(HashMap * map, int key) {
    int idx = safe_mod(key,map->size);
    while (map->table[idx].key != INT_MAX) {
        if(map->table[idx].key == key) return map->table[idx].val;
        idx = (idx+1) % map->size;
    }
    return -1;
}

void freeMap(HashMap *map) {
    free(map->table);
    free(map);
}

