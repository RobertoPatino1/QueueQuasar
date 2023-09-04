
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "connection_utils.h"
pthread_mutex_t mutex2;
int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Uso: %s <puerto_broker> <puerto_consumidor>\n", argv[0]);
        return 1;
    }

    int broker_port_productor = atoi(argv[1]);
    int broker_port_consumidor = atoi(argv[2]);
    if (broker_port_productor <= 0 || broker_port_consumidor <= 0)
    {
        printf("Error: Puertos inválidos\n");
        return 1;
    }

    int broker_sock_productor, broker_sock_consumidor;
    struct sockaddr_in broker_addr_productor, broker_addr_consumidor;

    broker_sock_productor = socket(AF_INET, SOCK_STREAM, 0);
    if (broker_sock_productor == -1)
    {
        printf("Error: No se pudo crear el socket para el productor\n");
        return 1;
    }
    broker_sock_consumidor = socket(AF_INET, SOCK_STREAM, 0);
    if (broker_sock_consumidor == -1)
    {
        printf("Error: No se pudo crear el socket para el consumidor\n");
        return 1;
    }

    broker_addr_productor.sin_family = AF_INET;
    broker_addr_productor.sin_addr.s_addr = INADDR_ANY;
    broker_addr_productor.sin_port = htons(broker_port_productor);

    broker_addr_consumidor.sin_family = AF_INET;
    broker_addr_consumidor.sin_addr.s_addr = INADDR_ANY;
    broker_addr_consumidor.sin_port = htons(broker_port_consumidor);

    if (bind(broker_sock_productor, (struct sockaddr *)&broker_addr_productor, sizeof(broker_addr_productor)) < 0)
    {
        printf("Error: Try to use a valid port\n");
        return 1;
    }

    if (bind(broker_sock_consumidor, (struct sockaddr *)&broker_addr_consumidor, sizeof(broker_addr_consumidor)) < 0)
    {
        printf("Error: Try to use a valid port\n");
        return 1;
    }

    listen(broker_sock_productor, 10);
    listen(broker_sock_consumidor, 10);
    printf("Successfully initialized broker using the port: %d\n", broker_port_productor);
    printf("Successfully initialized broker using the port: %d\n", broker_port_consumidor);

    if (pthread_mutex_init(&mutex2, NULL) != 0)
    {
        printf("Error al inicializar el mutex\n");
        return 1;
    }

    pthread_t productor_connections_thread_id;

    // Creando la cola multi-particiones
    MultiPartitionQueue *mp_queue = createMultiPartitionQueue(4);
    mp_queue_productor = createMultiPartitionQueue(4);
    struct ThreadContent data;
    data.broker_sock_productor = broker_sock_productor;
    data.mp_queue = mp_queue;

    if (pthread_create(&productor_connections_thread_id, NULL, handle_productor_connections, (void *)&data) != 0)
    {
        printf("Error creating connections thread\n");
        return 1;
    }

    while (1)
    {
        sleep(1);
    }

    pthread_mutex_destroy(&mutex2);

    close(broker_sock_productor);
    close(broker_sock_consumidor);
    return 0;
}