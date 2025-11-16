#include <stdio.h>
#include <limits.h>

int findMax(int arr[], int size) {
    if (size < 1)
        return INT_MIN;
    if (size == 1)
        return arr[0];

    int max = arr[0];
    for (int i = 0; i < size; i++) {
        if (arr[i] > max)
            max = arr[i];
    }
    return max;
}

int main() {
    int arr[5] = {4,5,2,6,7};
    int m = findMax(arr, 5);
    printf("max: %d\n", m);
    return 0;
}