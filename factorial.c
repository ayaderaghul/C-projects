#include <stdio.h>

int main() {
    int num;
    if(scanf("%d", &num)!=1){
        printf("invalid input");
        return 1;
    }

    //O(n) O(1)
    int total = 1;
    for (int i = 1; i <= num; i++) {
        total *= i;
    }

    printf("%d", total);
    return 0;
}