#ifndef CONNECTION_UTILS_H
#define CONNECTION_UTILS_H

typedef struct
{
    char *broker_ip;
    int broker_port;
    int broker_socket;
} BrokerConnection;

int establishBrokerConnection(BrokerConnection *connection, const char *ip, int port);

int sendMessageToBroker(BrokerConnection *connection, const char *message);

void closeBrokerConnection(BrokerConnection *connection);

#endif
