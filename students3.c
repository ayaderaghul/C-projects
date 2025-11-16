#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STUDENTS 100

struct Student {
    int roll;
    char name[50];
    float marks;
};

void addStudent(struct Student students[], int *count) {
    if(*count >= MAX_STUDENTS) {
        printf("Cannot add more. List full.\n");
        return;
    }
    struct Student s;
    scanf("%d", &s.roll);
    scanf(" %[^\n]", s.name);
    scanf("%f", &s.marks);

    students[*count] = s;
    (*count)++;
    printf("student added succesfully\n");
}

void displayStudents(struct Student students[], int count){
    for (int i = 0; i < count; i++) {
        printf("Roll: %d Name: %s Marks: %.2f\n", students[i].roll, students[i].name, students[i].marks);
    }
}

void searchStudents(struct Student students[], int count, int roll) {
    for (int i = 0; i < count; i++) {
        if (students[i].roll == roll) {
            printf("Found: %d %s %.2f\n", students[i].roll, students[i].name, students[i].marks);
            return;
        }
    }
    printf("student with roll %d not found\n", roll);
}

int main() {
    struct Student students[MAX_STUDENTS];
    int count = 0;
    int choice, roll;

    do {
        printf("1. add student. 2. display students. 3 search student. 4 exit. enter choice: ");
        scanf("%d", &choice);

        switch(choice) {
            case 1:
                addStudent(students, &count);
                break;
            case 2: 
                displayStudents(students, count);
                break;
            case 3:
                scanf("%d", &roll);
                searchStudents(students, count, roll);
            case 4:
                printf("Exiting..\n");
                break;
            default:
                printf("invalid choice\n");
        }
    } while (choice != 4);
    return 0;
}