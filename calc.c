#include <stdio.h>
#include <stdlib.h>

int main(void) {
    double num1, num2;
    char op;

    printf("enter an expression: 3 + 4:\n");
    if(scanf("%lf %c %lf", &num1, &op, &num2) != 3) {
        fprintf(stderr, "invalid input\n");
        return 1;
    }

    double result;
    switch(op) {
        case '+':
            result = num1 + num2;
            break;
        case '-':
            result = num1 - num2;
            break;
        case '*':
            result = num1 * num2;
            break;
        case '/':
            result = num1 / num2;
            break;
        default:
            fprintf(stderr, "unknown operator %c", op);
            return 1;
    }

    printf("result: %.2f", result);
    return 0;
}
