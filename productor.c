#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUFFER_SIZE 128

void getMemoryUsage(char *memoryUsage)
{
    char buffer[BUFFER_SIZE];
    FILE *fp = popen("free -h | awk 'NR==2{print $3}'", "r");
    if (fp == NULL)
    {
        perror("Error executing free");
        return;
    }

    if (fgets(buffer, sizeof(buffer), fp) != NULL)
    {
        snprintf(memoryUsage, BUFFER_SIZE, "Memory Usage: %s", buffer);
    }

    pclose(fp);
}

void getCPUUsage(char *cpuUsage)
{
    char buffer[BUFFER_SIZE];
    FILE *fp = popen("mpstat 1 1 | awk '/all/ {print 100 - $NF\"%\"}'", "r");
    if (fp == NULL)
    {
        perror("Error executing mpstat");
        return;
    }

    if (fgets(buffer, sizeof(buffer), fp) != NULL)
    {
        snprintf(cpuUsage, BUFFER_SIZE, "CPU Usage: %s", buffer);
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

    char idParticionStr[BUFFER_SIZE];
    if (argc >= 5)
    {
        snprintf(idParticionStr, sizeof(idParticionStr), "ID Particion: %s", argv[4]);
    }
    else
    {
        strcpy(idParticionStr, "Round Robin");
    }

    char memoryUsage[BUFFER_SIZE];
    char cpuUsage[BUFFER_SIZE];

    while (1)
    {
        getMemoryUsage(memoryUsage);
        getCPUUsage(cpuUsage);

        printf("Nombre Nodo: %s\n", nombreNodo);
        printf("Nombre Topico 1 (%s): %s\n", nombreTopico1, memoryUsage);
        printf("Nombre Topico 2 (%s): %s\n", nombreTopico2, cpuUsage);
        printf("%s\n", idParticionStr);

        sleep(5); // Espera 5 segundos
    }

    return 0;
}
