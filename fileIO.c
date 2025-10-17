#include <stdio.h>
#include <stdlib.h>

int main(void){
    const char* filename = "numbers.txt";
    FILE *fp;

    fp = fopen(filename, "w");
    if(!fp) {
        perror("error opening file for writing");
        return 1;
    }

    for (int i=1;i<10;i++) {
        fprintf(fp, "%d\n", i);
    }

    fclose(fp);

    fp = fopen(filename, "r");
    if(!fp) {
        perror("error opening for reading file");
        return 1;
    }

    printf("numbers read from file: ");
    int num;
    while(fscanf(fp, "%d", &num) ==1) {
        printf("%d ", num);
    }
    printf("\n");

    fclose(fp);
    return 0;

}