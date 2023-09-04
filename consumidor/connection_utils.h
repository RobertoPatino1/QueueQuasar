#ifndef CONNECTION_UTILS_H
#define CONNECTION_UTILS_H

#define MAX_MESSAGE_LENGTH 256

// Estructura para almacenar información sobre la conexión al broker
typedef struct
{
    char *broker_ip;
    int broker_port;
    int broker_socket;
} BrokerConnection;

// Función para establecer una conexión con el broker
int establishBrokerConnection(BrokerConnection *connection, const char *ip, int port);

// Función para enviar un mensaje al broker
int sendMessageToBroker(BrokerConnection *connection, const char *message);

// Función para recibir un mensaje del broker
int receiveMessageFromBroker(BrokerConnection *connection, char *message);

// Función para cerrar la conexión con el broker
void closeBrokerConnection(BrokerConnection *connection);

#endif
