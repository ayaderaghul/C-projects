#include <stdio.h>
int memo[1000] = {0};

int fibonacci(int n) {
    if (n == 0 || n == 1) return n;
    if (memo[n] != 0) return memo[n];

    memo[n] = fibonacci(n-1) + fibonacci(n-2); 
    return memo[n];
}

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


    int f = fibonacci(num);
    printf("%d", f);
    return 0;
    
}