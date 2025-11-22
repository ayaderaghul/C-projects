#include <stdio.h>
#include <time.h>
#include <stdlib.h>

int main() {
    // random numbers in a range
    srand(time(NULL));

    int min = 0, max = 10;

    int r = rand() % (max - min + 1) + min;

    // guess until correct
    int guess = min - 1;
    while(1) {
        if(scanf("%d", &guess) != 1) {
            printf("scanf failed\n");
            return 1;
        } 
        
        if (guess > r) printf("too big\n");
        else if (guess < r) printf("too small\n");
        else {
            printf("you won\n");
            break;
        }
    }
    return 0;
}