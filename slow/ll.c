#include <stdio.h>
#include <stdlib.h>

struct Node {
    int data;
    struct Node *next;
};

void printList(struct Node **head) {
    struct Node *tmp = *head;
    while(tmp) {
        printf("%d -> ", tmp->data);
        tmp = tmp->next;
    }
    return;
}   

int main() {
    struct Node *node1 = malloc(sizeof(struct Node));
    if (node1 == NULL) {
        printf("malloc failed");
        return 1;
    }

    node1->data = 5;
    node1->next = NULL;

    struct Node *node2 = malloc(sizeof(struct Node));
    if (node2 == NULL) {
        printf("malloc failed");
        return 1;
    }

    node2->data = 10;
    node2->next = NULL;
    
    node1->next = node2;

    printList(&node1);
    return 0;
}