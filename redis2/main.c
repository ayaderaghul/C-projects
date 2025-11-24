#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char global_key[32];
char global_value[32];

void kv_set(char*key, char *value){
    strcpy(global_key, key);
    strcpy(global_value,value);
}

char*kv_get(char*key){
    if(strcmp(global_key, key)==0){
        return global_value;
    }
    return "Not found";
}

// int main() {
//     set("mykey", "hello");
//     printf("result: %s\n", get("mykey"));
//     return 0;
// }