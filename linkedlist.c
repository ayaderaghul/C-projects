#include <stdio.h>
#include <stdlib.h>

typedef struct Node {
    int data;
    struct Node *next;
} Node;

void deleteAtEnd(Node**head) {
    if (*head == NULL) {
        printf("Empty list");
        return;
    }
    Node * tmp = *head;
    
    if (tmp->next==NULL) {
        free(tmp);
        *head=NULL;
        return;
    }

    while(tmp->next->next)
        tmp=tmp->next;

    free(tmp->next);
    tmp->next=NULL;
    
    return;
}

void deleteAtPosition(Node**head, int idx) {
    if (*head == NULL) {
        printf("empty list");
        return;
    }
    Node*tmp=*head;
    if(idx == 0) {
        *head = tmp->next;
        free(tmp);
        return;
    }
    for(int i = 0; tmp != NULL && i < idx - 1; i++)
        tmp=tmp->next;
    if (tmp==NULL || tmp->next==NULL) {
        printf("index out of range");
        return;
    }

    Node *del = tmp->next;
    tmp->next=del->next;
    free(del);

    return;
}