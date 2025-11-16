#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
int isIn(int n, int arr[], int size) {
    for (int i = 0; i< size;i++)
        if (n == arr[i])
            return i;
    return -1;
}
int main() {
    int arr[] = {3,4,5,4,5,6,7};
    int size = 7;

    int seen[size];

    int res[size];
    for (int i = 0; i < size; i++) { 
        res[i] = INT_MAX;
        seen[i] = INT_MAX;
    }
    
    for (int j = 0; j< size; j++) {
        if(isIn(arr[j], seen, size) != -1)
            res[j] = 1;
        else
            res[j] = 0;
        seen[j] = arr[j];
    }

    printf("printing result..\n");
    for (int k = 0; k < size; k++) {
        if (res[k] == 1)
            printf("%d ", arr[k]);
    }

    return 0;

}