#ifndef CONNECTION_UTILS_H
#define CONNECTION_UTILS_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <unistd.h>
#include "multiPartitionQueue.h"

#define MAX_MESSAGE_LENGTH 256

extern pthread_mutex_t mutex;
extern MultiPartitionQueue *mp_queue_productor;
struct ThreadContent
{
    int broker_sock_productor;
    MultiPartitionQueue *mp_queue;
};
// int persistencia;

void *handle_productor(void *arg);
void *handle_consumidor(void *arg);
void *handle_productor_connections(void *arg);
void *handle_consumidor_connections(void *arg);
int generateOption(char *solicitud_especifica);
void splitAndEnqueue(char *cadena);
void send_message_to_consumidor(int consumidor_socket, int opcion);
#endif
