#include <stdio.h>

typedef struct {
    char* name;
    int rollNum;
    float mark;
} Student;

int main() {
    Student A;
    A.name = "Chi";
    A.rollNum = 3;
    A.mark= 9.8;

    printf("name: %s, rollNum: %d, mark: %.2f", A.name, A.rollNum, A.mark);
    return 0;
}