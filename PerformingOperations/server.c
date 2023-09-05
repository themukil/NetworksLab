#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int server_socket, new_socket;
    struct sockaddr_in server_addr, new_addr;
    socklen_t addr_size;
    char buffer[BUFFER_SIZE];

    // Create a socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("Error in socket");
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = PORT;
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // Bind the socket
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Error in binding");
        exit(1);
    }

    // Listen for incoming connections
    if (listen(server_socket, 10) == 0) {
        printf("Listening...\n");
    } else {
        printf("Error in listening\n");
        exit(1);
    }

    addr_size = sizeof(new_addr);
    new_socket = accept(server_socket, (struct sockaddr *)&new_addr, &addr_size); // Accept a new connection

    // Conversation loop
    while (1) {
        memset(buffer, 0, sizeof(buffer)); // Clear the buffer
        recv(new_socket, buffer, sizeof(buffer), 0); // Receive client's message

        if (strcmp(buffer, "exit\n") == 0) {
            printf("Server is exiting...\n");
            break;
        }

        int num1, num2;
        sscanf(buffer, "%d %d", &num1, &num2); // Parse the received message as two integers

        memset(buffer, 0, sizeof(buffer)); // Clear the buffer

        // Perform arithmetic operations
        int sum = num1 + num2;
        int difference = num1 - num2;
        int product = num1 * num2;
        int quotient = num1 / num2;

        // Format the result as a string
        sprintf(buffer, "Sum: %d\nDifference: %d\nProduct: %d\nQuotient: %d\n", sum, difference, product, quotient);

        // Send the result to the client
        send(new_socket, buffer, strlen(buffer), 0);
    }

    close(new_socket);
    close(server_socket);

    return 0;
}
