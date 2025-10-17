#include <stdio.h>

int main() {
    int num;
    if(scanf("%d", &num) != 1) {
        fprintf(stderr,"invalid input");
        return 1;
    }

    if(num%2==0){
        printf("Even");
    } else {
        printf("Odd");
    }

    return 0;
}