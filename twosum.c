/**
 * Note: The returned array must be malloced, assume caller calls free().
 */
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

int isIn(int n, int arr[], int size)
{
    for (int i = 0; i < size; i++)
        if (n == arr[i]){
            return i;
        }
    
    return -1;
}

int *twoSum(int *nums, int numsSize, int target, int *returnSize)
{
    // allocate the seen array
    int seen[numsSize];
    for (int l = 0; l < numsSize; l++) 
        seen[l] = INT_MAX;
    // traverse the array, do the algo
    int n1, n2;
    int seenIdx;
    for (int j = 0; j < numsSize; j++)
    {
        n1 = nums[j];
        n2 = target - n1;
        if ((seenIdx = isIn(n2, seen, numsSize)) != -1)
        {
            *returnSize = 2;

            int *returnedArray = malloc(sizeof(int) * *returnSize);
            if (returnedArray == NULL)
            {
                printf("malloc failed");
                return NULL;
            }
            returnedArray[0] = seenIdx;
            returnedArray[1] = j;

            return returnedArray;
        }
        seen[j] = n1;
    }
    *returnSize = 0;
    return NULL;
}

int main()
{
    int nums[] = {0,4,3,0};
    int *returned;
    int returnedSize = 2;
    returned = twoSum(nums, 4, 0, &returnedSize);

    printf("returned: ");
    for (int i = 0; i < 2; i++)
        printf("%d ", returned[i]);
    return 0;
}