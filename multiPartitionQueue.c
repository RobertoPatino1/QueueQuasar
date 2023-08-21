#include "multiPartitionQueue.h"

MultiPartitionQueue *createMultiPartitionQueue(int numPartitions)
{
    MultiPartitionQueue *queue = (MultiPartitionQueue *)malloc(sizeof(MultiPartitionQueue));
    if (queue == NULL)
    {
        perror("Error creating multi-partition queue");
        return NULL;
    }

    queue->numPartitions = numPartitions;
    queue->partitions = (Partition *)malloc(numPartitions * sizeof(Partition));
    if (queue->partitions == NULL)
    {
        perror("Error creating partitions");
        free(queue);
        return NULL;
    }

    for (int i = 0; i < numPartitions; i++)
    {
        queue->partitions[i].queue = (Queue *)malloc(sizeof(Queue));
        if (queue->partitions[i].queue == NULL)
        {
            perror("Error creating partition queue");
            freeMultiPartitionQueue(queue);
            return NULL;
        }
        queue->partitions[i].queue->front = queue->partitions[i].queue->rear = NULL;
    }

    return queue;
}

void enqueue(MultiPartitionQueue *queue, char *sectionName, int partitionIndex, char *data)
{
    int partitionNumber = partitionIndex + 1;
    printf("Enqueuing message: \"%s\" into %s, partition %d\n", data, sectionName, partitionNumber);
    if (partitionIndex >= 0 && partitionIndex < queue->numPartitions)
    {
        QueueNode *newNode = (QueueNode *)malloc(sizeof(QueueNode));
        if (newNode == NULL)
        {
            perror("Error enqueuing data");
            return;
        }
        newNode->data = data;
        newNode->next = NULL;

        if (queue->partitions[partitionIndex].queue->rear == NULL)
        {
            queue->partitions[partitionIndex].queue->front = queue->partitions[partitionIndex].queue->rear = newNode;
        }
        else
        {
            queue->partitions[partitionIndex].queue->rear->next = newNode;
            queue->partitions[partitionIndex].queue->rear = newNode;
        }
    }
}

char *dequeue(MultiPartitionQueue *queue, char *sectionName, int partitionIndex)
{
    if (partitionIndex >= 0 && partitionIndex < queue->numPartitions)
    {
        if (queue->partitions[partitionIndex].queue->front == NULL)
        {
            return NULL; // Empty queue
        }

        QueueNode *temp = queue->partitions[partitionIndex].queue->front;
        char *data = temp->data;
        queue->partitions[partitionIndex].queue->front = temp->next;

        if (queue->partitions[partitionIndex].queue->front == NULL)
        {
            queue->partitions[partitionIndex].queue->rear = NULL;
        }

        free(temp);
        return data;
    }
    return NULL;
}

void printPartitionContents(MultiPartitionQueue *queue, char *sectionName, int partitionIndex)
{
    if (partitionIndex >= 0 && partitionIndex < queue->numPartitions)
    {
        printf("Contents of %s Partition %d:\n", sectionName, partitionIndex);
        QueueNode *current = queue->partitions[partitionIndex].queue->front;
        printf("[");
        while (current != NULL)
        {
            printf("%s", current->data);
            current = current->next;
            if (current != NULL)
            {
                printf(", ");
            }
        }
        printf("]\n");
    }
}

void freeQueue(Queue *q)
{
    while (q->front != NULL)
    {
        QueueNode *temp = q->front;
        q->front = q->front->next;
        free(temp);
    }
    free(q);
}

void freeMultiPartitionQueue(MultiPartitionQueue *queue)
{
    for (int i = 0; i < queue->numPartitions; i++)
    {
        freeQueue(queue->partitions[i].queue);
    }
    free(queue->partitions);
    free(queue);
}
