#include <stdio.h>
#include <limits.h>

int main() {
    int num;
    int times = 0;
    int sum = 0, max = INT_MIN;
    float avg = (float)INT_MIN;
    while (times < 5 && scanf("%d", &num) != -1) {
        sum += num;
        if (num > max)
            max = num;
        times++;
    }

    printf("max: %d\navg: %.2f\n", max, (float)sum / 5);
    return 0;
}