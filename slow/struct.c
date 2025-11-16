#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Person {
    char name[20];
    int age;
};

int main() {
    struct Person *p;
    strcpy(p->name, "Chi");
    p->age = 20;

    printf("person p: name: %s, age: %d\n", p->name, p->age);
    return 0;
}