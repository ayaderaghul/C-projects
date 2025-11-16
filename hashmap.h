#ifndef HASHMAP_H
#define HASHMAP_H

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

typedef struct {
    int key;
    int val;
} Entry;

typedef struct {
    Entry *table;
    int size;
} HashMap;

int safe_mod(int key, int size);

HashMap *createMap(int size);

void put(HashMap*map, int key, int val);

int get(HashMap * map, int key);

void freeMap(HashMap *map);

#endif