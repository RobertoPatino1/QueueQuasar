#include "connection_utils.h"
int persistencia;
MultiPartitionQueue *mp_queue_productor;

void *handle_productor(void *arg)
{
    struct ThreadContent *data = (struct ThreadContent *)arg;
    int productor_sock = (int)data->broker_sock_productor;

    char message[MAX_MESSAGE_LENGTH];
    int read_size;

    while ((read_size = recv(productor_sock, message, sizeof(message), 0)) > 0)
    {
        message[read_size] = '\0';
        printf("\n---Mensaje recibido del productor: %s---\n", message);
        splitAndEnqueue(message);
        sleep(2);
    }

    printf("El productor se ha desconectado\n");
    close(productor_sock);
    pthread_exit(NULL);
}

void *handle_consumidor(void *arg)
{
    int consumidor_sock = *(int *)arg;
    char solicitud[MAX_MESSAGE_LENGTH];
    int opcion;
    int read_size;
    read_size = recv(consumidor_sock, solicitud, sizeof(solicitud), 0);
    if (read_size == -1)
    {
        perror("Se ha cerrado la conexion");
        close(consumidor_sock);
    }
    solicitud[read_size] = '\0';

    printf("\nSolicitud recibida del consumidor: %s\n", solicitud);

    char *solicitud_especifica = strtok(solicitud, "/");
    solicitud_especifica = strtok(NULL, "-");

    persistencia = atoi(strtok(NULL, "-"));
    printf("\nUsar persistencia? %d\n", persistencia); // Si es 1, escribimos en un archivo!!!
    opcion = generateOption(solicitud_especifica);
    while (1)
    {
        send_message_to_consumidor(consumidor_sock, opcion);
        sleep(5); // Esperar antes de cada envio
    }
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

void *handle_consumidor_connections(void *arg)
{
    int broker_sock_consumidor = *(int *)arg;

    struct sockaddr_in broker_addr_consumidor;
    int c = sizeof(struct sockaddr_in);

    while (1)
    {

        int consumidor_sock = accept(broker_sock_consumidor, (struct sockaddr *)&broker_addr_consumidor, (socklen_t *)&c);
        if (consumidor_sock < 0)
        {
            printf("Error accepting productor connection\n");
            continue;
        }

        // Mostrar información de la conexión del Productor
        printf("Consumidor conectado desde %s:%d\n", inet_ntoa(broker_addr_consumidor.sin_addr), ntohs(broker_addr_consumidor.sin_port));

        pthread_t consumidor_thread_id;
        int *consumidor_sock_ptr = (int *)malloc(sizeof(int));
        *consumidor_sock_ptr = consumidor_sock;

        if (pthread_create(&consumidor_thread_id, NULL, handle_consumidor, (void *)consumidor_sock_ptr) != 0)
        {
            printf("Error creating productor thread\n");
            return NULL;
        }
        // pthread_detach(consumidor_thread_id);
    }

    // return NULL;
}

int generateOption(char *solicitud_especifica)
{
    int opcion = -1;
    if (strcmp(solicitud_especifica, "cpu") == 0)
    {
        opcion = 2; // Enviamos las 2 metricas
    }
    else if (strcmp(solicitud_especifica, "memoria") == 0)
    {
        opcion = 1; // Enviamos solo la metrica de memoria
    }
    else
    {
        opcion = 0; // Enviamos la metrica de CPU
    }
    return opcion;
}

void escribirEnArchivoPersistente(const char *cadena)
{
    FILE *archivo = fopen("persistence.txt", "a");
    if (archivo == NULL)
    {
        perror("Error al abrir el archivo");
        return;
    }
    fprintf(archivo, "%s\n", cadena);
    fclose(archivo);
}
void send_message_to_consumidor(int consumidor_socket, int opcion)
{
    if (opcion == 1 || opcion == 0)
    {
        // Desencolamos solo la metrica memoria y la enviamos
        char *dequeuedDataMemoryPartition1 = dequeue(mp_queue_productor, 0);
        if (dequeuedDataMemoryPartition1 != NULL)
        {
            printf("Elemento desencolado de \"memoria\" en la partición %d: %s\n", 1, dequeuedDataMemoryPartition1);
            if (send(consumidor_socket, dequeuedDataMemoryPartition1, sizeof(dequeuedDataMemoryPartition1), 0) == -1)
                perror("Error al enviar el mensaje");
            if (persistencia == 1)
                escribirEnArchivoPersistente(dequeuedDataMemoryPartition1);
            free(dequeuedDataMemoryPartition1); // Asegúrate de liberar la memoria del elemento desencolado.
        }
        char *dequeuedDataMemoryPartition2 = dequeue(mp_queue_productor, 1);
        if (dequeuedDataMemoryPartition2 != NULL)
        {
            printf("Elemento desencolado de \"memoria\" en la partición %d: %s\n", 2, dequeuedDataMemoryPartition2);
            if (send(consumidor_socket, dequeuedDataMemoryPartition2, sizeof(dequeuedDataMemoryPartition2), 0) == -1)
                perror("Error al enviar el mensaje");
            if (persistencia == 1)
                escribirEnArchivoPersistente(dequeuedDataMemoryPartition2);
            free(dequeuedDataMemoryPartition2); // Asegúrate de liberar la memoria del elemento desencolado.
        }
    }
    if (opcion == 2 || opcion == 0)
    {
        // Desencolamos la metrica cpu y la enviamos
        char *dequeuedDataCpuPartition1 = dequeue(mp_queue_productor, 2);
        if (dequeuedDataCpuPartition1 != NULL)
        {
            printf("Elemento desencolado de \"CPU\" en la partición %d: %s\n", 1, dequeuedDataCpuPartition1);
            if (send(consumidor_socket, dequeuedDataCpuPartition1, sizeof(dequeuedDataCpuPartition1), 0) == -1)
                perror("Error al enviar el mensaje");
            if (persistencia == 1)
                escribirEnArchivoPersistente(dequeuedDataCpuPartition1);
            free(dequeuedDataCpuPartition1); // Asegúrate de liberar la memoria del elemento desencolado.
        }
        char *dequeuedDataCpuPartition2 = dequeue(mp_queue_productor, 3);
        if (dequeuedDataCpuPartition2 != NULL)
        {
            printf("Elemento desencolado de \"CPU\" en la partición %d: %s\n", 2, dequeuedDataCpuPartition2);
            if (send(consumidor_socket, dequeuedDataCpuPartition2, sizeof(dequeuedDataCpuPartition2), 0) == -1)
                perror("Error al enviar el mensaje");
            if (persistencia == 1)
                escribirEnArchivoPersistente(dequeuedDataCpuPartition2);
            free(dequeuedDataCpuPartition2); // Asegúrate de liberar la memoria del elemento desencolado.
        }
    }
}

void splitAndEnqueue(char *cadena)
{
    char *token = strtok(cadena, "/");
    int i = 0;
    char *sectionName;
    int partitionNumber;
    char *data;
    while (token != NULL)
    {

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
    char *formatted_data;
    formatted_data = (char *)malloc(20);
    strcpy(formatted_data, data);

    char *formatted_section_name;
    formatted_section_name = (char *)malloc(20);
    strcpy(formatted_section_name, sectionName);
    if (strcmp(formatted_section_name, "memoria") == 0)
    {
        enqueue(mp_queue_productor, "memoria", partitionNumber - 1, formatted_data);
        printPartitionContents(mp_queue_productor, "memoria", partitionNumber - 1);
    }
    else if (strcmp(formatted_section_name, "cpu") == 0)
    {
        enqueue(mp_queue_productor, "cpu", partitionNumber + 1, formatted_data);
        printPartitionContents(mp_queue_productor, "cpu", partitionNumber + 1);
    }
}
