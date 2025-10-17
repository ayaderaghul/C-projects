#include <stdio.h>
#include <ctype.h>

int isVowel(char chr) {
    char c = tolower(chr);
    if(c=='a' || c =='e' ||c=='o'||c=='i'||c=='u') {
        return 1;
    }
    return 0;
}

int main(){

    char *string = "hello";
    int vowels = 0, consonants = 0;
    while(*string){
        if (isalpha(*string)) {
            if (isVowel(*string)) vowels++;
            else consonants++;
        }
        *string++;
    }

    printf("%d vowels, %d consonants", vowels, consonants);
    return 0;
}