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

char *buildMessage1(char *nombreNodo, char *nombreTopico, char *valorTopico, char *idParticion, int useRoundRobin)
{
    valorTopico[strcspn(valorTopico, "\r\n")] = '\0';

    // Variable estática para rastrear el valor de idParticion
    static char partitionId[] = "1";

    size_t totalSize = strlen(nombreNodo) + strlen(nombreTopico) + strlen(valorTopico) + strlen(idParticion) + 4;

    char *mensaje = (char *)malloc(totalSize);
    if (mensaje == NULL)
    {
        perror("Error allocating memory");
        return NULL;
    }
    // Si useRoundRobin es verdadero, cambia el valor de idParticion
    if (useRoundRobin)
    {
        printf("Using Round Robin\n");
        if (strcmp(partitionId, "1") == 0)
        {
            printf("Switching to partition 2\n");
            strcpy(partitionId, "2");
        }
        else
        {
            printf("Switching to partition 1\n");
            strcpy(partitionId, "1");
        }
        snprintf(mensaje, totalSize, "%s/%s|%s|%s", nombreNodo, nombreTopico, valorTopico, partitionId);
    }
    else
    {
        snprintf(mensaje, totalSize, "%s/%s|%s|%s", nombreNodo, nombreTopico, valorTopico, idParticion);
    }

    return mensaje;
}
char *buildMessage2(char *nombreNodo, char *nombreTopico, char *valorTopico, char *idParticion, int useRoundRobin)
{
    valorTopico[strcspn(valorTopico, "\r\n")] = '\0';

    // Variable estática para rastrear el valor de idParticion
    static char partitionId[] = "1";

    size_t totalSize = strlen(nombreNodo) + strlen(nombreTopico) + strlen(valorTopico) + strlen(idParticion) + 4;

    char *mensaje = (char *)malloc(totalSize);
    if (mensaje == NULL)
    {
        perror("Error allocating memory");
        return NULL;
    }
    // Si useRoundRobin es verdadero, cambia el valor de idParticion
    if (useRoundRobin)
    {
        printf("Using Round Robin\n");
        if (strcmp(partitionId, "1") == 0)
        {
            printf("Switching to partition 2\n");
            strcpy(partitionId, "2");
        }
        else
        {
            printf("Switching to partition 1\n");
            strcpy(partitionId, "1");
        }
        snprintf(mensaje, totalSize, "%s/%s|%s|%s", nombreNodo, nombreTopico, valorTopico, partitionId);
    }
    else
    {
        snprintf(mensaje, totalSize, "%s/%s|%s|%s", nombreNodo, nombreTopico, valorTopico, idParticion);
    }

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

    while (1)
    {
        getMemoryUsage(&memoryUsage);
        getCPUUsage(&cpuUsage);

        char *mensajeTopico1 = buildMessage1(nombreNodo, nombreTopico1, memoryUsage, idParticionTopico1Str, useRoundRobin);
        char *mensajeTopico2 = buildMessage2(nombreNodo, nombreTopico2, cpuUsage, idParticionTopico2Str, useRoundRobin);

        printf("%s\n", mensajeTopico1);
        printf("%s\n", mensajeTopico2);
        free(memoryUsage);
        free(cpuUsage);

        sleep(5); // Espera 5 segundos
    }

    return 0;
}
