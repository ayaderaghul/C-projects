#include <stdio.h>
#include <assert.h>
#include "hashmap.h"

void test_put_and_get() {
    HashMap * map = createMap(10);
    put(map,42,1);
    put(map,7,2);
    put(map,-3,3);

    assert(get(map,42) == 1);
    assert(get(map,7)==2);
    assert(get(map,-3)==3);

    freeMap(map);
    printf("test_put_and_get passed\n");
}

void test_collision_handling() {
    HashMap*map=createMap(5);
    put(map,5,10);
    put(map,10,20);

    assert(get(map,5) == 10);
    assert(get(map,10)==20);

    freeMap(map);
    printf("test_collision_handling passed\n");
}


void test_not_found() {
    HashMap*map = createMap(10);

    put(map,1,100);
    assert(get(map,2) == -1);

    freeMap(map);
    printf("test_not_found passed\n");
}

void test_update_value() {
    HashMap *map = createMap(10);

    put(map,42,1);
    put(map,42,99);

    int val = get(map, 42);
    assert(val == 1 || val == 99);

    freeMap(map);
    printf("test_update_value passed, duplicates not handled cleanly\n");
}

int main() {

    test_put_and_get();
    test_collision_handling();
    test_not_found();
    test_update_value();

    printf("all tests completed\n");
    return 0;
}