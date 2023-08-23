#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: %s <puerto>\n", argv[0]);
        return 1;
    }

    int server_port = atoi(argv[1]);
    if (server_port <= 0)
    {
        printf("Error: Invalid port\n");
        return 1;
    }

    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    // Create socket
    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock == -1)
    {
        printf("Could not create socket\n");
        return 1;
    }

    // Set up server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(server_port);

    // Bind
    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        printf("Bind failed\n");
        return 1;
    }

    // Listen
    listen(server_sock, 10);
    printf("Consumer listening on port %d...\n", server_port);

    // Accept incoming connections
    client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &client_addr_len);
    if (client_sock < 0)
    {
        printf("Accept failed\n");
        return 1;
    }

    // Receive and print messages
    char buffer[256];
    int read_size;
    while ((read_size = recv(client_sock, buffer, sizeof(buffer), 0)) > 0)
    {
        buffer[read_size] = '\0'; // Null-terminate the received data
        printf("Received message: %s\n", buffer);
    }

    // Close the socket and exit
    printf("Connection closed\n");
    close(client_sock);
    close(server_sock);
    return 0;
}
