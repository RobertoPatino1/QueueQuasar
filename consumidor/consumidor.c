#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "connection_utils.h"

int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        printf("Uso: %s <broker_ip> <broker_port> <request>\n", argv[0]);
        return 1;
    }

    char *broker_ip = argv[1];
    int broker_port = atoi(argv[2]);
    char *request = argv[3];

    BrokerConnection brokerConnection;

    if (establishBrokerConnection(&brokerConnection, broker_ip, broker_port) == -1)
    {
        return 1;
    }

    if (sendMessageToBroker(&brokerConnection, request) == -1)
    {
        closeBrokerConnection(&brokerConnection);
        return 1;
    }

    char response[MAX_MESSAGE_LENGTH];
    if (receiveMessageFromBroker(&brokerConnection, response) == -1)
    {
        closeBrokerConnection(&brokerConnection);
        return 1;
    }

    printf("Response from broker: %s\n", response);

    closeBrokerConnection(&brokerConnection);

    return 0;
}
