#include <stdio.h>
#include <time.h>
#include <stdlib.h>
int main() {
    // get a random number in the range min max
    srand(time(NULL));

    int min = 0, max = 10;

    int r = rand() % (max - min + 1) + min;
    int num;
    for(int i = 0; i < 3 ; i++ ) {
        scanf("%d", &num);
        if(num>r)
            printf("too big");
        else if (num < r)
            printf("too small");
        else {
            printf("you won");
            return 0;
        }
    }
    printf("you lost");
    return 0;
}