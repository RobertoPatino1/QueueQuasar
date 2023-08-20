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
        idParticionStr = strdup(argv[4]);
    }
    else
    {
        idParticionStr = strdup("Round Robin");
    }

    char *memoryUsage = NULL;
    char *cpuUsage = NULL;

    while (1)
    {
        getMemoryUsage(&memoryUsage);
        getCPUUsage(&cpuUsage);

        printf("Nombre Nodo: %s\n", nombreNodo);
        printf("Nombre Topico 1 (%s): Memory Usage: %s", nombreTopico1, memoryUsage);
        printf("Nombre Topico 2 (%s): CPU Usage: %s", nombreTopico2, cpuUsage);
        printf("Transmitiendo contenido a la particion: %s\n", idParticionStr);

        free(memoryUsage);
        free(cpuUsage);

        sleep(5); // Espera 5 segundos
    }

    free(idParticionStr);

    return 0;
}
