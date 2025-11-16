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

int hash(int key, int size) {
    return (key % size + size) % size;
}

HashMap *createMap(int size) {
    HashMap*map = malloc(sizeof(HashMap));
    map->size = size;
    map->table = calloc(size, sizeof(Entry));
    for(int i=0;i< size;i++) map->table[i].key = INT_MAX;
    return map;
}

void put(HashMap*map, int key, int val) {
    int idx = hash(key, map->size);
    while (map->table[idx].key != INT_MAX) {
        idx = (idx+1) % map->size; // linear probing
    }
    map->table[idx].key=key;
    map->table[idx].val=val;
}

int get(HashMap*map, int key) {
    int idx=hash(key, map->size);
    while (map->table[idx].key != INT_MAX) {
        if(map->table[idx].key==key) return map->table[idx].val;
        idx=(idx+1) % map->size;
    }
    return -1;
}

int *twoSum(int*nums,int numsSize, int target, int*returnSize) {
    HashMap*map = createMap(numsSize*2);
    int* result = malloc(2*sizeof(int));
    *returnSize=0;
    for (int i = 0; i < numsSize; i++) {
        int complement = target - nums[i];
        int idx = get(map, complement);
        if (idx != -1) {
            result[0] = idx;
            result[1] = i;
            *returnSize = 2;
            free(map->table);
            free(map);
            return result;
        }
        put(map, nums[i], i);
    }
    free(map->table);
    free(map);
    free(result);
    return NULL;
}

int main(){
    int nums[] = {0,4,3,0};
    int target = 0;
    int returnSize;
    int*res = twoSum(nums, 4, target, &returnSize);

    if(res){
        printf("indices: %d %d\n",  res[0], res[1]);
        free(res);
    } else {
        printf("no solution\n");
    }
    return 0;
}
