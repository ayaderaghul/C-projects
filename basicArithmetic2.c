#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    if(argc !=3) {
        fprintf(stderr, "Usage: %s <num1> <num2>\n", argv[0]);
        return 1;
    }

    int num1 = atoi(argv[1]);
    int num2 = atoi(argv[2]);

    if (num2 == 0) {
        fprintf(stderr, "Error: division by zero\n");
        return 1;
    }

    printf("Sum: %d, Difference: %d, Product: %d, Quotient: %d\n", num1 + num2, num1 - num2, num1 * num2, num1 / num2);
    return 0;
}