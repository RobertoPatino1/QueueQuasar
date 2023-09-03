#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "productor_utils.h"
#include "connection_utils.h"

char *nombreTopico1 = "memoria";
char *nombreTopico2 = "cpu";

int main(int argc, char *argv[]) //./productor <nombre_nodo> <puerto_productor> <ip_broker> <puerto_broker> [id_particion_topico_1] [id_particion_topico_2]
{
    BrokerConnection brokerConnection;

    if (argc < 5)
    {
        printf("Usage: %s <nombre_nodo> <puerto_productor> <ip_broker> <puerto_broker> [<id_particion_topico_1>] [<id_particion_topico_2>]\n", argv[0]);
        return 1;
    }

    char *nombreNodo = argv[1];
    char *puerto_productor = argv[2];
    char *ip_broker = argv[3];
    char *puerto_broker = argv[4];

    char *idParticionTopico1Str = NULL;
    char *idParticionTopico2Str = NULL;
    int useRoundRobinMessage1 = 0;
    int useRoundRobinMessage2 = 0;
    if (argc > 6)
    {
        idParticionTopico1Str = argv[5];
        idParticionTopico2Str = argv[6];
    }
    // Handle single partition ID
    else if (argc == 6)
    {
        idParticionTopico1Str = argv[5];
        idParticionTopico2Str = "2";
        useRoundRobinMessage2 = 1;
        printf("No partition ID was provided for topic 2, Round Robin will be used...\n\n");
    }

    else
    {
        idParticionTopico1Str = "2"; // Colocamos 2 para iniciar por defecto en la particion 1
        idParticionTopico2Str = "2";
        useRoundRobinMessage1 = 1;
        useRoundRobinMessage2 = 1;
        printf("No partition ID was provided for topic 1, Round Robin will be used...\n");
        printf("No partition ID was provided for topic 2, Round Robin will be used...\n\n");
    }
    if (establishBrokerConnection(&brokerConnection, ip_broker, atoi(puerto_broker)) == -1)
    {
        return 1;
    }
    char *memoryUsage = NULL;
    char *cpuUsage = NULL;
    TopicState topicState1 = {.idParticion = idParticionTopico1Str};
    TopicState topicState2 = {.idParticion = idParticionTopico2Str};

    printf("Initiating transmission of messages to the broker: \n");
    printf("DIRECCION_IP_PRODUCTOR:%s ---> %s:%s\n\n", puerto_productor, ip_broker, puerto_broker);
    while (1)
    {
        getMemoryUsage(&memoryUsage);
        getCPUUsage(&cpuUsage);

        char *mensajeTopico1 = buildMessage(nombreNodo, nombreTopico1, memoryUsage, &topicState1, useRoundRobinMessage1);
        char *mensajeTopico2 = buildMessage(nombreNodo, nombreTopico2, cpuUsage, &topicState2, useRoundRobinMessage2);

        printf("%s\n", mensajeTopico1);
        printf("%s\n", mensajeTopico2);
        printf("\n");

        if (sendMessageToBroker(&brokerConnection, mensajeTopico1) == -1)
        {
            closeBrokerConnection(&brokerConnection);
            return 1;
        }
        sleep(1);
        if (sendMessageToBroker(&brokerConnection, mensajeTopico2) == -1)
        {
            closeBrokerConnection(&brokerConnection);
            return 1;
        }

        free(memoryUsage);
        free(cpuUsage);

        sleep(5); // Espera 5 segundos
    }
    closeBrokerConnection(&brokerConnection);

    return 0;
}
