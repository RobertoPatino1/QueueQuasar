#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct
{
    char *idParticion;
} TopicState;

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
        printf("Using Round Robin\n");
        if (strcmp(topicState->idParticion, "1") == 0)
        {
            printf("Switching to partition 2\n");
            topicState->idParticion = "2";
        }
        else
        {
            printf("Switching to partition 1\n");
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
int main(int argc, char *argv[])
{
    if (argc < 4)
    {
        printf("Usage: %s <nombre_nodo> <nombre_topico_1> <nombre_topico_2> [<id_particion_topico_1>] [<id_particion_topico_2>]\n", argv[0]);
        return 1;
    }

    char *nombreNodo = argv[1];
    char *nombreTopico1 = argv[2];
    char *nombreTopico2 = argv[3];

    char *idParticionTopico1Str = NULL;
    char *idParticionTopico2Str = NULL;
    int useRoundRobin = 0;
    if (argc >= 6)
    {
        idParticionTopico1Str = argv[4];
        idParticionTopico2Str = argv[5];
    }
    else
    {
        idParticionTopico1Str = "2"; // Colocamos 2 para iniciar por defecto en la particion 1
        idParticionTopico2Str = "2";
        useRoundRobin = 1;
    }

    char *memoryUsage = NULL;
    char *cpuUsage = NULL;
    TopicState topicState1 = {.idParticion = idParticionTopico1Str};
    TopicState topicState2 = {.idParticion = idParticionTopico2Str};
    while (1)
    {
        getMemoryUsage(&memoryUsage);
        getCPUUsage(&cpuUsage);

        char *mensajeTopico1 = buildMessage(nombreNodo, nombreTopico1, memoryUsage, &topicState1, useRoundRobin);
        char *mensajeTopico2 = buildMessage(nombreNodo, nombreTopico2, cpuUsage, &topicState2, useRoundRobin);

        printf("%s\n", mensajeTopico1);
        printf("%s\n", mensajeTopico2);
        free(memoryUsage);
        free(cpuUsage);

        sleep(5); // Espera 5 segundos
    }

    return 0;
}
