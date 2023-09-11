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
extern MultiPartitionQueue *mp_queue_producer;
struct ThreadContent
{
    int broker_sock_producer;
    MultiPartitionQueue *mp_queue;
};

void *handle_productor(void *arg);
void *handle_consumidor(void *arg);
void *handle_producer_connections(void *arg);
void *handle_consumer_connections(void *arg);
int generateOption(char *specific_request);
void splitAndEnqueue(char *string);
void send_message_to_consumer(int consumer_socket, int option);
#endif
