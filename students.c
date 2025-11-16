#include <stdio.h>
#include <string.h>

#define MAX_STUDENTS 100

// Define the student structure
struct Student {
    int roll;
    char name[50];
    float marks;
};

// Function to add a student
void addStudent(struct Student students[], int *count) {
    if (*count >= MAX_STUDENTS) {
        printf("Cannot add more students.\n");
        return;
    }
    struct Student s;
    printf("Enter Roll Number: ");
    scanf("%d", &s.roll);
    printf("Enter Name: ");
    scanf(" %[^\n]", s.name);  // read string with spaces
    printf("Enter Marks: ");
    scanf("%f", &s.marks);

    students[*count] = s;
    (*count)++;
    printf("Student added successfully!\n");
}

// Function to display all students
void displayStudents(struct Student students[], int count) {
    printf("\n--- Student Records ---\n");
    for (int i = 0; i < count; i++) {
        printf("Roll: %d | Name: %s | Marks: %.2f\n",
               students[i].roll, students[i].name, students[i].marks);
    }
}

// Function to search a student by roll number
void searchStudent(struct Student students[], int count, int roll) {
    for (int i = 0; i < count; i++) {
        if (students[i].roll == roll) {
            printf("Found: Roll: %d | Name: %s | Marks: %.2f\n",
                   students[i].roll, students[i].name, students[i].marks);
            return;
        }
    }
    printf("Student with roll %d not found.\n", roll);
}

int main() {
    struct Student students[MAX_STUDENTS];
    int count = 0;
    int choice, roll;

    do {
        printf("\n--- Student Record System ---\n");
        printf("1. Add Student\n");
        printf("2. Display Students\n");
        printf("3. Search Student\n");
        printf("4. Exit\n");
        printf("Enter choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                addStudent(students, &count);
                break;
            case 2:
                displayStudents(students, count);
                break;
            case 3:
                printf("Enter roll number to search: ");
                scanf("%d", &roll);
                searchStudent(students, count, roll);
                break;
            case 4:
                printf("Exiting...\n");
                break;
            default:
                printf("Invalid choice.\n");
        }
    } while (choice != 4);

    return 0;
}
