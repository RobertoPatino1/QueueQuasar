#ifndef PRODUCTOR_UTILS_H
#define PRODUCTOR_UTILS_H

typedef struct
{
    char *idParticion;
} TopicState;

void getMemoryUsage(char **memoryUsage);
void getCPUUsage(char **cpuUsage);
char *buildMessage(char *nombreNodo, char *nombreTopico, char *valorTopico, TopicState *topicState, int useRoundRobin);

#endif
