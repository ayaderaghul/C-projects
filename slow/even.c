#include <stdio.h>

int main() {
    int num;
    if(scanf("%d", &num) !=1) {
        printf("invalid input\n");
        return 1;
    }

    printf( "%s\n", (num % 2 == 0) ? "even\n" : "odd\n");
    // odd numbers have last bit 1, even 0;
    // (num & 1) == 0 ? even : odd
    return 0;
}