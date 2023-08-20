#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct
{
    char *idParticion;
} TopicState;
char *nombreTopico1 = "memoria";
char *nombreTopico2 = "cpu";

void getMemoryUsage(char **memoryUsage)
{
    char buffer[128];
    FILE *fp = popen("free -h | awk 'NR==2{print $3}'", "r");
    if (fp == NULL)
    {
        perror("Error executing free");
        return;
    }

    if (fgets(buffer, sizeof(buffer), fp) != NULL)
    {
        *memoryUsage = strdup(buffer);
    }

    pclose(fp);
}

void getCPUUsage(char **cpuUsage)
{
    char buffer[128];
    FILE *fp = popen("mpstat 1 1 | awk '/all/ {print 100 - $NF\"%\"}'", "r");
    if (fp == NULL)
    {
        perror("Error executing mpstat");
        return;
    }

    if (fgets(buffer, sizeof(buffer), fp) != NULL)
    {
        *cpuUsage = strdup(buffer);
    }

    pclose(fp);
}

char *buildMessage(char *nombreNodo, char *nombreTopico, char *valorTopico, TopicState *topicState, int useRoundRobin)
{
    valorTopico[strcspn(valorTopico, "\r\n")] = '\0';

    if (useRoundRobin)
    {

        if (strcmp(topicState->idParticion, "1") == 0)
        {
            topicState->idParticion = "2";
        }
        else
        {
            topicState->idParticion = "1";
        }
    }

    size_t totalSize = strlen(nombreNodo) + strlen(nombreTopico) + strlen(valorTopico) + strlen(topicState->idParticion) + 4;

    char *mensaje = (char *)malloc(totalSize);
    if (mensaje == NULL)
    {
        perror("Error allocating memory");
        return NULL;
    }

    snprintf(mensaje, totalSize, "%s/%s|%s|%s", nombreNodo, nombreTopico, valorTopico, topicState->idParticion);

    return mensaje;
}
int main(int argc, char *argv[]) //./productor <nombre_nodo> <puerto_productor> <ip_broker> <puerto_broker> [id_particion_topico_1] [id_particion_topico_2]
{
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

    char *memoryUsage = NULL;
    char *cpuUsage = NULL;
    TopicState topicState1 = {.idParticion = idParticionTopico1Str};
    TopicState topicState2 = {.idParticion = idParticionTopico2Str};
    while (1)
    {
        getMemoryUsage(&memoryUsage);
        getCPUUsage(&cpuUsage);

        char *mensajeTopico1 = buildMessage(nombreNodo, nombreTopico1, memoryUsage, &topicState1, useRoundRobinMessage1);
        char *mensajeTopico2 = buildMessage(nombreNodo, nombreTopico2, cpuUsage, &topicState2, useRoundRobinMessage2);

        printf("%s\n", mensajeTopico1);
        printf("%s\n", mensajeTopico2);
        printf("\n");
        free(memoryUsage);
        free(cpuUsage);

        sleep(5); // Espera 5 segundos
    }

    return 0;
}
