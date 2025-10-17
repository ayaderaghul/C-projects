#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void) {
    char line[256];
    
    // read the line safely
    if(!fgets(line, sizeof(line), stdin)) {
        fprintf(stderr, "error reading input\n");
        return 1;
    }

    // remove new line
    line[strcspn(line, "\n")] = '\0';

    // split into tokens
    char *token = strtok(line, " ");
    if(token==NULL) {
        fprintf(stderr, "no numbers provided\n");
        return 1;
    }

    int count = 0; // for average stats
    long sum = 0; // use long to prevent overflow
    
    int largest = atoi(token); // initialize largest finding process

    while(token != NULL) {
        int num = atoi(token);
        sum += num;
        if(num > largest) largest = num;
        count++;
        token=strtok(NULL, " ");
    }

    double average = (count > 0) ? (double) sum / count : 0.0;

    printf("Count: %d, Average: %.2f, largest: %d\n", count, average, largest);

    return 0;
}