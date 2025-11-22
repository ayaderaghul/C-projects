#include <stdio.h>
#include <ctype.h>

int main() {
    char s[] = "hello    there   are\nfive\tpeople";
    int IN_WORD = 0;
    int count = 0;

    for (int i = 0; s[i] != '\0'; i++) {
        if (!isspace((unsigned char) s[i])) {
            if(!IN_WORD) {
                count++;
                IN_WORD = 1;
            }
        } else {
            IN_WORD = 0;
        }
    }

    printf("count: %d\n", count);
    
    return 0;
}