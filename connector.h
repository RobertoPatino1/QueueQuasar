#ifndef CONNECTOR_H
#define CONNECTOR_H

// Declaración de funciones
int establishConnection(const char *ip, int port);
void closeConnection(int sock);

#endif
