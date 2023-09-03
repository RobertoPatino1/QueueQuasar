#include "connection_utils.h"

void *handle_productor(void *arg)
{
    int productor_sock = *(int *)arg;
    char message[MAX_MESSAGE_LENGTH];
    int read_size;

    while ((read_size = recv(productor_sock, message, sizeof(message), 0)) > 0)
    {
        message[read_size] = '\0';

        pthread_mutex_lock(&mutex);
        // printf("%s\n\n", message);
        // char *nombreNodo = strtok(message, "/");
        // printf(" %s\n", nombreNodo); // Primera ocurrencia es el nombre del nodo

        // char *categoria = strtok(message, "/");
        // printf(" %s\n", categoria);

        splitAndEnqueue(message, "/");
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

void splitAndEnqueue(char *cadena, char *delimiter1)
{
    char *token = strtok(cadena, delimiter1);
    int i = 0;
    while (token != NULL)
    {
        if (i == 0)
        {
            // Se extrae el nombre del nodo
            printf("Nombre del nodo: %s\n", token);
        }

        if (i == 1)
        {
            // Se extrae el nombre de la metrica
            printf("Nombre de la metrica: %s\n", token);
        }
        else if (i == 2)
        {
            // Se extrae el valor de la metrica
            printf("Valor de la metrica: %s\n", token);
        }
        else if (i == 3)
        {
            // Se extrae el numero de la particion
            printf("Numero de la particion: %s\n", token);
        }
        i++;
        token = strtok(NULL, "|");
    }
    printf("\n");

    return 0;
}