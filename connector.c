#include "connection.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>

int establishConnection(const char *ip, int port)
{
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1)
    {
        perror("Error al crear el socket");
        return -1;
    }

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(ip);
    server.sin_port = htons(port);

    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) == -1)
    {
        perror("Error de conexión");
        return -1;
    }

    printf("Conexión establecida con éxito a %s:%d\n", ip, port);
    return sock;
}

void closeConnection(int sock)
{
    close(sock);
    printf("Conexión cerrada\n");
}
