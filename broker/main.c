// #include "multiPartitionQueue.h"

// int main()
// {
//     int numPartitions = 3;
//     MultiPartitionQueue *queue = createMultiPartitionQueue(numPartitions);

//     enqueue(queue, "seccion_memoria", 0, "Dato 1");
//     enqueue(queue, "seccion_memoria", 0, "Dato 2");
//     enqueue(queue, "seccion_memoria", 1, "Dato 3");
//     enqueue(queue, "seccion_cpu", 0, "Dato 4");

//     printPartitionContents(queue, "seccion_memoria", 0);
//     printPartitionContents(queue, "seccion_memoria", 1);
//     printPartitionContents(queue, "seccion_cpu", 0);

//     enqueue(queue, "seccion_cpu", 0, "Dato 5");

//     freeMultiPartitionQueue(queue);
//     return 0;
// }
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "connection_utils.h"
#define MAX_MESSAGE_LENGTH 256

void *handle_producer(void *arg)
{
    int producer_socket = *(int *)arg;
    char buffer[MAX_MESSAGE_LENGTH];

    while (1)
    {
        int read_size = recv(producer_socket, buffer, sizeof(buffer), 0);
        if (read_size <= 0)
        {
            break; // Si la conexión se cerró o hubo un error, salimos del bucle
        }
        buffer[read_size] = '\0'; // Añadir el carácter de terminación de cadena
        printf("Mensaje del productor: %s\n", buffer);
    }

    close(producer_socket);
    free(arg);
    pthread_exit(NULL);
}

/*
TODO: TERMINAR LA IMPLEMENTACION DE ESTO
*/
void *handle_consumer(void *arg)
{
    int consumer_socket = *(int *)arg;
    // Lógica para manejar la conexión con el consumidor
    // ...
    close(consumer_socket);
    free(arg);
    pthread_exit(NULL);
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define MAX_PRODUCERS 10

// ... Definiciones y funciones previas ...

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Uso: %s <puerto_productores> <puerto_consumidores>\n", argv[0]);
        return 1;
    }

    int producer_port = atoi(argv[1]);
    int consumer_port = atoi(argv[2]);

    // Configurar socket para productores
    int producer_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (producer_socket == -1)
    {
        printf("No se pudo crear el socket para productores\n");
        return 1;
    }

    struct sockaddr_in producer_addr;
    producer_addr.sin_family = AF_INET;
    producer_addr.sin_addr.s_addr = INADDR_ANY;
    producer_addr.sin_port = htons(producer_port);

    if (bind(producer_socket, (struct sockaddr *)&producer_addr, sizeof(producer_addr)) < 0)
    {
        printf("Error en la operación de bind para productores\n");
        return 1;
    }

    // Configurar socket para consumidores
    int consumer_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (consumer_socket == -1)
    {
        printf("No se pudo crear el socket para consumidores\n");
        return 1;
    }

    struct sockaddr_in consumer_addr;
    consumer_addr.sin_family = AF_INET;
    consumer_addr.sin_addr.s_addr = INADDR_ANY;
    consumer_addr.sin_port = htons(consumer_port);

    if (bind(consumer_socket, (struct sockaddr *)&consumer_addr, sizeof(consumer_addr)) < 0)
    {
        printf("Error en la operación de bind para consumidores\n");
        return 1;
    }

    // Resto del código ...

    // Escuchar conexiones de productores y consumidores
    listen(producer_socket, MAX_PRODUCERS);
    listen(consumer_socket, 10);

    // Aceptar conexiones de productores y manejarlas secuencialmente
    while (1)
    {
        struct sockaddr_in producer_addr;
        int producer_addr_len = sizeof(producer_addr);
        int producer_connection = accept(producer_socket, (struct sockaddr *)&producer_addr, &producer_addr_len);

        if (producer_connection < 0)
        {
            printf("Error al aceptar la conexión del productor\n");
            continue;
        }

        handle_producer(producer_connection);

        // Cerrar la conexión con el productor después de manejarla
        close(producer_connection);
    }
    // Destruir el mutex, cerrar sockets y liberar recursos ...

    return 0;
}
