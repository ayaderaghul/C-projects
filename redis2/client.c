#include <stdio.h>
#include <string.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

int main() {
    // 1. Setup Windows Network
    WSADATA wsa;
    WSAStartup(MAKEWORD(2,2), &wsa);

    // 2. Create Socket
    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    
    // 3. Connect to Localhost (127.0.0.1) on port 6379
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(6379);
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("CLIENT: Connection Failed. Is the server running?\n");
        return 1;
    }

    // 4. Send Command
    // Change this string to test different things!
    char *message = "SET mykey hello_redis"; 
    send(sock, message, strlen(message), 0);
    printf("CLIENT: Sent -> %s\n", message);

    // 5. Read Reply
    char buffer[1024] = {0};
    recv(sock, buffer, 1024, 0);
    printf("CLIENT: Server replied -> %s\n", buffer);

    // 6. Cleanup
    closesocket(sock);
    WSACleanup();
    return 0;
}