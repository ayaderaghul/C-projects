#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <unistd.h>

#include "main.h"

#pragma comment(lib, "ws2_32.lib")


int main() {
    WSADATA wsa;
    if(WSAStartup(MAKEWORD(2,2), &wsa)!=0){
        printf("failed to start winsock\n");
        return 1;
    };


    SOCKET server_fd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr= INADDR_ANY;
    address.sin_port=htons(6379);


    bind(server_fd,(struct sockaddr*)&address, sizeof(address));

    listen(server_fd, 3);
    printf("waiting for connection..\n");

    while(1) {
        SOCKET client_fd=accept(server_fd, NULL, NULL);
        char buffer[1024] = {0};
        recv(client_fd, buffer,1024,0);

        char cmd[10], arg1[32], arg2[32];
        sscanf(buffer, "%s %s %s", cmd, arg1, arg2);

        char response[100];

        if(strcmp(cmd,"SET")==0){
            kv_set(arg1,arg2);
            sprintf(response, "OK saved %s\n", arg1);
        } else if(strcmp(cmd, "GET")==0){
            char *val=kv_get(arg1);
            sprintf(response, "VALUE: %s\n", val);
        } else {
            sprintf(response, "ERROR: unknown command\n");
        }

        send(client_fd, response, strlen(response), 0);
        closesocket(client_fd);
    }

    printf("someone connected\n");

    closesocket(server_fd);
    WSACleanup();
    return 0;
}