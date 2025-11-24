#include <stdio.h>
#include <string.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

int main() {
    // 1. Setup Windows Network (Once at the start)
    WSADATA wsa;
    WSAStartup(MAKEWORD(2,2), &wsa);

    printf("--- INTERACTIVE REDIS CLIENT ---\n");
    printf("Type 'exit' to quit.\n\n");

    char input[1024]; // Buffer for your typing

    while(1) {
        // 2. Show prompt and get user input
        printf("redis> ");
        fgets(input, 1024, stdin); // Wait for you to type and hit Enter

        // Remove the "Enter" key (\n) from the end of the string
        input[strcspn(input, "\n")] = 0;

        // Check if user wants to quit
        if (strcmp(input, "exit") == 0) {
            break;
        }

        // 3. Create Socket (We do this EVERY time)
        SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
        
        struct sockaddr_in serv_addr;
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(6379);
        serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

        // 4. Connect
        if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
            printf("Connection Failed. Is server running?\n");
            closesocket(sock);
            continue; // Go back to top of loop
        }

        // 5. Send your typed command
        send(sock, input, strlen(input), 0);

        // 6. Read Reply
        char buffer[1024] = {0};
        recv(sock, buffer, 1024, 0);
        printf("%s", buffer); // Server sends a newline, so just print buffer

        // 7. Close this specific connection
        closesocket(sock);
    }

    // Cleanup when "exit" is typed
    WSACleanup();
    return 0;
}