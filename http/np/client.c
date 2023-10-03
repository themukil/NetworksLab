#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define SERVER_IP "127.0.0.1"
#define PORT 8080
#define MAX_REQUESTS 3

void send_http_request(int sockfd, int request_number) {
    char request[] = "GET / HTTP/1.1\r\nHost: " SERVER_IP "\r\n\r\n";

    printf("Sending Request #%d:\n%s", request_number, request);
    send(sockfd, request, strlen(request), 0);
}

int main() {
    int sockfd;
    struct sockaddr_in server_addr;

    for (int i = 1; i <= MAX_REQUESTS; i++) {
        // Create a socket
        if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            perror("Socket creation failed");
            exit(EXIT_FAILURE);
        }

        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(PORT);

        // Convert the IP address from string to binary
        if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0) {
            perror("Invalid address/ Address not supported");
            exit(EXIT_FAILURE);
        }

        // Connect to the server
        if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
            perror("Connection failed");
            exit(EXIT_FAILURE);
        }

        send_http_request(sockfd, i);

        char response[4096];
        ssize_t bytes_received;

        // Receive and print the server's response
        bytes_received = recv(sockfd, response, sizeof(response) - 1, 0);

        if (bytes_received > 0) {
            response[bytes_received] = '\0';
            printf("\nResponse #%d received:\n%s\n", i, response);
        }

        // Close the connection after receiving the response
        close(sockfd);
    }

    return 0;
}
