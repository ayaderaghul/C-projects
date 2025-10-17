#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    char line[256];
    int *numbers = NULL;
    int count = 0;
    int capacity = 0;

    if(!fgets(line, sizeof(line), stdin)) {
        perror("error reading input");
        return 1;
    }

    char *token = strtok(line, " \n\t");

    while(token != NULL) {
        int value = atoi(token) ;
        if(count >= capacity) {
            capacity = capacity == 0 ? 4 : capacity * 2;
            int *temp = realloc(numbers, capacity * sizeof(int));
            if(!temp) {
                perror("malloc failed");
                free(numbers);
                return 1;
            }
            numbers = temp;
        }

        numbers[count++] = value;
        token=strtok(NULL, " \n\t");
    }

    if (count == 0) {
        printf("no numbers provided\n");
        free(numbers);
        return 0;
    }

    int max = numbers[0];
    long sum = numbers[0];
    for (int i = 1; i < count;i++) {
        if(numbers[i] > max) max = numbers[i];
        sum += numbers[i];
    }

    double average = (double) sum / count;

    printf("count: %d\n", count);
    for (int i = 0; i<count;i++){
        printf("%d ", numbers[i]);
    }
    printf("\n");

    printf("average: %.2f, largest: %d\n",average, max);
    free (numbers);
    return 0;
}
