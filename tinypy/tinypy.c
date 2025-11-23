#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// --- 1. Environment (Variables) ---
struct Var { char name[32]; int value; };
struct Var env[100];
int env_count = 0;

void set_var(char *name, int val) {
    for (int i=0; i<env_count; i++) {
        if (strcmp(env[i].name, name) == 0) {
            env[i].value = val; return;
        }
    }
    strcpy(env[env_count].name, name);
    env[env_count].value = val;
    env_count++;
}

int get_var(char *name) {
    for (int i=0; i<env_count; i++) {
        if (strcmp(env[i].name, name) == 0) return env[i].value;
    }
    printf("Runtime Error: Undefined variable '%s'\n", name);
    exit(1);
}

// --- 2. Lexer (Tokenizer) ---
typedef enum { 
    TOKEN_INT, TOKEN_ID, TOKEN_ASSIGN, TOKEN_PLUS, TOKEN_PRINT, TOKEN_EOF 
} TokenType;

typedef struct { TokenType type; char text[32]; } Token;

const char *src;
Token curr;

void next_token() {
    curr.text[0] = '\0';
    while (*src && isspace(*src)) src++;
    
    if (!*src) { curr.type = TOKEN_EOF; return; }
    
    if (isdigit(*src)) {
        curr.type = TOKEN_INT;
        int i=0; while(isdigit(*src)) curr.text[i++] = *src++;
        curr.text[i] = '\0';
    } else if (isalpha(*src)) {
        int i=0; while(isalnum(*src)) curr.text[i++] = *src++;
        curr.text[i] = '\0';
        if (strcmp(curr.text, "print") == 0) curr.type = TOKEN_PRINT;
        else curr.type = TOKEN_ID;
    } else if (*src == '=') { curr.type = TOKEN_ASSIGN; src++; }
    else if (*src == '+') { curr.type = TOKEN_PLUS; src++; }
}

// --- 3. AST Node Definitions ---
typedef enum { 
    NODE_INT,    // 5
    NODE_VAR,    // x
    NODE_ADD,    // +
    NODE_ASSIGN, // =
    NODE_PRINT   // print
} NodeType;

typedef struct ASTNode {
    NodeType type;
    int int_val;            // For numbers
    char var_name[32];      // For variables
    struct ASTNode *left;   // Child 1
    struct ASTNode *right;  // Child 2
} ASTNode;

ASTNode* make_node(NodeType type) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = type;
    node->left = NULL; node->right = NULL;
    return node;
}

// --- 4. Parser (Constructs AST) ---
ASTNode* parse_expr(); // Forward decl

// Parses atomic values: "5" or "x"
ASTNode* parse_factor() {
    ASTNode* node;
    if (curr.type == TOKEN_INT) {
        node = make_node(NODE_INT);
        node->int_val = atoi(curr.text);
        next_token();
    } else if (curr.type == TOKEN_ID) {
        node = make_node(NODE_VAR);
        strcpy(node->var_name, curr.text);
        next_token();
    } else {
        return NULL; // Error handling skipped for brevity
    }
    return node;
}

// Parses expressions: "5 + x"
ASTNode* parse_expr() {
    ASTNode* left = parse_factor();
    
    while (curr.type == TOKEN_PLUS) {
        next_token(); // Skip '+'
        ASTNode* node = make_node(NODE_ADD);
        node->left = left;
        node->right = parse_factor();
        left = node; // The new add node becomes the root
    }
    return left;
}

// Parses statements: "x = 5" or "print x"
ASTNode* parse_statement() {
    if (curr.type == TOKEN_PRINT) {
        next_token(); // Skip 'print'
        ASTNode* node = make_node(NODE_PRINT);
        node->left = parse_expr();
        return node;
    } 
    else if (curr.type == TOKEN_ID) {
        // Lookahead hack: check if next is '='
        // In a real compiler we'd check properly, here we assume valid syntax
        char name[32];
        strcpy(name, curr.text);
        next_token(); // Skip ID
        
        if (curr.type == TOKEN_ASSIGN) {
            next_token(); // Skip '='
            ASTNode* node = make_node(NODE_ASSIGN);
            strcpy(node->var_name, name);
            node->left = parse_expr(); // The value to assign is in 'left'
            return node;
        }
    }
    return NULL;
}

// --- 5. Evaluator (Runs AST) ---
int eval(ASTNode* node) {
    if (!node) return 0;

    switch (node->type) {
        case NODE_INT:
            return node->int_val;

        case NODE_VAR:
            return get_var(node->var_name);

        case NODE_ADD:
            // Recursion happens here!
            return eval(node->left) + eval(node->right);

        case NODE_ASSIGN: {
            int val = eval(node->left); // Solve the expression first
            set_var(node->var_name, val);
            return val;
        }

        case NODE_PRINT: {
            int val = eval(node->left); // Solve the expression first
            printf("%d\n", val);
            return val;
        }
    }
    return 0;
}

int main() {
    // A script that uses variables and math
    char code[] = "x = 10 \n y = 20 \n print x + y";
    src = code;

    printf("--- Code ---\n%s\n--- Output ---\n", code);

    next_token(); // Prime the pump

    while (curr.type != TOKEN_EOF) {
        // Phase 1: PARSE
        ASTNode* root = parse_statement();
        
        // Phase 2: EVALUATE
        if (root) {
            eval(root);
            // In a real app, we would free(root) here
        }
    }

    return 0;
}