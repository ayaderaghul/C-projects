#include <stdio.h>

int sum(int arr[], int size) {
    int s = 0;
    for (int i = 0 ; i < size; i++)
        s += arr[i];

    return s;
}

int main() {
    int arr[5] = {3,5,4,6,8};
    int s = sum(arr, 5);
    float avg = s / 5;
    printf("%.2f", avg);

    return 0;

}