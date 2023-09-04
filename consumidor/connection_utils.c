#include "connection_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

int establishBrokerConnection(BrokerConnection *connection, const char *ip, int port)
{
    connection->broker_ip = strdup(ip);
    connection->broker_port = port;

    connection->broker_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (connection->broker_socket == -1)
    {
        perror("Socket creation failed");
        return -1;
    }

    struct sockaddr_in broker_addr;
    broker_addr.sin_family = AF_INET;
    broker_addr.sin_addr.s_addr = inet_addr(ip);
    broker_addr.sin_port = htons(port);

    if (connect(connection->broker_socket, (struct sockaddr *)&broker_addr, sizeof(broker_addr)) < 0)
    {
        perror("Connection with broker failed");
        close(connection->broker_socket);
        return -1;
    }

    printf("Connected to the broker at %s:%d\n", ip, port);
    return 0;
}

int sendMessageToBroker(BrokerConnection *connection, const char *message)
{
    printf("Se enviara el mensaje: %s al broker\n", message);
    if (send(connection->broker_socket, message, strlen(message), 0) == -1)
    {
        perror("Error sending message to broker");
        return -1;
    }
    return 0;
}

int receiveMessageFromBroker(BrokerConnection *connection, char *message)
{
    int read_size = recv(connection->broker_socket, message, MAX_MESSAGE_LENGTH, 0);
    if (read_size == -1)
    {
        perror("Error receiving message from broker");
        return -1;
    }
    message[read_size] = '\0';
    return 0;
}

void closeBrokerConnection(BrokerConnection *connection)
{
    close(connection->broker_socket);
    free(connection->broker_ip);
}
