#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>

#define BUFFER_SIZE 1024

// Function to handle HTTP GET requests
void handle_get_request(int client_socket, const char *path) {
    char response_header[BUFFER_SIZE];
    char response_data[BUFFER_SIZE];
    FILE *file;

    // Check if the requested file exists
    file = fopen(path, "r");
    if (file == NULL) {
        sprintf(response_header, "HTTP/1.1 404 Not Found\r\n\r\n");
        send(client_socket, response_header, strlen(response_header), 0);
        return;
    }

    // Read and send the file content
    sprintf(response_header, "HTTP/1.1 200 OK\r\n\r\n");
    send(client_socket, response_header, strlen(response_header), 0);

    while (fgets(response_data, sizeof(response_data), file) != NULL) {
        send(client_socket, response_data, strlen(response_data), 0);
    }

    fclose(file);
}

// Function to handle HTTP POST requests
void handle_post_request(int client_socket, const char *path, const char *data) {
    char response[BUFFER_SIZE];
    FILE *file;

    // Open the file for writing
    file = fopen(path, "w");
    if (file == NULL) {
        perror("Error opening file");
        sprintf(response, "HTTP/1.1 500 Internal Server Error\r\n\r\n");
        send(client_socket, response, strlen(response), 0);
        return;
    }
    
    printf("Received POST data:\n%s\n", data);

    // Write the POST data to the file
    if (data != NULL) {
        size_t data_length = strlen(data);
        fwrite(data, 1, data_length, file);
    }
    fclose(file);

    // Send a success response
    sprintf(response, "HTTP/1.1 200 OK\r\n\r\n");
    strcat(response, "Data saved successfully.\n");
    send(client_socket, response, strlen(response), 0);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        return 1;
    }

    int port = atoi(argv[1]);
    int server_socket, client_socket;
    struct sockaddr_in server_address, client_address;
    socklen_t client_address_length = sizeof(client_address);

    // Create a socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Socket creation failed");
        return 1;
    }

    // Initialize server address
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons(port);

    // Bind the socket to the specified port
    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1) {
        perror("Socket bind failed");
        return 1;
    }

    // Listen for incoming connections
    if (listen(server_socket, 5) == -1) {
        perror("Socket listen failed");
        return 1;
    }

    printf("HTTP server is listening on port %d...\n", port);

    while (1) {
        // Accept a connection from a client
        client_socket = accept(server_socket, (struct sockaddr *)&client_address, &client_address_length);
        if (client_socket == -1) {
            perror("Connection accept failed");
            continue;
        }

        // Read the client's HTTP request
        char request[BUFFER_SIZE];
        recv(client_socket, request, sizeof(request), 0);

        // Parse the HTTP request
        char *method = strtok(request, " ");
        char *path = strtok(NULL, " ");
        char *http_version = strtok(NULL, "\r\n");
        char *post_data = strstr(request, "\r\n\r\n");

        if (method == NULL || path == NULL || http_version == NULL) {
            perror("Invalid HTTP request");
            close(client_socket);
            continue;
        }

        printf("Received HTTP request: %s %s %s\n", method, path, http_version);

        if (strcmp(method, "GET") == 0) {
            handle_get_request(client_socket, path + 1); // Remove leading '/'
        } else if (strcmp(method, "POST") == 0) {
            handle_post_request(client_socket, path + 1, post_data != NULL ? post_data + 4 : NULL); // +4 to skip "\r\n\r\n"
        } else {
            perror("Unsupported HTTP method");
        }

        // Close the client socket
        close(client_socket);
    }

    // Close the server socket
    close(server_socket);

    return 0;
}
