#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

// --- CONSTANTS ---
#define OBJ_STRING 0
#define OBJ_LIST   1
#define OBJ_HASH   2
#define OBJ_SET    3
#define MAX_CLIENTS 30
#define AUTO_SAVE_SEC 60

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

// Generic Object Wrapper
typedef struct RedisObject {
    int type;       // 0=String, 1=List, 2=Hash, 3=Set
    void *ptr;      // Pointer to the actual data
} RedisObject;

// Database Entry
typedef struct Entry {
    char *key;
    RedisObject *obj;
    time_t expiry;  // 0 = Live forever
    struct Entry *next;
} Entry;

// Hash Table (The DB)
typedef struct HashTable {
    Entry **buckets;
    int size;
} HashTable;

// Disk Format (Simplified: Only saves Strings for this demo)
typedef struct {
    char key[64];
    char value[64];
} DiskEntry;

// Globals
HashTable *GLOBAL_DB;
time_t last_save_time;

// --- MEMORY MANAGEMENT ---

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

void delete_entry(HashTable *t, char *key) {
    unsigned int slot = hash(key, t->size);
    Entry *curr = t->buckets[slot];
    Entry *prev = NULL;
    while (curr) {
        if (strcmp(curr->key, key) == 0) {
            if (prev) prev->next = curr->next;
            else t->buckets[slot] = curr->next;
            free_object(curr->obj);
            free(curr->key); free(curr);
            return;
        }
        prev = curr; curr = curr->next;
    }
}

// Returns 1 if expired, 0 if safe
int check_expiry(HashTable *t, char *key) {
    unsigned int slot = hash(key, t->size);
    Entry *e = t->buckets[slot];
    while (e) {
        if (strcmp(e->key, key) == 0) {
            if (e->expiry != 0 && time(NULL) > e->expiry) {
                delete_entry(t, key);
                return 1;
            }
            return 0;
        }
        e = e->next;
    }
    return 0;
}

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
    n->expiry = 0;
    n->next = t->buckets[slot];
    t->buckets[slot] = n;
    return n;
}

// --- COMMANDS ---

void set(HashTable *t, char *key, char *val) {
    Entry *e = get_entry_internal(t, key, 1);
    if (e->obj) free_object(e->obj);
    e->obj = malloc(sizeof(RedisObject));
    e->obj->type = OBJ_STRING;
    e->obj->ptr = strdup(val);
    e->expiry = 0; // Reset expiry
}

char* get(HashTable *t, char *key) {
    if (check_expiry(t, key)) return NULL;
    Entry *e = get_entry_internal(t, key, 0);
    if (!e || !e->obj || e->obj->type != OBJ_STRING) return NULL;
    return (char*)e->obj->ptr;
}

int expire(HashTable *t, char *key, int sec) {
    if (check_expiry(t, key)) return 0;
    Entry *e = get_entry_internal(t, key, 0);
    if (!e) return 0;
    e->expiry = time(NULL) + sec;
    return 1;
}

int ttl(HashTable *t, char *key) {
    if (check_expiry(t, key)) return -2;
    Entry *e = get_entry_internal(t, key, 0);
    if (!e) return -2;
    if (e->expiry == 0) return -1;
    return (int)(e->expiry - time(NULL));
}

int lpush(HashTable *t, char *key, char *val) {
    check_expiry(t, key);
    Entry *e = get_entry_internal(t, key, 1);
    if (!e->obj) { e->obj = malloc(sizeof(RedisObject)); e->obj->type = OBJ_LIST; e->obj->ptr=NULL; }
    if (e->obj->type != OBJ_LIST) return -1;
    ListNode *n = malloc(sizeof(ListNode));
    n->val = strdup(val); n->next = (ListNode*)e->obj->ptr; e->obj->ptr = n;
    int c=0; ListNode *temp=n; while(temp){c++; temp=temp->next;} return c;
}

char* lrange(HashTable *t, char *key) {
    if (check_expiry(t, key)) return NULL;
    Entry *e = get_entry_internal(t, key, 0);
    if (!e || !e->obj || e->obj->type != OBJ_LIST) return NULL;
    static char buf[4096]; buf[0]=0;
    ListNode *c = (ListNode*)e->obj->ptr;
    while(c) { strcat(buf, c->val); strcat(buf, "\n"); c = c->next; }
    return buf;
}

int sadd(HashTable *t, char *key, char *val) {
    check_expiry(t, key);
    Entry *e = get_entry_internal(t, key, 1);
    if (!e->obj) { e->obj = malloc(sizeof(RedisObject)); e->obj->type = OBJ_SET; e->obj->ptr=NULL; }
    if (e->obj->type != OBJ_SET) return -1;
    ListNode *c = (ListNode*)e->obj->ptr;
    while(c) { if(strcmp(c->val, val)==0) return 0; c=c->next; }
    ListNode *n = malloc(sizeof(ListNode)); n->val = strdup(val); n->next = (ListNode*)e->obj->ptr; e->obj->ptr=n;
    return 1;
}

char* smembers(HashTable *t, char *key) {
    if (check_expiry(t, key)) return NULL;
    Entry *e = get_entry_internal(t, key, 0);
    if (!e || !e->obj || e->obj->type != OBJ_SET) return NULL;
    static char buf[4096]; buf[0]=0;
    ListNode *c = (ListNode*)e->obj->ptr;
    while(c) { strcat(buf, c->val); strcat(buf, "\n"); c = c->next; }
    return buf;
}

int hset(HashTable *t, char *key, char *f, char *v) {
    check_expiry(t, key);
    Entry *e = get_entry_internal(t, key, 1);
    if (!e->obj) { e->obj = malloc(sizeof(RedisObject)); e->obj->type = OBJ_HASH; e->obj->ptr=NULL; }
    if (e->obj->type != OBJ_HASH) return -1;
    HashNode *head = (HashNode*)e->obj->ptr;
    HashNode *c = head;
    while(c) {
        if(strcmp(c->field, f)==0) { free(c->value); c->value=strdup(v); return 0; }
        c=c->next;
    }
    HashNode *n=malloc(sizeof(HashNode)); n->field=strdup(f); n->value=strdup(v); n->next=head; e->obj->ptr=n;
    return 1;
}

char* hget(HashTable *t, char *key, char *f) {
    if (check_expiry(t, key)) return NULL;
    Entry *e = get_entry_internal(t, key, 0);
    if (!e || !e->obj || e->obj->type != OBJ_HASH) return NULL;
    HashNode *c = (HashNode*)e->obj->ptr;
    while(c) { if(strcmp(c->field,f)==0) return c->value; c=c->next; }
    return NULL;
}

// --- PERSISTENCE (STRINGS ONLY FOR DEMO) ---

void save_to_disk() {
    printf("SNAPSHOT: Saving strings to disk... ");
    FILE *fp = fopen("dump.db", "wb");
    if (!fp) return;
    for (int i=0; i<GLOBAL_DB->size; i++) {
        Entry *e = GLOBAL_DB->buckets[i];
        while (e) {
            // Only save live keys that are strings
            if ((e->expiry == 0 || e->expiry > time(NULL)) && e->obj->type == OBJ_STRING) {
                DiskEntry d; memset(&d,0,sizeof(d));
                strncpy(d.key, e->key, 63);
                strncpy(d.value, (char*)e->obj->ptr, 63);
                fwrite(&d, sizeof(d), 1, fp);
            }
            e = e->next;
        }
    }
    fclose(fp);
    printf("Done.\n");
    last_save_time = time(NULL);
}

void load_from_disk() {
    FILE *fp = fopen("dump.db", "rb");
    if (!fp) { printf("SNAPSHOT: Starting clean.\n"); return; }
    printf("SNAPSHOT: Loading... ");
    DiskEntry d;
    while (fread(&d, sizeof(d), 1, fp)) set(GLOBAL_DB, d.key, d.value);
    fclose(fp);
    printf("Loaded.\n");
}

// --- NETWORK & RESP ---

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

// --- MAIN LOOP ---

int main() {
    GLOBAL_DB = create_table(100);
    load_from_disk();
    last_save_time = time(NULL);

    WSADATA wsa; WSAStartup(MAKEWORD(2,2), &wsa);
    SOCKET master = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET; addr.sin_addr.s_addr = INADDR_ANY; addr.sin_port = htons(6379);
    bind(master, (struct sockaddr *)&addr, sizeof(addr));
    listen(master, 5);

    SOCKET clients[MAX_CLIENTS] = {0};
    printf("MINI-REDIS ULTIMATE: Ready on 6379.\n");

    while(1) {
        fd_set readfds; FD_ZERO(&readfds);
        FD_SET(master, &readfds);
        SOCKET max_sd = master;
        for(int i=0; i<MAX_CLIENTS; i++) {
            if(clients[i]>0) FD_SET(clients[i], &readfds);
            if(clients[i]>max_sd) max_sd = clients[i];
        }

        // Wait up to 1 second
        struct timeval timeout; timeout.tv_sec=1; timeout.tv_usec=0;
        select(0, &readfds, NULL, NULL, &timeout);

        // Auto-Save Check
        if (time(NULL) - last_save_time > AUTO_SAVE_SEC) save_to_disk();

        // New Connection
        if (FD_ISSET(master, &readfds)) {
            SOCKET ns = accept(master, NULL, NULL);
            for(int i=0; i<MAX_CLIENTS; i++) if(clients[i]==0) { clients[i]=ns; break; }
        }

        // Handle Clients
        for (int i=0; i<MAX_CLIENTS; i++) {
            SOCKET s = clients[i];
            if (FD_ISSET(s, &readfds)) {
                char buf[1024]={0};
                int bytes = recv(s, buf, 1024, 0);
                if (bytes <= 0) { closesocket(s); clients[i]=0; }
                else {
                    char cmd[64]={0}, a1[64]={0}, a2[64]={0}, a3[64]={0};
                    parse_resp(buf, cmd, a1, a2, a3);
                    printf("CMD: %s %s %s %s\n", cmd, a1, a2, a3);

                    if (strcmp(cmd, "SET")==0) { set(GLOBAL_DB, a1, a2); send_ok(s); }
                    else if (strcmp(cmd, "GET")==0) { send_bulk(s, get(GLOBAL_DB, a1)); }
                    else if (strcmp(cmd, "EXPIRE")==0) { send_int(s, expire(GLOBAL_DB, a1, atoi(a2))); }
                    else if (strcmp(cmd, "TTL")==0) { send_int(s, ttl(GLOBAL_DB, a1)); }
                    else if (strcmp(cmd, "LPUSH")==0) { 
                        int r=lpush(GLOBAL_DB, a1, a2); 
                        if(r==-1) send_err(s,"WRONGTYPE"); else send_int(s,r); 
                    }
                    else if (strcmp(cmd, "LRANGE")==0) { send_bulk(s, lrange(GLOBAL_DB, a1)); }
                    else if (strcmp(cmd, "SADD")==0) { 
                        int r=sadd(GLOBAL_DB, a1, a2); 
                        if(r==-1) send_err(s,"WRONGTYPE"); else send_int(s,r); 
                    }
                    else if (strcmp(cmd, "SMEMBERS")==0) { send_bulk(s, smembers(GLOBAL_DB, a1)); }
                    else if (strcmp(cmd, "HSET")==0) { 
                        int r=hset(GLOBAL_DB, a1, a2, a3); 
                        if(r==-1) send_err(s,"WRONGTYPE"); else send_int(s,r); 
                    }
                    else if (strcmp(cmd, "HGET")==0) { send_bulk(s, hget(GLOBAL_DB, a1, a2)); }
                    else if (strcmp(cmd, "SAVE")==0) { save_to_disk(); send_ok(s); }
                    else if (strcmp(cmd, "PING")==0) send(s, "+PONG\r\n", 7, 0);
                    else if (strcmp(cmd, "COMMAND")==0) send_err(s, "Unsup"); // Keep Docker happy
                    else send_err(s, "Unknown");
                }
            }
        }
    }
    return 0;
}