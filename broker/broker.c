#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <semaphore.h>

#define MAX_MESSAGE_LENGTH 256

pthread_mutex_t mutex;
void *handle_gateway(void *arg)
{
    int gateway_sock = *(int *)arg;
    char message[MAX_MESSAGE_LENGTH];
    int read_size;

    while ((read_size = recv(gateway_sock, message, sizeof(message), 0)) > 0)
    {
        message[read_size] = '\0';

        pthread_mutex_lock(&mutex);
        printf("%s\n", message);
        pthread_mutex_unlock(&mutex);
    }

    // Si el hilo sale del bucle, es porque la conexión con el gateway se ha cerrado
    printf("El productor se ha desconectado\n");
    close(gateway_sock);
    free(arg);
    pthread_exit(NULL);
}

// Función para manejar las conexiones del gateway
void *handle_gateway_connections(void *arg)
{
    int broker_sock_gateway = *(int *)arg;
    struct sockaddr_in broker_addr_gateway;
    int c = sizeof(struct sockaddr_in);

    while (1)
    {
        int gateway_sock = accept(broker_sock_gateway, (struct sockaddr *)&broker_addr_gateway, (socklen_t *)&c);
        if (gateway_sock < 0)
        {
            printf("Error al aceptar la conexión del gateway\n");
            continue;
        }

        // Mostrar información de la conexión del Productor
        printf("Productor conectado desde %s:%d\n", inet_ntoa(broker_addr_gateway.sin_addr), ntohs(broker_addr_gateway.sin_port));

        // Crear hilo para manejar al productor
        pthread_t gateway_thread_id;
        int *gateway_sock_ptr = (int *)malloc(sizeof(int));
        *gateway_sock_ptr = gateway_sock;
        if (pthread_create(&gateway_thread_id, NULL, handle_gateway, (void *)gateway_sock_ptr) != 0)
        {
            printf("Error al crear el hilo para el Gateway\n");
            return NULL;
        }
        // Liberar el hilo principal para aceptar más conexiones del gateway
        pthread_detach(gateway_thread_id);
    }

    return NULL;
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Uso: %s <puerto_gateway> <puerto_subscribers>\n", argv[0]);
        return 1;
    }

    int broker_port_gateway = atoi(argv[1]);
    int broker_port_subscribers = atoi(argv[2]);
    if (broker_port_gateway <= 0 || broker_port_subscribers <= 0)
    {
        printf("Error: Puertos inválidos\n");
        return 1;
    }

    int broker_sock_gateway, broker_sock_subscribers;
    struct sockaddr_in broker_addr_gateway, broker_addr_subscribers;

    // Configurar socket para el gateway
    broker_sock_gateway = socket(AF_INET, SOCK_STREAM, 0);
    if (broker_sock_gateway == -1)
    {
        printf("No se pudo crear el socket para el gateway\n");
        return 1;
    }

    broker_addr_gateway.sin_family = AF_INET;
    broker_addr_gateway.sin_addr.s_addr = INADDR_ANY;
    broker_addr_gateway.sin_port = htons(broker_port_gateway);

    if (bind(broker_sock_gateway, (struct sockaddr *)&broker_addr_gateway, sizeof(broker_addr_gateway)) < 0)
    {
        printf("Error en la operación de bind para el gateway, asegúrese de utilizar un puerto válido\n");
        return 1;
    }

    listen(broker_sock_gateway, 10);
    printf("Broker inicializado para el gateway en el puerto %d\n", broker_port_gateway);

    // Inicializar el mutex
    if (pthread_mutex_init(&mutex, NULL) != 0)
    {
        printf("Error al inicializar el mutex\n");
        return 1;
    }

    // Crear hilo para manejar las conexiones del gateway
    pthread_t gateway_connections_thread_id;
    if (pthread_create(&gateway_connections_thread_id, NULL, handle_gateway_connections, (void *)&broker_sock_gateway) != 0)
    {
        printf("Error al crear el hilo para manejar las conexiones del gateway\n");
        return 1;
    }

    // Mantener el hilo principal en ejecución
    while (1)
    {
        sleep(1);
    }

    // Destruir el mutex antes de salir
    pthread_mutex_destroy(&mutex);

    close(broker_sock_gateway);
    close(broker_sock_subscribers);
    return 0;
}