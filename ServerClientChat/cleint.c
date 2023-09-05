// Client.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int client_socket;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];

    // Create a socket
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0) {
        perror("Error in socket");
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = PORT;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Server IP address

    // Connect to the server
    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Error in connection");
        exit(1);
    }

    // Conversation loop
    while (1) {
        memset(buffer, 0, sizeof(buffer)); // Clear the buffer
        recv(client_socket, buffer, sizeof(buffer), 0); // Receive server's message

        printf("Server: %s", buffer);

        if (strcmp(buffer, "exit\n") == 0) {
            printf("Server is exiting...\n");
            break;
        }

        memset(buffer, 0, sizeof(buffer)); // Clear the buffer
        printf("Client: ");
        fgets(buffer, sizeof(buffer), stdin); // Read input from the client's console

        // Send the message to the server
        send(client_socket, buffer, strlen(buffer), 0);

        if (strcmp(buffer, "exit\n") == 0) {
            printf("Client is exiting...\n");
            break;
        }
    }

    close(client_socket);

    return 0;
}
