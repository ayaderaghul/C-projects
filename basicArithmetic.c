#include <stdio.h>
#include <stdlib.h>

int main() {
    int num1, num2;
    if(scanf("%d", &num1) != 1 || scanf("%d", &num2) != 1){
        fprintf(stderr, "invalid input\n");
        return 1;
    }

    if (num2 == 0) {
        fprintf(stderr, "Error: Division by zero\n");
        return 1;
    }

    printf("Sum: %d, Difference: %d, Product: %d, Quotient: %d",
    num1 + num2, num1 - num2, num1 * num2, num1 / num2);

    return 0;
}