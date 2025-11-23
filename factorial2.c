#include <stdio.h>
int factorial(int n) {
    if(n == 1) {
        return 1;
    }
    // O(n) O(n) - stack can overflow
    return n * factorial (n - 1);
}
int main() {
    int num;
    if(scanf("%d", &num)!=1){
        printf("invalid input");
        return 1;
    }
    int total = factorial(num);
    printf("%d", total);
    return 0;
}