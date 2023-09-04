#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        fprintf(stderr, "Uso: %s <ip_broker> <puerto_broker> <solicitud>\n", argv[0]);
        return 1;
    }

    const char *ip_broker = argv[1];
    int puerto_broker = atoi(argv[2]);
    const char *solicitud = argv[3];

    int cliente_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (cliente_socket == -1)
    {
        perror("Error al crear el socket");
        return 1;
    }

    struct sockaddr_in servidor_addr;
    servidor_addr.sin_family = AF_INET;
    servidor_addr.sin_port = htons(puerto_broker);
    if (inet_pton(AF_INET, ip_broker, &servidor_addr.sin_addr) <= 0)
    {
        perror("Error al convertir la dirección IP");
        close(cliente_socket);
        return 1;
    }

    // Conectar al servidor
    if (connect(cliente_socket, (struct sockaddr *)&servidor_addr, sizeof(servidor_addr)) == -1)
    {
        perror("Error al conectar al servidor");
        close(cliente_socket);
        return 1;
    }

    // Enviando la solicitud al broker
    if (send(cliente_socket, solicitud, strlen(solicitud), 0) == -1)
    {
        perror("Error al enviar el mensaje");
        close(cliente_socket);
        return 1;
    }
    printf("La solicitud \"%s\" fue enviada al broker de manera exitosa\n", solicitud);

    // Recibir la respuesta del broker
    char respuesta[256];
    int bytes_recibidos = recv(cliente_socket, respuesta, sizeof(respuesta), 0);
    if (bytes_recibidos == -1)
    {
        perror("Error al recibir la respuesta");
        close(cliente_socket);
        return 1;
    }

    respuesta[bytes_recibidos] = '\0';

    // Recibir mensajes constantemente desde el broker
    while (1)
    {
        char mensaje_recibido[256];
        int bytes_recibidos = recv(cliente_socket, mensaje_recibido, sizeof(mensaje_recibido), 0);
        if (bytes_recibidos <= 0)
        {
            perror("Se ha cerrado la conexion del broker");
            break; // Salir del bucle si se cierra la conexion
        }
        mensaje_recibido[bytes_recibidos] = '\0';
        printf("Respuesta recibida del broker: %s\n", mensaje_recibido);
    }

    // Cerrar el socket y finalizar
    close(cliente_socket);

    return 0;
}
