#include "connection_utils.h"
int persistence;
MultiPartitionQueue *mp_queue_producer;

void *handle_producer(void *arg)
{
    struct ThreadContent *data = (struct ThreadContent *)arg;
    int producer_sock = (int)data->broker_sock_producer;

    char message[MAX_MESSAGE_LENGTH];
    int read_size;

    while ((read_size = recv(producer_sock, message, sizeof(message), 0)) > 0)
    {
        message[read_size] = '\0';
        printf("\n---Message recieved from producer: %s---\n", message);
        splitAndEnqueue(message);
        sleep(2);
    }

    printf("Producer disconnected\n");
    close(producer_sock);
    pthread_exit(NULL);
}

void *handle_consumer(void *arg)
{
    int consumer_sock = *(int *)arg;
    char request[MAX_MESSAGE_LENGTH];
    int option;
    int read_size;
    read_size = recv(consumer_sock, request, sizeof(request), 0);
    if (read_size == -1)
    {
        perror("Conection closed");
        close(consumer_sock);
    }
    request[read_size] = '\0';

    printf("\nRequest recieved from consumer: %s\n", request);

    char *sepecific_request = strtok(request, "/");
    sepecific_request = strtok(NULL, "-");

    persistence = atoi(strtok(NULL, "-"));
    printf("\nUse persistence? %d\n", persistence); // Si es 1, escribimos en un archivo!!!
    option = generateOption(sepecific_request);
    while (1)
    {
        send_message_to_consumer(consumer_sock, option);
        sleep(5);
    }
}

void *handle_producer_connections(void *arg)
{
    struct ThreadContent *data = (struct ThreadContent *)arg;
    int broker_sock_producer = (int)data->broker_sock_producer;

    struct sockaddr_in broker_addr_producer;
    int c = sizeof(struct sockaddr_in);

    while (1)
    {
        int producer_sock = accept(broker_sock_producer, (struct sockaddr *)&broker_addr_producer, (socklen_t *)&c);
        if (producer_sock < 0)
        {
            printf("Error accepting producer connection\n");
            continue;
        }
        printf("Producer connected from %s:%d\n", inet_ntoa(broker_addr_producer.sin_addr), ntohs(broker_addr_producer.sin_port));

        pthread_t producer_thread_id;
        int *producer_sock_ptr = (int *)malloc(sizeof(int));
        *producer_sock_ptr = producer_sock;
        data->broker_sock_producer = *producer_sock_ptr;

        if (pthread_create(&producer_thread_id, NULL, handle_producer, (void *)data) != 0)
        {
            printf("Error creating producer thread\n");
            return NULL;
        }
        pthread_detach(producer_thread_id);
    }

    return NULL;
}

void *handle_consumer_connections(void *arg)
{
    int broker_sock_consumer = *(int *)arg;

    struct sockaddr_in broker_addr_consumer;
    int c = sizeof(struct sockaddr_in);

    while (1)
    {

        int consumer_sock = accept(broker_sock_consumer, (struct sockaddr *)&broker_addr_consumer, (socklen_t *)&c);
        if (consumer_sock < 0)
        {
            printf("Error accepting producer connection\n");
            continue;
        }

        printf("Consumer connected from %s:%d\n", inet_ntoa(broker_addr_consumer.sin_addr), ntohs(broker_addr_consumer.sin_port));

        pthread_t consumer_thread_id;
        int *consumer_sock_ptr = (int *)malloc(sizeof(int));
        *consumer_sock_ptr = consumer_sock;

        if (pthread_create(&consumer_thread_id, NULL, handle_consumer, (void *)consumer_sock_ptr) != 0)
        {
            printf("Error creating productor thread\n");
            return NULL;
        }
        pthread_detach(consumer_thread_id);
    }

    return NULL;
}

int generateOption(char *specific_request)
{
    int option = -1;
    if (strcmp(specific_request, "cpu") == 0)
    {
        option = 2;
    }
    else if (strcmp(specific_request, "memory") == 0)
    {
        option = 1;
    }
    else
    {
        option = 0;
    }
    return option;
}

void writeOnPersistentFile(const char *string)
{
    FILE *file = fopen("persistence.txt", "a");
    if (file == NULL)
    {
        perror("Error opening the file");
        return;
    }
    fprintf(file, "%s\n", string);
    fclose(file);
}
void send_message_to_consumer(int consumer_socket, int option)
{
    if (option == 1 || option == 0)
    {
        char *dequeuedDataMemoryPartition1 = dequeue(mp_queue_producer, 0);
        if (dequeuedDataMemoryPartition1 != NULL)
        {
            printf("Element dequeued from section \"memory\" partition #%d: %s\n", 1, dequeuedDataMemoryPartition1);
            if (send(consumer_socket, dequeuedDataMemoryPartition1, sizeof(dequeuedDataMemoryPartition1), 0) == -1)
                perror("Error sending the message");
            if (persistence == 1)
                writeOnPersistentFile(dequeuedDataMemoryPartition1);
            free(dequeuedDataMemoryPartition1);
        }
        char *dequeuedDataMemoryPartition2 = dequeue(mp_queue_producer, 1);
        if (dequeuedDataMemoryPartition2 != NULL)
        {
            printf("Element dequeued from section \"memory\" partition #%d: %s\n", 2, dequeuedDataMemoryPartition2);
            if (send(consumer_socket, dequeuedDataMemoryPartition2, sizeof(dequeuedDataMemoryPartition2), 0) == -1)
                perror("Error sending the message");
            if (persistence == 1)
                writeOnPersistentFile(dequeuedDataMemoryPartition2);
            free(dequeuedDataMemoryPartition2);
        }
    }
    if (option == 2 || option == 0)
    {
        // Desencolamos la metrica cpu y la enviamos
        char *dequeuedDataCpuPartition1 = dequeue(mp_queue_producer, 2);
        if (dequeuedDataCpuPartition1 != NULL)
        {
            printf("Element dequeued from section \"CPU\" partition #%d: %s\n", 1, dequeuedDataCpuPartition1);
            if (send(consumer_socket, dequeuedDataCpuPartition1, sizeof(dequeuedDataCpuPartition1), 0) == -1)
                perror("Error sending the message");
            if (persistence == 1)
                writeOnPersistentFile(dequeuedDataCpuPartition1);
            free(dequeuedDataCpuPartition1);
        }
        char *dequeuedDataCpuPartition2 = dequeue(mp_queue_producer, 3);
        if (dequeuedDataCpuPartition2 != NULL)
        {
            printf("Element dequeued from section \"CPU\" partition #%d: %s\n", 2, dequeuedDataCpuPartition2);
            if (send(consumer_socket, dequeuedDataCpuPartition2, sizeof(dequeuedDataCpuPartition2), 0) == -1)
                perror("Error sending the message");
            if (persistence == 1)
                writeOnPersistentFile(dequeuedDataCpuPartition2);
            free(dequeuedDataCpuPartition2);
        }
    }
}

void splitAndEnqueue(char *string)
{
    char *token = strtok(string, "/");
    int i = 0;
    char *sectionName;
    int partitionNumber;
    char *data;
    while (token != NULL)
    {

        if (i == 1)
        {
            sectionName = token;
        }
        if (i == 2)
        {
            data = token;
        }
        if (i == 3)
        {
            partitionNumber = atoi(token);
        }
        token = strtok(NULL, "|");
        i++;
    }

    printf("\n");
    char *formatted_data;
    formatted_data = (char *)malloc(20);
    strcpy(formatted_data, data);

    char *formatted_section_name;
    formatted_section_name = (char *)malloc(20);
    strcpy(formatted_section_name, sectionName);
    if (strcmp(formatted_section_name, "memory") == 0)
    {
        enqueue(mp_queue_producer, "memory", partitionNumber - 1, formatted_data);
        printPartitionContents(mp_queue_producer, "memory", partitionNumber - 1);
    }
    else if (strcmp(formatted_section_name, "cpu") == 0)
    {
        enqueue(mp_queue_producer, "cpu", partitionNumber + 1, formatted_data);
        printPartitionContents(mp_queue_producer, "cpu", partitionNumber + 1);
    }
}
