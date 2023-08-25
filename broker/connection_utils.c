#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "connection_utils.h"

int acceptClientConnection(int server_socket, ClientConnection *connection)
{
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    connection->client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_len);
    if (connection->client_socket == -1)
    {
        perror("Accepting connection failed");
        return -1;
    }
    return 0;
}

int receiveMessageFromClient(ClientConnection *connection, char *buffer, size_t buffer_size)
{
    ssize_t bytes_received = recv(connection->client_socket, buffer, buffer_size - 1, 0);
    if (bytes_received <= 0)
    {
        // Error or connection closed
        return -1;
    }
    buffer[bytes_received] = '\0'; // Ensure buffer is a valid string
    return bytes_received;
}

void closeClientConnection(ClientConnection *connection)
{
    close(connection->client_socket);
}
