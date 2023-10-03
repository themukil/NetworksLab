#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>

#define BUFFER_SIZE 1024

void send_get_request(const char *host, int port, const char *path) {
    int client_socket;
    struct sockaddr_in server_addr;
    struct hostent *server;

    // Create a socket
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0) {
        perror("Error creating socket");
        exit(1);
    }

    // Get the server's IP address
    server = gethostbyname(host);
    if (server == NULL) {
        perror("Error getting host by name");
        exit(1);
    }

    // Initialize server address structure
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    memcpy(&server_addr.sin_addr.s_addr, server->h_addr, server->h_length);

    // Connect to the server
    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Error connecting to server");
        exit(1);
    }

    // Send the GET request
    char request[BUFFER_SIZE];
    snprintf(request, sizeof(request), "GET %s HTTP/1.1\r\nHost: %s\r\n\r\n", path, host);
    send(client_socket, request, strlen(request), 0);

    // Receive and print the response
    char response[BUFFER_SIZE];
    while (recv(client_socket, response, sizeof(response), 0) > 0) {
        printf("%s", response);
        memset(response, 0, sizeof(response));
    }

    close(client_socket);
}

void send_post_request(const char *host, int port, const char *path, const char *data) {
    int client_socket;
    struct sockaddr_in server_addr;
    struct hostent *server;

    // Create a socket
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0) {
        perror("Error creating socket");
        exit(1);
    }

    // Get the server's IP address
    server = gethostbyname(host);
    if (server == NULL) {
        perror("Error getting host by name");
        exit(1);
    }

    // Initialize server address structure
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    memcpy(&server_addr.sin_addr.s_addr, server->h_addr, server->h_length);

    // Connect to the server
    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Error connecting to server");
        exit(1);
    }

    // Send the POST request with data
    char request[BUFFER_SIZE];
    snprintf(request, sizeof(request), "POST %s HTTP/1.1\r\nHost: %s\r\nContent-Length: %ld\r\n\r\n%s",
             path, host, strlen(data) + 2, data);
             
     // Print the POST request before sending
    printf("Sending POST request:\n%s\n", request);
    
    send(client_socket, request, strlen(request), 0);

    // Receive and print the response
    char response[BUFFER_SIZE];
    while (recv(client_socket, response, sizeof(response), 0) > 0) {
        printf("%s", response);
        memset(response, 0, sizeof(response));
    }

    close(client_socket);
}

int main(int argc, char *argv[]) {
    if (argc < 5) {
        fprintf(stderr, "Usage: %s <host> <port> <path> <method: GET or POST> [data]\n", argv[0]);
        return 1;
    }

    const char *host = argv[1];
    int port = atoi(argv[2]);
    const char *path = argv[3];
    const char *method = argv[4];

    if (strcmp(method, "GET") == 0) {
        send_get_request(host, port, path);
    } else if (strcmp(method, "POST") == 0) {
        if (argc == 6) {
            const char *data = argv[5];
            send_post_request(host, port, path, data);
        } else {
            fprintf(stderr, "Missing data for POST request.\n");
            return 1;
        }
    } else {
        fprintf(stderr, "Invalid method.\n");
        return 1;
    }

    return 0;
}
