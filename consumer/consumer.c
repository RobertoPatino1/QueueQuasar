#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        fprintf(stderr, "Usage: %s <broker_ip> <broker_port> <request>\n", argv[0]);
        return 1;
    }

    const char *broker_ip = argv[1];
    int broker_port = atoi(argv[2]);
    const char *request = argv[3];

    int socket_client = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_client == -1)
    {
        perror("Error during socket creation");
        return 1;
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(broker_port);
    if (inet_pton(AF_INET, broker_ip, &server_addr.sin_addr) <= 0)
    {
        perror("Error during IP address convertion");
        close(socket_client);
        return 1;
    }

    if (connect(socket_client, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        perror("Error during server connection");
        close(socket_client);
        return 1;
    }

    if (send(socket_client, request, strlen(request), 0) == -1)
    {
        perror("Error while sending the message");
        close(socket_client);
        return 1;
    }
    printf("The request \"%s\" was successfully sent to the broker\n", request);

    char response[256];
    int recieved_bytes = recv(socket_client, response, sizeof(response), 0);
    if (recieved_bytes == -1)
    {
        perror("Error al recibir la respuesta");
        close(socket_client);
        return 1;
    }

    response[recieved_bytes] = '\0';

    while (1)
    {
        char recieved_message[256];
        int recieved_bytes = recv(socket_client, recieved_message, sizeof(recieved_message), 0);
        if (recieved_bytes <= 0)
        {
            perror("Broker connection closed");
            break;
        }
        recieved_message[recieved_bytes] = '\0';
        printf("\nResponse from the broker: %s\n\n", recieved_message);
    }

    close(socket_client);

    return 0;
}
