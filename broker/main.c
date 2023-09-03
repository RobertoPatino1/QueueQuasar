#include "multiPartitionQueue.h"
#include "connection_utils.h"
int main()
{
    int numPartitions = 3;
    MultiPartitionQueue *queue = createMultiPartitionQueue(numPartitions);

    enqueue(queue, "memoria", 0, "Dato 1%");
    enqueue(queue, "memoria", 0, "Dato 2%");
    enqueue(queue, "memoria", 1, "Dato 3%");
    enqueue(queue, "cpu", 0, "Dato 4%");

    printPartitionContents(queue, "memoria", 0);
    printPartitionContents(queue, "memoria", 1);
    printPartitionContents(queue, "cpu", 0);

    enqueue(queue, "cpu", 0, "Dato 5%");

    freeMultiPartitionQueue(queue);
    return 0;
}
