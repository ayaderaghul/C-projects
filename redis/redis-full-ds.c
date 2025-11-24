#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

// --- CONSTANTS ---
#define OBJ_STRING 0
#define OBJ_LIST   1
#define OBJ_HASH   2
#define OBJ_SET    3  // [NEW] Unique items only

// --- DATA STRUCTURES ---

// 1. Linked List Node (for Lists and Sets)
typedef struct ListNode {
    char *val;
    struct ListNode *next;
} ListNode;

// 2. Hash Node (for Hashes: Field -> Value)
typedef struct HashNode {
    char *field;
    char *value;
    struct HashNode *next;
} HashNode;

// 3. The Generic Object Wrapper
typedef struct RedisObject {
    int type;       // 0=String, 1=List, 2=Hash, 3=Set
    void *ptr;      // Points to the specific data structure
} RedisObject;

// 4. The Database Entry
typedef struct Entry {
    char *key;
    RedisObject *obj;
    struct Entry *next;
} Entry;

// 5. The Main Table
typedef struct HashTable {
    Entry **buckets;
    int size;
} HashTable;

HashTable *GLOBAL_DB;

// --- MEMORY MANAGEMENT ---

// Helper to free an object before we overwrite it
void free_object(RedisObject *o) {
    if (!o) return;
    if (o->type == OBJ_STRING) {
        free(o->ptr);
    } 
    else if (o->type == OBJ_LIST || o->type == OBJ_SET) {
        ListNode *curr = (ListNode*)o->ptr;
        while (curr) {
            ListNode *next = curr->next;
            free(curr->val);
            free(curr);
            curr = next;
        }
    }
    else if (o->type == OBJ_HASH) {
        HashNode *curr = (HashNode*)o->ptr;
        while (curr) {
            HashNode *next = curr->next;
            free(curr->field);
            free(curr->value);
            free(curr);
            curr = next;
        }
    }
    free(o);
}

// --- CORE FUNCTIONS ---

HashTable* create_table(int size) {
    HashTable *t = malloc(sizeof(HashTable));
    t->size = size;
    t->buckets = calloc(size, sizeof(Entry*));
    return t;
}

unsigned int hash(char *key, int size) {
    unsigned long int value = 0;
    for (int i=0; i<strlen(key); ++i) value = value * 37 + key[i];
    return value % size;
}

// Find existing entry or create a new wrapper
Entry* get_entry(HashTable *t, char *key, int create_if_missing) {
    unsigned int slot = hash(key, t->size);
    Entry *e = t->buckets[slot];
    while (e) {
        if (strcmp(e->key, key) == 0) return e;
        e = e->next;
    }
    if (!create_if_missing) return NULL;

    // Create new
    Entry *n = malloc(sizeof(Entry));
    n->key = strdup(key);
    n->obj = NULL;
    n->next = t->buckets[slot];
    t->buckets[slot] = n;
    return n;
}

// --- COMMAND LOGIC ---

// SET (String)
void set(HashTable *t, char *key, char *val) {
    Entry *e = get_entry(t, key, 1);
    if (e->obj) free_object(e->obj); // Clean old data

    e->obj = malloc(sizeof(RedisObject));
    e->obj->type = OBJ_STRING;
    e->obj->ptr = strdup(val);
}

// GET (String)
char* get(HashTable *t, char *key) {
    Entry *e = get_entry(t, key, 0);
    if (!e || !e->obj || e->obj->type != OBJ_STRING) return NULL;
    return (char*)e->obj->ptr;
}

// LPUSH (List)
int lpush(HashTable *t, char *key, char *val) {
    Entry *e = get_entry(t, key, 1);
    
    // Init List if empty
    if (!e->obj) {
        e->obj = malloc(sizeof(RedisObject));
        e->obj->type = OBJ_LIST;
        e->obj->ptr = NULL;
    }
    if (e->obj->type != OBJ_LIST) return -1; // Error

    ListNode *head = (ListNode*)e->obj->ptr;
    ListNode *n = malloc(sizeof(ListNode));
    n->val = strdup(val);
    n->next = head;
    e->obj->ptr = n;
    
    // Count size
    int count = 0;
    ListNode *c = n;
    while(c) { count++; c=c->next; }
    return count;
}

// LRANGE (List - Returns all for simplicity)
char* lrange(HashTable *t, char *key) {
    Entry *e = get_entry(t, key, 0);
    if (!e || !e->obj || e->obj->type != OBJ_LIST) return NULL;
    
    // Build a big string (Simulated Array)
    static char buf[4096]; buf[0]=0;
    ListNode *c = (ListNode*)e->obj->ptr;
    while(c) {
        strcat(buf, c->val);
        strcat(buf, "\n");
        c = c->next;
    }
    return buf;
}

// HSET (Hash)
int hset(HashTable *t, char *key, char *field, char *val) {
    Entry *e = get_entry(t, key, 1);
    if (!e->obj) {
        e->obj = malloc(sizeof(RedisObject));
        e->obj->type = OBJ_HASH;
        e->obj->ptr = NULL;
    }
    if (e->obj->type != OBJ_HASH) return -1;

    HashNode *head = (HashNode*)e->obj->ptr;
    HashNode *c = head;
    while(c) {
        if(strcmp(c->field, field)==0) {
            free(c->value); c->value = strdup(val); return 0; // Updated
        }
        c = c->next;
    }
    // New Field
    HashNode *n = malloc(sizeof(HashNode));
    n->field = strdup(field);
    n->value = strdup(val);
    n->next = head;
    e->obj->ptr = n;
    return 1; // Created
}

// HGET (Hash)
char* hget(HashTable *t, char *key, char *field) {
    Entry *e = get_entry(t, key, 0);
    if (!e || !e->obj || e->obj->type != OBJ_HASH) return NULL;
    HashNode *c = (HashNode*)e->obj->ptr;
    while(c) { if(strcmp(c->field, field)==0) return c->value; c=c->next; }
    return NULL;
}

// SADD (Set - Unique List)
int sadd(HashTable *t, char *key, char *val) {
    Entry *e = get_entry(t, key, 1);
    if (!e->obj) {
        e->obj = malloc(sizeof(RedisObject));
        e->obj->type = OBJ_SET;
        e->obj->ptr = NULL;
    }
    if (e->obj->type != OBJ_SET) return -1;

    ListNode *head = (ListNode*)e->obj->ptr;
    ListNode *c = head;
    while(c) {
        if(strcmp(c->val, val)==0) return 0; // Already exists!
        c = c->next;
    }
    // Add new
    ListNode *n = malloc(sizeof(ListNode));
    n->val = strdup(val);
    n->next = head;
    e->obj->ptr = n;
    return 1;
}

// SMEMBERS (Set)
char* smembers(HashTable *t, char *key) {
    Entry *e = get_entry(t, key, 0);
    if (!e || !e->obj || e->obj->type != OBJ_SET) return NULL;
    static char buf[4096]; buf[0]=0;
    ListNode *c = (ListNode*)e->obj->ptr;
    while(c) { strcat(buf, c->val); strcat(buf, "\n"); c = c->next; }
    return buf;
}

// --- NETWORK RESP ---

void parse_resp(char *buf, char *cmd, char *a1, char *a2, char *a3) {
    cmd[0]=0; a1[0]=0; a2[0]=0; a3[0]=0;
    if(buf[0]!='*') { sscanf(buf, "%s %s %s %s", cmd, a1, a2, a3); return; }
    char *c = strstr(buf, "\r\n"); if(!c) return; c+=2;
    int idx=0;
    while(*c) {
        if(*c=='$') {
            c = strstr(c, "\r\n"); if(!c) break; c+=2;
            char *e = strstr(c, "\r\n"); if(!e) break;
            int len = e-c;
            if(idx==0) { strncpy(cmd,c,len); cmd[len]=0; }
            else if(idx==1) { strncpy(a1,c,len); a1[len]=0; }
            else if(idx==2) { strncpy(a2,c,len); a2[len]=0; }
            else if(idx==3) { strncpy(a3,c,len); a3[len]=0; }
            idx++; c=e+2;
        } else break;
    }
}

void send_ok(SOCKET s) { send(s, "+OK\r\n", 5, 0); }
void send_int(SOCKET s, int n) { char b[64]; sprintf(b, ":%d\r\n", n); send(s, b, strlen(b), 0); }
void send_err(SOCKET s, char *m) { char b[1024]; sprintf(b, "-ERR %s\r\n", m); send(s, b, strlen(b), 0); }
void send_bulk(SOCKET s, char *v) {
    if(!v) { send(s, "$-1\r\n", 5, 0); return; }
    char b[4096]; sprintf(b, "$%d\r\n%s\r\n", (int)strlen(v), v); send(s, b, strlen(b), 0);
}

// --- MAIN LOOP ---

int main() {
    GLOBAL_DB = create_table(100);
    WSADATA wsa; WSAStartup(MAKEWORD(2,2), &wsa);
    
    SOCKET master_socket = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET; addr.sin_addr.s_addr = INADDR_ANY; addr.sin_port = htons(6379);
    bind(master_socket, (struct sockaddr *)&addr, sizeof(addr));
    listen(master_socket, 5);

    SOCKET clients[30];
    for(int i=0; i<30; i++) clients[i] = 0;

    printf("FULL REDIS (String, List, Hash, Set): Ready.\n");

    while(1) {
        fd_set readfds; FD_ZERO(&readfds);
        FD_SET(master_socket, &readfds);
        SOCKET max_sd = master_socket;
        for(int i=0; i<30; i++) {
            if(clients[i] > 0) FD_SET(clients[i], &readfds);
            if(clients[i] > max_sd) max_sd = clients[i];
        }

        select(0, &readfds, NULL, NULL, NULL);

        // New Connection
        if (FD_ISSET(master_socket, &readfds)) {
            SOCKET new_socket = accept(master_socket, NULL, NULL);
            for (int i=0; i<30; i++) { if(clients[i]==0) { clients[i]=new_socket; break; }}
        }

        // Check Clients
        for (int i=0; i<30; i++) {
            SOCKET s = clients[i];
            if (FD_ISSET(s, &readfds)) {
                char buf[1024]={0};
                if (recv(s, buf, 1024, 0) <= 0) {
                    closesocket(s); clients[i] = 0;
                } else {
                    char cmd[64]={0}, a1[64]={0}, a2[64]={0}, a3[64]={0};
                    parse_resp(buf, cmd, a1, a2, a3);
                    printf("CMD: %s %s %s %s\n", cmd, a1, a2, a3);

                    // --- STRINGS ---
                    if (strcmp(cmd, "SET") == 0) {
                        set(GLOBAL_DB, a1, a2); send_ok(s);
                    } 
                    else if (strcmp(cmd, "GET") == 0) {
                        char *val = get(GLOBAL_DB, a1); send_bulk(s, val);
                    }
                    // --- LISTS ---
                    else if (strcmp(cmd, "LPUSH") == 0) {
                        int sz = lpush(GLOBAL_DB, a1, a2);
                        if(sz==-1) send_err(s,"WRONGTYPE"); else send_int(s, sz);
                    }
                    else if (strcmp(cmd, "LRANGE") == 0) {
                        char *res = lrange(GLOBAL_DB, a1); send_bulk(s, res);
                    }
                    // --- HASHES ---
                    else if (strcmp(cmd, "HSET") == 0) {
                        int res = hset(GLOBAL_DB, a1, a2, a3);
                        if(res==-1) send_err(s,"WRONGTYPE"); else send_int(s, res);
                    }
                    else if (strcmp(cmd, "HGET") == 0) {
                        char *val = hget(GLOBAL_DB, a1, a2); send_bulk(s, val);
                    }
                    // --- SETS ---
                    else if (strcmp(cmd, "SADD") == 0) {
                        int res = sadd(GLOBAL_DB, a1, a2);
                        if(res==-1) send_err(s,"WRONGTYPE"); else send_int(s, res);
                    }
                    else if (strcmp(cmd, "SMEMBERS") == 0) {
                        char *res = smembers(GLOBAL_DB, a1); send_bulk(s, res);
                    }
                    // --- SYSTEM ---
                    else if (strcmp(cmd, "PING") == 0) send(s, "+PONG\r\n", 7, 0);
                    else if (strcmp(cmd, "COMMAND") == 0) send_err(s, "Unsup");
                    else send_err(s, "Unknown");
                }
            }
        }
    }
    return 0;
}