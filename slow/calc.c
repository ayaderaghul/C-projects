#include <stdio.h>

int main() {
    int n1, n2;
    char op;

    if(scanf("%d %c %d", &n1, &op, &n2)!= 3){
        printf("scanf failed\n");
        return 1;
    }
    switch (op){
        case '+':
            printf("%d + %d = %d", n1, n2, n1 + n2);
            break;
        case '-':
            printf("%d - %d = %d", n1, n2, n1 - n2);
            break;
        case '*':
            printf("%d * %d = %d", n1, n2, n1 * n2);
            break;
        case '/':
            if (n2 == 0) {
                printf("error: division by zero\n");
                return 1;
            }
            printf("%d / %d = %d", n1, n2, n1 / n2);
            break;
        default:
            printf("invalid input\n");
    }
    return 0;
}