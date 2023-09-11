#include "producer_utils.h"
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
        perror("Error executing free command");
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
        perror("Error executing mpstat command");
        return;
    }

    if (fgets(buffer, sizeof(buffer), fp) != NULL)
    {
        *cpuUsage = strdup(buffer);
    }

    pclose(fp);
}

char *buildMessage(char *nodeName, char *topicName, char *topicValue, TopicState *topicState, int useRoundRobin)
{
    topicValue[strcspn(topicValue, "\r\n")] = '\0';

    if (useRoundRobin)
    {
        if (strcmp(topicState->partitionID, "1") == 0)
        {
            topicState->partitionID = "2";
        }
        else
        {
            topicState->partitionID = "1";
        }
    }

    size_t totalSize = strlen(nodeName) + strlen(topicName) + strlen(topicValue) + strlen(topicState->partitionID) + 4;

    char *message = (char *)malloc(totalSize);
    if (message == NULL)
    {
        perror("Error allocating memory");
        return NULL;
    }

    snprintf(message, totalSize, "%s/%s|%s|%s", nodeName, topicName, topicValue, topicState->partitionID);

    return message;
}
