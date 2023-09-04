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

pthread_mutex_t mutex;
MultiPartitionQueue *mp_queue_productor;
struct ThreadContent
{
    int broker_sock_productor;
    MultiPartitionQueue *mp_queue;
};

void *handle_productor(void *arg);
void *handle_consumidor_read(void *arg);
void *handle_consumidor_send(void *arg);
void *handle_productor_connections(void *arg);
void *handle_consumidor_connections_read(void *arg);
void *handle_consumidor_connections_send(void *arg);
int initialize_broker_socket(int port);
void splitAndEnqueue(char *cadena);
int send_message_to_consumidor(int consumidor_socket, const char *message);

#endif
