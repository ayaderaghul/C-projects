#include <stdio.h>

int main() {

    int num;
    if(scanf("%d", &num)!=1){
        printf("invalid input");
        return 1;
    }

    if (num < 0) {
        fprintf(stderr, "invalid number");
        return 1;
    }

    int a=0, b =1, c;

    for (int i = 2; i <= num; i++) {
        c = a + b;
        a = b; 
        b = c;
    }

    printf("%d", c);
    return 0;
}