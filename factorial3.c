#include <stdio.h>
int main(){
    int n = 5;
    int prod = 1;
    while (n > 1) prod *=n--;
    printf("%d\n",prod);
    return 0;
}