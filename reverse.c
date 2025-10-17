#include <stdio.h>

void swap(int *a, int *b) {
    int tmp = *a;
    *a = *b;
    *b = tmp;
}

void reverse(int *arr, int size) {
    for(int i = 0; i <size / 2;i++) {
        swap(&arr[i], &arr[size-i-1]);
    }
}

void printArray(int* arr, int size) {
    for (int i =0;i<size;i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
}


int main() {
    int arr[] = {5,4,3,6,7};
    printArray(arr, 5);

    reverse(arr, 5);
    printArray(arr, 5);

    return 0;
}