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

struct ThreadContent
{
    int broker_sock_productor;
    MultiPartitionQueue *mp_queue;
};

void *handle_productor(void *arg);
void *handle_productor_connections(void *arg);
int initialize_broker_socket(int port);
void splitAndEnqueue(char *cadena, char *delimiter1, MultiPartitionQueue *mp_queue);

#endif
