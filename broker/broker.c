#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <semaphore.h>

#define MAX_MESSAGE_LENGTH 256

pthread_mutex_t mutex;
void *handle_productor(void *arg)
{
    int productor_sock = *(int *)arg;
    char message[MAX_MESSAGE_LENGTH];
    int read_size;

    while ((read_size = recv(productor_sock, message, sizeof(message), 0)) > 0)
    {
        message[read_size] = '\0';

        pthread_mutex_lock(&mutex);
        printf("%s\n", message);
        pthread_mutex_unlock(&mutex);
    }

    printf("El productor se ha desconectado\n");
    close(productor_sock);
    free(arg);
    pthread_exit(NULL);
}

void *handle_productor_connections(void *arg)
{
    int broker_sock_productor = *(int *)arg;
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
        if (pthread_create(&productor_thread_id, NULL, handle_productor, (void *)productor_sock_ptr) != 0)
        {
            printf("Error creating productor thread\n");
            return NULL;
        }
        pthread_detach(productor_thread_id);
    }

    return NULL;
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Uso: %s <puerto_productor> <puerto_consumidor>\n", argv[0]);
        return 1;
    }

    int broker_port_productor = atoi(argv[1]);
    int broker_port_subscribers = atoi(argv[2]);
    if (broker_port_productor <= 0 || broker_port_subscribers <= 0)
    {
        printf("Error: Puertos inválidos\n");
        return 1;
    }

    int broker_sock_productor, broker_sock_subscribers;
    struct sockaddr_in broker_addr_productor, broker_addr_subscribers;

    broker_sock_productor = socket(AF_INET, SOCK_STREAM, 0);
    if (broker_sock_productor == -1)
    {
        printf("Error: Unable to create socket\n");
        return 1;
    }

    broker_addr_productor.sin_family = AF_INET;
    broker_addr_productor.sin_addr.s_addr = INADDR_ANY;
    broker_addr_productor.sin_port = htons(broker_port_productor);

    if (bind(broker_sock_productor, (struct sockaddr *)&broker_addr_productor, sizeof(broker_addr_productor)) < 0)
    {
        printf("Error: Try to use a valid port\n");
        return 1;
    }

    listen(broker_sock_productor, 10);
    printf("Successfully initialized broker using the port: %d\n", broker_port_productor);

    if (pthread_mutex_init(&mutex, NULL) != 0)
    {
        printf("Error al inicializar el mutex\n");
        return 1;
    }

    pthread_t productor_connections_thread_id;
    if (pthread_create(&productor_connections_thread_id, NULL, handle_productor_connections, (void *)&broker_sock_productor) != 0)
    {
        printf("Error creating connections thread\n");
        return 1;
    }

    while (1)
    {
        sleep(1);
    }

    pthread_mutex_destroy(&mutex);

    close(broker_sock_productor);
    close(broker_sock_subscribers);
    return 0;
}