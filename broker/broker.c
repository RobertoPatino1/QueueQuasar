#include "connection_utils.h"
#include <signal.h>

pthread_mutex_t broker_mutex;
int main(int argc, char *argv[])
{
    signal(SIGPIPE, SIG_IGN);

    if (argc != 3)
    {
        printf("Usage: %s <broker_port_producer> <broker_port_consumer> \n", argv[0]);
        return 1;
    }

    int broker_port_producer = atoi(argv[1]);
    int broker_port_consumer = atoi(argv[2]);
    if (broker_port_producer <= 0 || broker_port_consumer <= 0)
    {
        printf("Error: Puertos inválidos\n");
        return 1;
    }

    int broker_sock_producer, broker_sock_consumer;
    struct sockaddr_in broker_addr_producer, broker_addr_consumer;

    broker_sock_producer = socket(AF_INET, SOCK_STREAM, 0);
    if (broker_sock_producer == -1)
    {
        printf("Error: Unable to create producer socket\n");
        return 1;
    }

    broker_sock_consumer = socket(AF_INET, SOCK_STREAM, 0);
    if (broker_sock_consumer == -1)
    {
        printf("Error: Unable to create consumer socket\n");
        return 1;
    }

    broker_addr_producer.sin_family = AF_INET;
    broker_addr_producer.sin_addr.s_addr = INADDR_ANY;
    broker_addr_producer.sin_port = htons(broker_port_producer);

    broker_addr_consumer.sin_family = AF_INET;
    broker_addr_consumer.sin_addr.s_addr = INADDR_ANY;
    broker_addr_consumer.sin_port = htons(broker_port_consumer);

    if (bind(broker_sock_producer, (struct sockaddr *)&broker_addr_producer, sizeof(broker_addr_producer)) < 0)
    {
        printf("Error: Try to use a valid port for the producer\n");
        close(broker_sock_producer);
        return 1;
    }

    if (bind(broker_sock_consumer, (struct sockaddr *)&broker_addr_consumer, sizeof(broker_addr_consumer)) < 0)
    {
        printf("Error: Try to use a valid port for the consumer\n");
        close(broker_sock_consumer);
        return 1;
    }

    listen(broker_sock_producer, 10);
    listen(broker_sock_consumer, 10);
    printf("Broker port: %d listening for producer\n", broker_port_producer);
    printf("Broker port: %d listening for consumer\n", broker_port_consumer);

    if (pthread_mutex_init(&broker_mutex, NULL) != 0)
    {
        printf("Error while initializing mutex\n");
        return 1;
    }

    pthread_t producer_connection_thread_id, consumer_connection_thread_id;
    MultiPartitionQueue *mp_queue = createMultiPartitionQueue(4);
    mp_queue_producer = createMultiPartitionQueue(4);
    struct ThreadContent data;
    data.broker_sock_producer = broker_sock_producer;
    data.mp_queue = mp_queue;

    if (pthread_create(&producer_connection_thread_id, NULL, handle_producer_connections, (void *)&data) != 0)
    {
        printf("Error creating producer connections thread\n");
        return 1;
    }

    if (pthread_create(&consumer_connection_thread_id, NULL, handle_consumer_connections, (void *)&broker_sock_consumer) != 0)
    {
        printf("Error creating consumer connections thread\n");
        return 1;
    }

    while (1)
    {
        sleep(1);
    }
    pthread_mutex_destroy(&broker_mutex);

    close(broker_sock_producer);
    close(broker_sock_consumer);
    return 0;
}
