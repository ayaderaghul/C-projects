#include <stdio.h>
int add(int a, int b) {
    return a + b;
}
int main () {
    int a, b;
    if (scanf("%d", &a) != 1 || scanf("%d", &b) != 1) {
        printf("scanf failed");
        return 1;
    }
    printf("add: a + b = %d\n", add(a, b));
    return 0;
}