#include <stdio.h>
#include <stdlib.h>

int main(void){
    const char *filename = "numbers.bin";
    FILE *fp;

    int numbers[10];
    for (int i= 0; i < 10; i++) {
        numbers[i] = i + 1;
    }

    fp = fopen(filename, "wb");
    if(!fp) {
        perror("error opening file");
        return 1;
    }

    size_t written = fwrite(numbers, sizeof(int), 10, fp);

    if(written!=10) {
        fprintf(stderr, "error writing");
        fclose(fp);
        return 1;
    }

    fclose(fp);


    int readNumbers[10];
    fp = fopen(filename, "rb");
    if(!fp) {
        perror("error opening file");
        return 1;
    }

    size_t read = fread(readNumbers, sizeof(int), 10, fp);
    if(read!=10){
        fprintf(stderr, "error reading");
        fclose(fp);
        return 1;
    }

    fclose(fp);

    printf("numbers read from binary file: ");

    for(int i = 0; i< 10; i++) {
        printf("%d ", readNumbers[i]);
    }

    printf("\n");
    return 0;

}