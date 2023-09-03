#include "multiPartitionQueue.h"
int main()
{
    int numPartitions = 4;
    MultiPartitionQueue *queue = createMultiPartitionQueue(numPartitions);

    enqueue(queue, "seccion_memoria", 0, "Dato 1");
    enqueue(queue, "seccion_memoria", 0, "Dato 2");
    enqueue(queue, "seccion_memoria", 1, "Dato 3");
    enqueue(queue, "seccion_memoria", 1, "Dato 4");

    enqueue(queue, "seccion_cpu", 2, "Dato5");
    enqueue(queue, "seccion_cpu", 2, "Dato6");
    enqueue(queue, "seccion_cpu", 3, "Dato7");
    enqueue(queue, "seccion_cpu", 3, "Dato8");

    printPartitionContents(queue, "seccion_memoria", 0);
    // Dato1, Dato2
    printPartitionContents(queue, "seccion_memoria", 1);
    // Dato3, Dato4
    printPartitionContents(queue, "seccion_cpu", 2);
    printPartitionContents(queue, "seccion_cpu", 3);
    freeMultiPartitionQueue(queue);
    return 0;
}
