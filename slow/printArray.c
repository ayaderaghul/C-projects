#include <stdio.h>
void printArray(int arr[], int size) {
    for (int i = 0; i < size; i++)
        printf("%d ", arr[i]);
    return;
}
int main(){
    int arr[5] = {3,6,7,8,7};
    printArray(arr, 5);
    return 0;
}