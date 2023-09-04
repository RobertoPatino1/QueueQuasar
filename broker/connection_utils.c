#include "connection_utils.h"
void *handle_productor(void *arg)
{
    struct ThreadContent *data = (struct ThreadContent *)arg;
    int productor_sock = (int)data->broker_sock_productor;
    printf("%d\n", data->broker_sock_productor);
    MultiPartitionQueue *mp_queue = data->mp_queue;

    char message[MAX_MESSAGE_LENGTH];
    int read_size;
    while ((read_size = recv(productor_sock, message, sizeof(message), 0)) > 0)
    {

        message[read_size] = '\0';
        printf("Mensaje recibido del productor: %s\n", message);
        pthread_mutex_lock(&mutex);

        splitAndEnqueue(message, mp_queue);

        sleep(2);
        pthread_mutex_unlock(&mutex);
    }

    printf("El productor se ha desconectado\n");
    close(productor_sock);
    // free(arg);
    pthread_exit(NULL);
}

void *handle_productor_connections(void *arg)
{
    struct ThreadContent *data = (struct ThreadContent *)arg;
    int broker_sock_productor = (int)data->broker_sock_productor;

    struct sockaddr_in broker_addr_productor;
    int c = sizeof(struct sockaddr_in);

    while (1)
    {
        int productor_sock = accept(broker_sock_productor, (struct sockaddr *)&broker_addr_productor, (socklen_t *)&c);
        if (productor_sock < 0)
        {
            printf("Error accepting productor connection\n");
            continue;
        }

        // Mostrar información de la conexión del Productor
        printf("Productor connected from %s:%d\n", inet_ntoa(broker_addr_productor.sin_addr), ntohs(broker_addr_productor.sin_port));

        // Crear hilo para manejar al productor
        pthread_t productor_thread_id;
        int *productor_sock_ptr = (int *)malloc(sizeof(int));
        *productor_sock_ptr = productor_sock;
        data->broker_sock_productor = *productor_sock_ptr;

        if (pthread_create(&productor_thread_id, NULL, handle_productor, (void *)data) != 0)
        {
            printf("Error creating productor thread\n");
            return NULL;
        }
        pthread_detach(productor_thread_id);
    }

    return NULL;
}

void splitAndEnqueue(char *cadena, MultiPartitionQueue *mp_queue)
{

    char *token = strtok(cadena, "/");
    int i = 0;
    char *sectionName;
    int partitionNumber;
    char *data;
    while (token != NULL)
    {
        if (i == 0)
        {
            // Se extrae el nombre del nodo
            // printf("Nombre del nodo: %s\n", token);
        }

        if (i == 1)
        {
            // Se extrae el nombre de la metrica
            sectionName = token;
        }
        if (i == 2)
        {
            // Se extrae el valor de la metrica
            data = token;
        }
        if (i == 3)
        {
            // Se extrae el numero de la particion
            partitionNumber = atoi(token);
        }
        token = strtok(NULL, "|");
        i++;
    }

    printf("\n");
    char *data2;
    data2 = (char *)malloc(20);
    strcpy(data2, data);

    char *sectionName2;
    sectionName2 = (char *)malloc(20);
    strcpy(sectionName2, sectionName);
    if (strcmp(sectionName2, "memoria") == 0)
    {
        enqueue(mp_queue_productor, "memoria", partitionNumber - 1, data2);
        printPartitionContents(mp_queue_productor, "memoria", partitionNumber - 1);

        // Ahora, vamos a desencolar un elemento de "memoria" en la misma partición
        char *dequeuedData = dequeue(mp_queue_productor, "memoria", partitionNumber - 1);
        if (dequeuedData != NULL)
        {
            printf("Elemento desencolado de \"memoria\" en la partición %d: %s\n", partitionNumber - 1, dequeuedData);
            free(dequeuedData); // Asegúrate de liberar la memoria del elemento desencolado.
        }
        else
        {
            printf("No hay elementos en la cola \"memoria\" en la partición %d\n", partitionNumber - 1);
        }
    }
    else if (strcmp(sectionName2, "cpu") == 0)
    {
        enqueue(mp_queue_productor, "cpu", partitionNumber + 1, data2);
        printPartitionContents(mp_queue_productor, "cpu", partitionNumber + 1);

        // Ahora, vamos a desencolar un elemento de "cpu" en la misma partición
        char *dequeuedData = dequeue(mp_queue_productor, "cpu", partitionNumber + 1);
        if (dequeuedData != NULL)
        {
            printf("Elemento desencolado de \"cpu\" en la partición %d: %s\n", partitionNumber + 1, dequeuedData);
            free(dequeuedData); // Asegúrate de liberar la memoria del elemento desencolado.
        }
        else
        {
            printf("No hay elementos en la cola \"cpu\" en la partición %d\n", partitionNumber + 1);
        }
    }
}
