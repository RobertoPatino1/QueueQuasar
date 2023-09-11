#ifndef PRODUCER_UTILS_H
#define PRODUCER_UTILS_H

typedef struct
{
    char *partitionID;
} TopicState;

void getMemoryUsage(char **memoryUsage);
void getCPUUsage(char **cpuUsage);
char *buildMessage(char *nodeName, char *topicName, char *topicValue, TopicState *topicState, int useRoundRobin);

#endif
