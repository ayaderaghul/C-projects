#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int roll;
    char name[50];
    float marks;
} Student;

// Function prototypes
void addStudent(Student **students, int *count);
void displayStudents(Student *students, int count);
void searchStudent(Student *students, int count, int roll);
void updateStudent(Student *students, int count, int roll);
void deleteStudent(Student **students, int *count, int roll);
void sortStudents(Student *students, int count, int byMarks);
void saveToFile(Student *students, int count, const char *filename);
void loadFromFile(Student **students, int *count, const char *filename);
// save to load from csv
int main() {
    Student *students = NULL;
    int count = 0;
    int choice, roll;

    loadFromFile(&students, &count, "students.dat");

    do {
        printf("\n--- Student Record System ---\n");
        printf("1. Add Student\n");
        printf("2. Display Students\n");
        printf("3. Search Student\n");
        printf("4. Update Student\n");
        printf("5. Delete Student\n");
        printf("6. Sort Students\n");
        printf("7. Save Records\n");
        printf("8. Exit\n");
        printf("Enter choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                addStudent(&students, &count);
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
                printf("Enter roll number to update: ");
                scanf("%d", &roll);
                updateStudent(students, count, roll);
                break;
            case 5:
                printf("Enter roll number to delete: ");
                scanf("%d", &roll);
                deleteStudent(&students, &count, roll);
                break;
            case 6:
                printf("Sort by (0=Roll, 1=Marks): ");
                scanf("%d", &roll);
                sortStudents(students, count, roll);
                break;
            case 7:
                saveToFile(students, count, "students.dat");
                break;
            case 8:
                saveToFile(students, count, "students.dat");
                printf("Exiting...\n");
                break;
            default:
                printf("Invalid choice.\n");
        }
    } while (choice != 8);

    free(students);
    return 0;
}

// Add student
void addStudent(Student **students, int *count) {
    *students = realloc(*students, (*count + 1) * sizeof(Student));
    if (*students == NULL) {
        printf("Memory allocation failed!\n");
        exit(1);
    }
    Student s;
    printf("Enter Roll Number: ");
    scanf("%d", &s.roll);
    printf("Enter Name: ");
    scanf(" %[^\n]", s.name);
    printf("Enter Marks: ");
    scanf("%f", &s.marks);

    (*students)[*count] = s;
    (*count)++;
    printf("Student added successfully!\n");
}

// Display students
void displayStudents(Student *students, int count) {
    printf("\n--- Student Records ---\n");
    for (int i = 0; i < count; i++) {
        printf("Roll: %d | Name: %s | Marks: %.2f\n",
               students[i].roll, students[i].name, students[i].marks);
    }
}

// Search student
void searchStudent(Student *students, int count, int roll) {
    for (int i = 0; i < count; i++) {
        if (students[i].roll == roll) {
            printf("Found: Roll: %d | Name: %s | Marks: %.2f\n",
                   students[i].roll, students[i].name, students[i].marks);
            return;
        }
    }
    printf("Student with roll %d not found.\n", roll);
}

// Update student
void updateStudent(Student *students, int count, int roll) {
    for (int i = 0; i < count; i++) {
        if (students[i].roll == roll) {
            printf("Enter new name: ");
            scanf(" %[^\n]", students[i].name);
            printf("Enter new marks: ");
            scanf("%f", &students[i].marks);
            printf("Record updated!\n");
            return;
        }
    }
    printf("Student not found.\n");
}

// Delete student
void deleteStudent(Student **students, int *count, int roll) {
    for (int i = 0; i < *count; i++) {
        if ((*students)[i].roll == roll) {
            for (int j = i; j < *count - 1; j++) {
                (*students)[j] = (*students)[j + 1];
            }
            (*count)--;
            *students = realloc(*students, (*count) * sizeof(Student));
            printf("Record deleted!\n");
            return;
        }
    }
    printf("Student not found.\n");
}

// Sort students
void sortStudents(Student *students, int count, int byMarks) {
    for (int i = 0; i < count - 1; i++) {
        for (int j = i + 1; j < count; j++) {
            int condition = byMarks ?
                (students[i].marks > students[j].marks) :
                (students[i].roll > students[j].roll);
            if (condition) {
                Student tmp = students[i];
                students[i] = students[j];
                students[j] = tmp;
            }
        }
    }
    printf("Students sorted!\n");
}

// Save to file
void saveToFile(Student *students, int count, const char *filename) {
    FILE *fp = fopen(filename, "wb");
    if (!fp) {
        printf("Error saving file.\n");
        return;
    }
    fwrite(&count, sizeof(int), 1, fp);
    fwrite(students, sizeof(Student), count, fp);
    fclose(fp);
    printf("Records saved to %s\n", filename);
}

// Load from file
void loadFromFile(Student **students, int *count, const char *filename) {
    FILE *fp = fopen(filename, "rb");
    if (!fp) return;
    fread(count, sizeof(int), 1, fp);
    *students = malloc(*count * sizeof(Student));
    fread(*students, sizeof(Student), *count, fp);
    fclose(fp);
    printf("Loaded %d records from %s\n", *count, filename);
}
