#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>        // Required for time()
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

// --- CONSTANTS ---
#define OBJ_STRING 0
#define OBJ_LIST   1
#define OBJ_HASH   2
#define OBJ_SET    3

// --- DATA STRUCTURES ---

typedef struct ListNode {
    char *val;
    struct ListNode *next;
} ListNode;

typedef struct HashNode {
    char *field;
    char *value;
    struct HashNode *next;
} HashNode;

typedef struct RedisObject {
    int type;
    void *ptr;
} RedisObject;

typedef struct Entry {
    char *key;
    RedisObject *obj;
    time_t expiry;      // [NEW] 0 = Forever, >0 = Timestamp of death
    struct Entry *next;
} Entry;

typedef struct HashTable {
    Entry **buckets;
    int size;
} HashTable;

HashTable *GLOBAL_DB;

// --- MEMORY CLEANUP ---

void free_object(RedisObject *o) {
    if (!o) return;
    if (o->type == OBJ_STRING) {
        free(o->ptr);
    } 
    else if (o->type == OBJ_LIST || o->type == OBJ_SET) {
        ListNode *curr = (ListNode*)o->ptr;
        while (curr) {
            ListNode *next = curr->next;
            free(curr->val); free(curr);
            curr = next;
        }
    }
    else if (o->type == OBJ_HASH) {
        HashNode *curr = (HashNode*)o->ptr;
        while (curr) {
            HashNode *next = curr->next;
            free(curr->field); free(curr->value); free(curr);
            curr = next;
        }
    }
    free(o);
}

// --- CORE STORAGE ---

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

// [NEW] Internal Delete Function
void delete_entry(HashTable *t, char *key) {
    unsigned int slot = hash(key, t->size);
    Entry *curr = t->buckets[slot];
    Entry *prev = NULL;

    while (curr) {
        if (strcmp(curr->key, key) == 0) {
            // Found it, remove from list
            if (prev) prev->next = curr->next;
            else t->buckets[slot] = curr->next;

            // Free memory
            free_object(curr->obj);
            free(curr->key);
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

// [NEW] The Lazy Expiration Check
// Returns 1 if key was expired (and deleted), 0 if safe.
int check_expiry(HashTable *t, char *key) {
    unsigned int slot = hash(key, t->size);
    Entry *e = t->buckets[slot];
    while (e) {
        if (strcmp(e->key, key) == 0) {
            // Check Time
            if (e->expiry != 0 && time(NULL) > e->expiry) {
                delete_entry(t, key);
                return 1; // It died
            }
            return 0; // It is alive
        }
        e = e->next;
    }
    return 0; // Key doesn't exist
}

// Helper to get entry (now simpler, assumes check_expiry called before)
Entry* get_entry_internal(HashTable *t, char *key, int create) {
    unsigned int slot = hash(key, t->size);
    Entry *e = t->buckets[slot];
    while (e) {
        if (strcmp(e->key, key) == 0) return e;
        e = e->next;
    }
    if (!create) return NULL;
    
    Entry *n = malloc(sizeof(Entry));
    n->key = strdup(key);
    n->obj = NULL;
    n->expiry = 0; // Default: Live forever
    n->next = t->buckets[slot];
    t->buckets[slot] = n;
    return n;
}

// --- COMMANDS ---

// SET
void set(HashTable *t, char *key, char *val) {
    // Note: SET removes TTL in real Redis, so we reset expiry to 0
    Entry *e = get_entry_internal(t, key, 1);
    if (e->obj) free_object(e->obj);
    
    e->obj = malloc(sizeof(RedisObject));
    e->obj->type = OBJ_STRING;
    e->obj->ptr = strdup(val);
    e->expiry = 0; // Clear expiry
}

// GET
char* get(HashTable *t, char *key) {
    if (check_expiry(t, key)) return NULL; // Lazy check
    
    Entry *e = get_entry_internal(t, key, 0);
    if (!e || !e->obj || e->obj->type != OBJ_STRING) return NULL;
    return (char*)e->obj->ptr;
}

// EXPIRE key seconds
int expire(HashTable *t, char *key, int seconds) {
    if (check_expiry(t, key)) return 0; // Already dead
    
    Entry *e = get_entry_internal(t, key, 0);
    if (!e) return 0; // Key not found
    
    e->expiry = time(NULL) + seconds; // Set death time
    return 1;
}

// TTL key
int ttl(HashTable *t, char *key) {
    if (check_expiry(t, key)) return -2; // Key missing (expired)

    Entry *e = get_entry_internal(t, key, 0);
    if (!e) return -2; // Key missing
    if (e->expiry == 0) return -1; // No expiry

    int remaining = e->expiry - time(NULL);
    if (remaining < 0) return 0; // Should have been deleted, but just in case
    return remaining;
}

// LPUSH
int lpush(HashTable *t, char *key, char *val) {
    if (check_expiry(t, key)) {} // If expired, it deletes, so we create new below

    Entry *e = get_entry_internal(t, key, 1);
    if (!e->obj) {
        e->obj = malloc(sizeof(RedisObject));
        e->obj->type = OBJ_LIST;
        e->obj->ptr = NULL;
    }
    if (e->obj->type != OBJ_LIST) return -1;

    ListNode *head = (ListNode*)e->obj->ptr;
    ListNode *n = malloc(sizeof(ListNode));
    n->val = strdup(val);
    n->next = head;
    e->obj->ptr = n;
    
    int c = 0; while(n){c++; n=n->next;} return c;
}

// LRANGE
char* lrange(HashTable *t, char *key) {
    if (check_expiry(t, key)) return NULL;
    
    Entry *e = get_entry_internal(t, key, 0);
    if (!e || !e->obj || e->obj->type != OBJ_LIST) return NULL;
    
    static char buf[4096]; buf[0]=0;
    ListNode *c = (ListNode*)e->obj->ptr;
    while(c) { strcat(buf, c->val); strcat(buf, "\n"); c = c->next; }
    return buf;
}

// HSET
int hset(HashTable *t, char *key, char *field, char *val) {
    check_expiry(t, key); // Ensure we don't write to dead key
    Entry *e = get_entry_internal(t, key, 1);
    if (!e->obj) { e->obj = malloc(sizeof(RedisObject)); e->obj->type = OBJ_HASH; e->obj->ptr=NULL; }
    if (e->obj->type != OBJ_HASH) return -1;
    
    HashNode *head = (HashNode*)e->obj->ptr;
    HashNode *c = head;
    while(c) {
        if(strcmp(c->field, field)==0) { free(c->value); c->value=strdup(val); return 0; }
        c=c->next;
    }
    HashNode *n=malloc(sizeof(HashNode)); n->field=strdup(field); n->value=strdup(val); n->next=head; e->obj->ptr=n;
    return 1;
}

// HGET
char* hget(HashTable *t, char *key, char *field) {
    if (check_expiry(t, key)) return NULL;
    Entry *e = get_entry_internal(t, key, 0);
    if (!e || !e->obj || e->obj->type != OBJ_HASH) return NULL;
    HashNode *c = (HashNode*)e->obj->ptr;
    while(c) { if(strcmp(c->field,field)==0) return c->value; c=c->next; }
    return NULL;
}

// --- RESP NETWORK ---

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
void send_bulk(SOCKET s, char *v) {
    if(!v) { send(s, "$-1\r\n", 5, 0); return; }
    char b[4096]; sprintf(b, "$%d\r\n%s\r\n", (int)strlen(v), v); send(s, b, strlen(b), 0);
}
void send_err(SOCKET s, char *m) { char b[1024]; sprintf(b, "-ERR %s\r\n", m); send(s, b, strlen(b), 0); }

// --- MAIN ---

int main() {
    GLOBAL_DB = create_table(100);
    WSADATA wsa; WSAStartup(MAKEWORD(2,2), &wsa);
    SOCKET master = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET; addr.sin_addr.s_addr = INADDR_ANY; addr.sin_port = htons(6379);
    bind(master, (struct sockaddr *)&addr, sizeof(addr));
    listen(master, 5);

    SOCKET clients[30] = {0};
    printf("REDIS WITH EXPIRATION: Ready.\n");

    while(1) {
        fd_set readfds; FD_ZERO(&readfds);
        FD_SET(master, &readfds);
        SOCKET max_sd = master;
        for(int i=0; i<30; i++) {
            if(clients[i]>0) FD_SET(clients[i], &readfds);
            if(clients[i]>max_sd) max_sd = clients[i];
        }

        select(0, &readfds, NULL, NULL, NULL);

        if (FD_ISSET(master, &readfds)) {
            SOCKET ns = accept(master, NULL, NULL);
            for(int i=0; i<30; i++) if(clients[i]==0) { clients[i]=ns; break; }
        }

        for (int i=0; i<30; i++) {
            SOCKET s = clients[i];
            if (FD_ISSET(s, &readfds)) {
                char buf[1024]={0};
                if (recv(s, buf, 1024, 0) <= 0) { closesocket(s); clients[i]=0; }
                else {
                    char cmd[64]={0}, a1[64]={0}, a2[64]={0}, a3[64]={0};
                    parse_resp(buf, cmd, a1, a2, a3);
                    printf("CMD: %s %s %s\n", cmd, a1, a2);

                    if (strcmp(cmd, "SET") == 0) { set(GLOBAL_DB, a1, a2); send_ok(s); }
                    else if (strcmp(cmd, "GET") == 0) { send_bulk(s, get(GLOBAL_DB, a1)); }
                    
                    // --- EXPIRE COMMANDS ---
                    else if (strcmp(cmd, "EXPIRE") == 0) {
                        int seconds = atoi(a2);
                        int res = expire(GLOBAL_DB, a1, seconds);
                        send_int(s, res);
                    }
                    else if (strcmp(cmd, "TTL") == 0) {
                        int res = ttl(GLOBAL_DB, a1);
                        send_int(s, res);
                    }

                    else if (strcmp(cmd, "LPUSH") == 0) {
                        int sz = lpush(GLOBAL_DB, a1, a2);
                        if(sz==-1) send_err(s,"WRONGTYPE"); else send_int(s, sz);
                    }
                    else if (strcmp(cmd, "LRANGE") == 0) { send_bulk(s, lrange(GLOBAL_DB, a1)); }
                    else if (strcmp(cmd, "HSET") == 0) {
                        int r = hset(GLOBAL_DB, a1, a2, a3);
                        if(r==-1) send_err(s,"WRONGTYPE"); else send_int(s, r);
                    }
                    else if (strcmp(cmd, "HGET") == 0) { send_bulk(s, hget(GLOBAL_DB, a1, a2)); }
                    else if (strcmp(cmd, "PING") == 0) send(s, "+PONG\r\n", 7, 0);
                    else if (strcmp(cmd, "COMMAND") == 0) send_err(s, "Unsup");
                    else send_err(s, "Unknown");
                }
            }
        }
    }
    return 0;
}