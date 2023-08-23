#ifndef MULTI_PARTITION_QUEUE_H
#define MULTI_PARTITION_QUEUE_H

#include <stdio.h>
#include <stdlib.h>

typedef struct QueueNode
{
    char *data;
    struct QueueNode *next;
} QueueNode;

typedef struct Queue
{
    QueueNode *front;
    QueueNode *rear;
} Queue;

typedef struct Partition
{
    Queue *queue;
} Partition;

typedef struct MultiPartitionQueue
{
    int numPartitions;
    Partition *partitions;
} MultiPartitionQueue;

MultiPartitionQueue *createMultiPartitionQueue(int numPartitions);
void enqueue(MultiPartitionQueue *queue, char *sectionName, int partitionIndex, char *data);
char *dequeue(MultiPartitionQueue *queue, char *sectionName, int partitionIndex);
void printPartitionContents(MultiPartitionQueue *queue, char *sectionName, int partitionIndex);
void freeMultiPartitionQueue(MultiPartitionQueue *queue);

#endif