#include <stdio.h>
#include <stdlib.h>

int main() {
    int*arr = malloc(sizeof(int) * 3);

    for (int i = 0; i < 3; i++) {
        arr[i] = i;
        printf("%d ", arr[i]);
    }

    return 0;
}