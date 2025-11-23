#include <stdio.h>

int main(){
    int n = 10;
    for (int i = 1; i< n; i++) {
        printf("debug: i: %d", i);
        for (int k = n-i; k >0;k--)
            printf(" ");
        for (int j = 1; j < i; j++) 
            printf("--");
        printf("\n");
    }
    return 0;
}