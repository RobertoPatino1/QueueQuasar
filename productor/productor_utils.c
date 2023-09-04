#include "productor_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void getMemoryUsage(char **memoryUsage)
{
    char buffer[128];
    FILE *fp = popen("free -h | awk 'NR==2{print $3}'", "r");
    if (fp == NULL)
    {
        perror("Error al ejecutar free");
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
        perror("Error al ejecutar mpstat");
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
        perror("Error alojando memoria");
        return NULL;
    }

    snprintf(mensaje, totalSize, "%s/%s|%s|%s", nombreNodo, nombreTopico, valorTopico, topicState->idParticion);

    return mensaje;
}
