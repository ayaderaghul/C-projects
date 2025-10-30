#include <stdio.h>
#include <stdlib.h>

#define MAX_SIZE 25 // max size for stack

typedef struct{
    int arr[MAX_SIZE];
    int top;
} Stack;

typedef struct{
    int arr[MAX_SIZE];
    int front;
    int rear;
} Queue;

// stack functions
void stackOption();
void push(Stack *stack, int value);
void pop(Stack *stack);
int peekStack(Stack *stack);
int isFullStack(Stack *stack);
int isEmptyStack(Stack *stack);
void printStack(Stack *stack);

// queue functions
void queueOption();
void enqueue(Queue *queue, int value);
void dequeue(Queue *queue);
int peekQueue(Queue *queue);
int isFullQueue(Queue *queue);
int isEmptyQueue(Queue *queue);
void printQueue(Queue *queue);

// other
void clearScreen();
void clearBuffer();


int main(){
    printf("Max value for both stack and queue: 25\n");
    while(1){
        printf("\n-------------------------------\n");
        printf("1 - Stack\n");
        printf("2 - Queue\n");
        printf("3 - Exit\n");
        printf("-------------------------------\n");

        printf("Choose option: ");

        int menu_option;
        char sign; //used to check if input is integer(if its not float)
        if(scanf("%d%c", &menu_option, &sign) != 2 || sign != '\n'){
            clearScreen();
            clearBuffer();
            printf("Invalid option, try again.\n");
            continue;
        }
        
        clearScreen();

        switch(menu_option){
            case 1:
                stackOption();
                break;
            case 2:
                queueOption();
                break;
            case 3:
                return 1;
            default:
                printf("Invalid option, try again.\n");
        }
    }
    return 0;
}

// stack functions

void stackOption(){
    Stack stack;
    stack.top = -1;

    while(1){
        printf("\n-------------------------------\n");
        printf("1 - Push value into the stack\n");
        printf("2 - Pop value from the stack\n");
        printf("3 - peekStack at the top of the stack\n");
        printf("4 - Print stack\n");
        printf("5 - Exit to main menu\n");
        printf("-------------------------------\n");

        printf("Choose option: ");

        int menu_option;
        char sign; //used to check if input is integer(if its not float)
        if(scanf("%d%c", &menu_option, &sign) != 2 || sign != '\n'){
            clearScreen();
            clearBuffer();
            printf("Invalid option, try again.\n");
            continue;
        }

        clearScreen();

        if(menu_option == 1){ 
            printf("Enter integer value to push into the stack: "); 
            int value; 
            if(scanf("%d%c", &value, &sign) != 2 || sign != '\n'){ 
                clearScreen(); 
                clearBuffer(); 
                printf("[ERROR] Need to enter integer\n"); 
                continue; 
            } 
            push(&stack, value); 
        } 
        else if(menu_option == 2){ 
            pop(&stack);
        }
        else if(menu_option == 3){
            int peeked_value = peekStack(&stack); 
            printf("Peeked value: %d\n", peeked_value); 
        } 
        else if(menu_option == 4){ 
            printStack(&stack); 
        }
        else if(menu_option == 5){ 
            return; 
        }
    }
}

void push(Stack *stack, int value){
    if(isFullStack(stack)){
        printf("Stack is full\n");
        return;
    }
    stack->arr[++stack->top] = value;
}

void pop(Stack* stack){
    if(isEmptyStack(stack)){
        printf("Stack is empty\n");
        return;
    }
    int popped_value = stack->arr[stack->top];
    stack->top--;
    printf("Popped value: %d\n", popped_value);
}

int peekStack(Stack *stack){
    if(isEmptyStack(stack)){
        printf("Stack is empty\n");
        return -1;
    }

    return stack->arr[stack->top];
}

int isFullStack(Stack *stack){
    if(stack->top == MAX_SIZE-1)
        return 1;
    return 0;
}

int isEmptyStack(Stack *stack){
    if(stack->top == -1)
        return 1;
    return 0;
}

void printStack(Stack *stack){
    if(isEmptyStack(stack)){
        printf("Stack is empty\n");
        return;
    }
    printf("Stack values (from top to bottom):\n");
    for(int i = stack->top; i >= 0; i--){
        printf("|%d|\n", stack->arr[i]);
    }
}

// queue functions
void queueOption(){
    Queue queue;
    queue.front = 0;
    queue.rear = 0;

    while(1){
        printf("\n-------------------------------\n");
        printf("1 - Enqueue value into the queue\n");
        printf("2 - Deque the queue\n");
        printf("3 - Peek at the front of the queue\n");
        printf("4 - Print queue\n");
        printf("5 - Exit to main menu\n");
        printf("-------------------------------\n");

        printf("Choose option: ");

        int menu_option;
        char sign; //used to check if input is integer(if its not float)
        if(scanf("%d%c", &menu_option, &sign) != 2 || sign != '\n'){
            clearScreen();
            clearBuffer();
            printf("Invalid option, try again.\n");
            continue;
        }

        clearScreen();

        if(menu_option == 1){
            printf("Enter integer value to push into the queue: ");
            int value;
            if(scanf("%d%c", &value, &sign) != 2 || sign != '\n'){
                clearScreen();
                clearBuffer();
                printf("[ERROR] Need to enter integer\n");
                continue;
            }

            enqueue(&queue, value);
        }
        else if(menu_option == 2){
            dequeue(&queue);
        }
        else if(menu_option == 3){
            int peeked_value = peekQueue(&queue);
            printf("Peeked value: %d\n", peeked_value);
        }
        else if(menu_option == 4){
            printQueue(&queue);
        }
        else if(menu_option == 5){
            return;
        }
    }
}

void enqueue(Queue *queue, int value){
    if(isFullQueue(queue)){
        printf("Queue is full\n");
        return;
    }
    queue->arr[queue->rear] = value;
    queue->rear++;
}

void dequeue(Queue *queue){
    if(isEmptyQueue(queue)){
        printf("Queue is empty\n");
        return;
    }
    queue->front++;
}

int peekQueue(Queue *queue){
    if(isEmptyQueue(queue)){
        printf("Queue is empty\n");
        return -1;
    }
    return queue->arr[queue->front];
}

int isFullQueue(Queue *queue){
    if(queue->rear == MAX_SIZE)
        return 1;
    return 0;
}

int isEmptyQueue(Queue *queue){
    if(queue->front == queue->rear)
        return 1;
    return 0;
}

void printQueue(Queue *queue){
    if(isEmptyQueue(queue)){
        printf("Queue is empty\n");
        return;
    }
    printf("Queue values (from front to rear):\n");
    for(int i = queue->front; i < queue->rear; i++){
        printf("%d ", queue->arr[i]);
    }
    printf("\n");
}

// other

void clearBuffer(){
    while(getchar() != '\n');
}

void clearScreen(){
    #ifdef _WIN32
        system("cls");
    #elif __linux__
        system("clear");
    #endif
}
