#include <stdio.h>
int main(){
    char *name;
    int age;

    scanf("%s", name);
    scanf("%d", &age);
    printf("Hello %s, you are %d yo.\n", name, age);
    return 0;
}