#include <stdio.h>

void swap (int*a, int*b) {
    int tmp = *a;
    *a = *b;
    *b = tmp;
    return;
}

int main() {

    int a = 4, b = 5;
    swap(&a,&b);
    printf("a: %d, b: %d\n", a,b);
    return 0;
}