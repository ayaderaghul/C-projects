#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NUMS 100

int main(void){
    char line[256];
    int numbers[MAX_NUMS] = {0};
    int count = 0;

    if(!fgets(line, sizeof(line), stdin)) {
        fprintf(stderr, "error reading input\n");
        return 1;
    }

    line[strcspn(line, "\n")] = '\0';

    char *token = strtok(line, " ");
    while(token != NULL && count < MAX_NUMS) {
        numbers[count] == atoi(token);
        printf("%d ", numbers[count]);
        count++;
        token = strtok(NULL, " ");
    }

    if(count == 0) {
        fprintf(stderr, "no valid numbers entered");
        return 1;
    }

    int largest = numbers[0];
    long sum = 0;
    for (int i = 0; i < count; i++) {
        if(numbers[i] > largest) largest = numbers[i];
        sum += numbers[i];
    }

    double average = (double)sum / count;

    printf("count: %d\n", count);
    for (int i = 0; i < count; i++) {
        printf("%d ", numbers[i]);
    }
    printf("\n");

    printf("average: %.2f, largest %d\n", average, largest);
}