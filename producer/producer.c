#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "producer_utils.h"
#include "connection_utils.h"

char *topicName1 = "memory";
char *topicName2 = "cpu";

int main(int argc, char *argv[])
{
    BrokerConnection brokerConnection;

    if (argc < 4)
    {
        printf("Usage: %s <node_name> <broker_ip> <broker_port> [<partition_id_topic_1>] [<partition_id_topic_2>]\n", argv[0]);
        return 1;
    }

    char *nodeName = argv[1];
    char *brokerIP = argv[2];
    char *brokerPort = argv[3];

    char *idPartition1TopicStr = NULL;
    char *idPartition2TopicStr = NULL;
    int useRoundRobinMessage1 = 0;
    int useRoundRobinMessage2 = 0;
    if (argc > 6)
    {
        idPartition1TopicStr = argv[5];
        idPartition2TopicStr = argv[6];
    }
    // Handle single partition ID
    else if (argc == 6)
    {
        idPartition1TopicStr = argv[5];
        idPartition2TopicStr = "2";
        useRoundRobinMessage2 = 1;
        printf("No partition ID was provided for topic 2, Round Robin will be used...\n\n");
    }

    else
    {
        idPartition1TopicStr = "2"; // Colocamos 2 para iniciar por defecto en la particion 1
        idPartition2TopicStr = "2";
        useRoundRobinMessage1 = 1;
        useRoundRobinMessage2 = 1;
        printf("No partition ID was provided for topic 1, Round Robin will be used...\n");
        printf("No partition ID was provided for topic 2, Round Robin will be used...\n\n");
    }
    if (establishBrokerConnection(&brokerConnection, brokerIP, atoi(brokerPort)) == -1)
    {
        return 1;
    }
    char *memoryUsage = NULL;
    char *cpuUsage = NULL;
    TopicState topicState1 = {.partitionID = idPartition1TopicStr};
    TopicState topicState2 = {.partitionID = idPartition2TopicStr};

    printf("Begining data transmition to the broker: \n\n");
    while (1)
    {
        getMemoryUsage(&memoryUsage);
        getCPUUsage(&cpuUsage);

        char *messageTopic1 = buildMessage(nodeName, topicName1, memoryUsage, &topicState1, useRoundRobinMessage1);
        char *messageTopic2 = buildMessage(nodeName, topicName2, cpuUsage, &topicState2, useRoundRobinMessage2);

        printf("%s\n", messageTopic1);
        printf("%s\n", messageTopic2);
        printf("\n");

        if (sendMessageToBroker(&brokerConnection, messageTopic1) == -1)
        {
            closeBrokerConnection(&brokerConnection);
            return 1;
        }
        sleep(3);
        if (sendMessageToBroker(&brokerConnection, messageTopic2) == -1)
        {
            closeBrokerConnection(&brokerConnection);
            return 1;
        }

        free(memoryUsage);
        free(cpuUsage);
    }
    closeBrokerConnection(&brokerConnection);

    return 0;
}
