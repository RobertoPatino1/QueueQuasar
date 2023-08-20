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

char *buildMessage(char *nombreNodo, char *nombreTopico, char *valorTopico, char *idParticion)
{
    valorTopico[strcspn(valorTopico, "\r\n")] = '\0';
    idParticion[strcspn(idParticion, "\r\n")] = '\0';

    size_t totalSize = strlen(nombreNodo) + strlen(nombreTopico) + strlen(valorTopico) + strlen(idParticion) + 4; // +4 para los delimitadores y el terminador nulo

    char *mensaje = (char *)malloc(totalSize);
    if (mensaje == NULL)
    {
        perror("Error allocating memory");
        return NULL;
    }
    // Building the desired string
    snprintf(mensaje, totalSize, "%s/%s|%s|%s", nombreNodo, nombreTopico, valorTopico, idParticion);

    return mensaje;
}

int main(int argc, char *argv[])
{
    if (argc < 4)
    {
        printf("Usage: %s <nombre_nodo> <nombre_topico_1> <nombre_topico_2> [<id_particion>]\n", argv[0]);
        return 1;
    }

    char *nombreNodo = argv[1];
    char *nombreTopico1 = argv[2];
    char *nombreTopico2 = argv[3];

    char *idParticionStr = NULL;
    if (argc >= 5)
    {
        idParticionStr = argv[4];
    }
    else
    {
        // idParticionStr = strdup("Round Robin");
        // handleRoundRobin();
        // idParticionStr = NULL;
    }

    char *memoryUsage = NULL;
    char *cpuUsage = NULL;

    while (1)
    {
        getMemoryUsage(&memoryUsage);
        getCPUUsage(&cpuUsage);

        char *mensajeTopico1 = buildMessage(nombreNodo, nombreTopico1, memoryUsage, idParticionStr);
        char *mensajeTopico2 = buildMessage(nombreNodo, nombreTopico2, cpuUsage, idParticionStr);

        printf("%s\n", mensajeTopico1);
        printf("%s\n", mensajeTopico2);
        free(memoryUsage);
        free(cpuUsage);

        sleep(5); // Espera 5 segundos
    }

    free(idParticionStr);

    return 0;
}
