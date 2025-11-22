#include <stdio.h>

int main(){
    FILE *file = fopen("output.txt", "w");

    if (file == NULL) {
        printf("fopen failed");
        return 1;
    }
    int b = 20;
    fprintf(file, "%d\n", b);
    printf("data written successfully\n");

    fclose(file);

    // open to read from file:

    FILE *fptr;
    fptr = fopen("output.txt", "r");

    int a;

    fscanf(fptr, "%i\n", &a);

    printf("number: %d\n", a);


    return 0;

}