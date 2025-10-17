#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main(void) {
    int num;
    if(scanf("%d", &num) != 1) {
        fprintf(stderr, "invalid input");
        return 1;
    }

    if (num < 2) {
        printf("%d is not prime", num);
        return 0;
    }

    if(num==2) {
        printf("2 is prime");
        return 0;
    }

    if(num % 2 == 0) {
        printf("%d is not prime", num);
        return 0;
    }

    int limit = (int) sqrt(num);
    
    for(int i = 3; i<= limit; i+=2) {
        if (num % i == 0) {
            printf("%d is not prime", num);
            return 0;
        }
    }

    printf("%d is prime",num);
    return 0;
}