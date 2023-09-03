#include "multiPartitionQueue.h"

int main()
{
    int numPartitions = 3;
    MultiPartitionQueue *queue = createMultiPartitionQueue(numPartitions);

    enqueue(queue, "seccion_memoria", 0, "Dato 1");
    enqueue(queue, "seccion_memoria", 0, "Dato 2");
    enqueue(queue, "seccion_memoria", 1, "Dato 3");
    enqueue(queue, "seccion_cpu", 0, "Dato 4");

    printPartitionContents(queue, "seccion_memoria", 0);
    printPartitionContents(queue, "seccion_memoria", 1);
    printPartitionContents(queue, "seccion_cpu", 0);

    enqueue(queue, "seccion_cpu", 0, "Dato 5");

    freeMultiPartitionQueue(queue);
    return 0;
}
