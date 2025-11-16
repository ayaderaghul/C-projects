#include <stdio.h>

int main(){
    int a = 0, b = 1, c;
    int n = 7;
    while (n > 1) {
        c = a + b;
        a = b;
        b = c;
        n--;
    }
    printf("%d\n", c);
    return 0;
}