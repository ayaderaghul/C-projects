#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <winsock2.h>
#include <time.h>
#include "hash.h"

    HashTable* GLOBAL_DB;
    time_t last_save_time;

unsigned int hash(char*key, int table_size) {
    // djb2 algo
    unsigned int h = 5381; 
    int c;

    while((c=*key++)) h = (h << 5) + h + c;

    return h % table_size;
}

HashTable*create_table(int size){
    HashTable*table=malloc(sizeof(HashTable));
    if (!table){
        printf("malloc failed\n");
        return NULL;
    }
    table->size=size;
    table->buckets=(Entry**)calloc(size,sizeof(Entry*));
    return table;
}

void destroy_table(HashTable*table){
    for (int i=0;i<table->size;i++){
        Entry*curr=table->buckets[i];
        while(curr){
            Entry*next_entry=curr->next;
            free(curr);
            curr=next_entry;
        }
    }
    free(table->buckets);
    free(table);
}

void set(HashTable*table, char*key, char*value){
    unsigned int slot = hash(key, table->size);

    Entry*entry=table->buckets[slot];

    while(entry){
        if(strcmp(entry->key, key)==0){
            free(entry->value);
            entry->value=strdup(value);
            return;
        }
        entry=entry->next;
    }

    Entry*new_entry=malloc(sizeof(Entry));
    new_entry->key=strdup(key);
    new_entry->value=strdup(value);

    new_entry->next=table->buckets[slot];
    table->buckets[slot]=new_entry;
}

char*get(HashTable*table, char*key){
    unsigned int slot = hash(key, table->size);

    Entry *entry=table->buckets[slot];

    while(entry){
        if(strcmp(entry->key,key)==0){
            return entry->value;
        }
        entry=entry->next;
    };

    return NULL;
}

int delete(HashTable*table, char*key){
    unsigned int slot = hash(key, table->size);

    Entry *entry=table->buckets[slot];
    Entry *prev=NULL;

    while(entry){
        if(strcmp(entry->key,key)==0){
            if(prev==NULL){
                table->buckets[slot]=entry->next;
            } else {
                prev->next=entry->next;
            }

            free(entry->key);
            free(entry->value);
            free(entry);
            table->size--;
            return 0;
        }
        prev=entry;
        entry=entry->next;
    };
    return -1;
}

void save_to_disk(){
    printf("snapshot:saving to disk..\n");
    FILE *fp=fopen("dump.db", "wb");
    if(!fp) return;

    // iterate over every bucket
    for (int i=0; i< GLOBAL_DB->size;i++){
        Entry *e = GLOBAL_DB->buckets[i];
        while(e) {
            // ram entry -> disk entry
            DiskEntry d;
            memset(&d,0,sizeof(DiskEntry)); // clear memory
            strncpy(d.key, e->key, 63);
            strncpy(d.value, e->value, 63);

            fwrite(&d, sizeof(DiskEntry), 1, fp);
            e=e->next;
        }
    }
    fclose(fp);
    printf("done\n");
    last_save_time=time(NULL); // reset timer
}

void load_from_disk(){
    FILE *fp=fopen("dump.db", "rb");
    if(!fp) {
        printf("snapshot: no db found, start fresh\n");
        return;
    }

    printf("snapshot: loading data...\n");
    DiskEntry d;
    // read one diskentry at a time until EOF
    while(fread(&d, sizeof(DiskEntry), 1, fp)){
        set(GLOBAL_DB, d.key, d.value);
    }
    fclose(fp);
    printf("loaded\n");
}


// RESP parser
void parse_resp(char *buffer, char *cmd, char*arg1, char*arg2){
    cmd[0]=0; arg1[0]=0;arg2[0]=0;
    if(buffer[0]!='*') return; // buffer starts with * (array)
    char *cursor = buffer;

    // skip the first line *3\r\n
    cursor = strstr(cursor, "\r\n");
    if(!cursor) return;
    cursor+=2; // move past \r\n

    // loop to extract words
    // $len\r\nWORD\r\n
    int arg_index=0;
    while(*cursor != '\0'){
        if(*cursor=='$'){
            cursor=strstr(cursor, "\r\n"); // skip the $len line
            if(!cursor) break;
            cursor+=2;


            char *end=strstr(cursor, "\r\n");
            if(!end) break;

            int len = end-cursor;
            if(arg_index==0){
                strncpy(cmd, cursor,len);
                cmd[len]='\0';
            } else if (arg_index==1){
                strncpy(arg1, cursor,len);
                arg1[len]='\0';
            } else if (arg_index==2){
                strncpy(arg2, cursor, len);
                arg2[len]='\0';
            }
            arg_index++;
            cursor=end+2;
        } else {
            break;
        }
    }
}

// send +OK\r\n
void send_ok(SOCKET fd) {
    char *msg = "+OK\r\n";
    send(fd, msg, strlen(msg), 0);
}


// send $5\r\nvalue\r\n
void send_bulk(SOCKET fd, char*str) {
    if(str==NULL){
        char *msg = "$-1\r\n"; // NULL reply
        send(fd, msg, strlen(msg), 0);
        return;
    }
    char buffer[1024];
    // format $length \r\n string \r\n
    sprintf(buffer, "$%d\r\n%s\r\n", (int)strlen(str), str);
    send(fd,buffer, strlen(buffer),0);
}
//send -Error msg\r\n
void send_error(SOCKET fd, char *msg){
    char buffer[1024];
    sprintf(buffer, "-ERR %s\r\n", msg);
    send(fd, buffer,strlen(buffer),0);
}

int main(){

    GLOBAL_DB=create_table(100);
    load_from_disk();
    last_save_time=time(NULL);
    // initialize windows network
    WSADATA wsa; WSAStartup(MAKEWORD(2,2), &wsa);
    SOCKET master_socket = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET; addr.sin_addr.s_addr = INADDR_ANY; addr.sin_port = htons(6379);
    bind(master_socket, (struct sockaddr *)&addr, sizeof(addr));
    listen(master_socket, 5);

    // list of client sockets
    SOCKET clients[30];
    for (int i=0; i<30;i++) clients[i]=0;

    printf("MINI multiclient REDIS (snapshot server): ready on port 6379...\n");


    while(1) {
        // 1. Prepare the Set
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(master_socket, &readfds);
        SOCKET max_sd = master_socket;

        for(int i=0; i<30; i++) {
            SOCKET sd = clients[i];
            if(sd > 0) FD_SET(sd, &readfds);
            if(sd > max_sd) max_sd = sd; // Not needed on Windows, but good habit
        }

        // 2. Wait (Timeout 1 sec to check autosave)
        struct timeval timeout;
        timeout.tv_sec = 1; timeout.tv_usec = 0;
        
        int activity = select(0, &readfds, NULL, NULL, &timeout);

        // Check Auto-Save (Non-blocking now!)
        if(time(NULL) - last_save_time > 60) save_to_disk();

        if (activity < 0) printf("Select error\n");

        // 3. New Connection?
        if (FD_ISSET(master_socket, &readfds)) {
            SOCKET new_socket = accept(master_socket, NULL, NULL);
            printf("New connection!\n");
            
            // Add to list
            for (int i=0; i<30; i++) {
                if (clients[i] == 0) {
                    clients[i] = new_socket;
                    break;
                }
            }
        }

        // 4. IO on existing clients
        for (int i=0; i<30; i++) {
            SOCKET s = clients[i];
            if (FD_ISSET(s, &readfds)) {
                char buf[1024] = {0};
                int valread = recv(s, buf, 1024, 0);

                if (valread <= 0) {
                    closesocket(s);
                    clients[i] = 0; // Remove from list
                    printf("Client disconnected.\n");
                } else {
                    // Logic
                    char cmd[64]={0}, a1[64]={0}, a2[64]={0};
                    parse_resp(buf, cmd, a1, a2);
                    printf("CMD: %s\n", cmd);

                    if(strcmp(cmd, "SET")==0) { set(GLOBAL_DB, a1, a2); send_ok(s); }
                    else if(strcmp(cmd, "GET")==0) { send_bulk(s, get(GLOBAL_DB, a1)); }
                    else if(strcmp(cmd, "PING")==0) { send(s, "+PONG\r\n", 7, 0); }
                    else if(strcmp(cmd, "SAVE")==0) { save_to_disk(); send_ok(s); }
                    else if(strcmp(cmd, "COMMAND")==0) { send_error(s, "Not supported"); }
                    else { send_error(s, "Unknown"); }
                }
            }
        }
    }
    return 0;
}