#ifndef CONNECTION_UTILS_H
#define CONNECTION_UTILS_H

#include <netinet/in.h>

typedef struct
{
    int client_socket;
} ClientConnection;

int acceptClientConnection(int server_socket, ClientConnection *connection);
int receiveMessageFromClient(ClientConnection *connection, char *buffer, size_t buffer_size);
void closeClientConnection(ClientConnection *connection);

#endif
