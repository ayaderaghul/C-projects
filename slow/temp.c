#include <stdio.h>

int CtoF (int C) {
    return (C * 9) / 5 + 32;
}

int FtoC(int F) {
    return ((F-32)*5)/9;
}
int main() {
    int C = 0;
    int F = 70;

    printf("C to F: 0 -> %d\n", CtoF(C));
    printf("F to C: 70 -> %d\n", FtoC(F));
    return 0;
}