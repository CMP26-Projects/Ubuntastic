#include <stdio.h>
#include <stdlib.h>
#include "./queue.h"
#include "../SystemComponents/processUnits.h"


int main() {
    // Create a queue
    queue_t* queue = createQueue();

    // Test enqueue
    printf("Enqueueing processes...\n");
    int processInfo1[] = {1, 0, 5, 3}; // Example process info
    int processInfo2[] = {2, 0, 3, 2}; // Example process info
    int processInfo3[] = {3, 0, 7, 1}; // Example process info

    process_t* process1 = createProcess(processInfo1);
    process_t* process2 = createProcess(processInfo2);
    process_t* process3 = createProcess(processInfo3);

    #ifdef DEBUG
    printf("Enqueueing process 1...\n");
    #endif
    enqueue(queue, process1);

    #ifdef DEBUG
    printf("Enqueueing process 2...\n");
    #endif
    enqueue(queue, process2);

    #ifdef DEBUG
    printf("Enqueueing process 3...\n");
    #endif
    enqueue(queue, process3);

    // Print the queue
    printf("\nQueue contents:\n");
    printQueue(queue,printProcess);

    // Test dequeue
    printf("\nDequeuing processes...\n");
    process_t* dequeuedProcess;
    while (isEmptyQueue(queue))
    {
        dequeuedProcess=(process_t*)front(queue);
        dequeue(queue);
        printf("Dequeued Process ID: %d\n", dequeuedProcess->ID);
        free(dequeuedProcess); // Free memory allocated for the dequeued process
    }

    #ifdef DEBUG
    printf("Destroying the queue...\n");
    #endif
    destroyQueue(queue);

    #ifdef DEBUG
    printf("Queue destroyed.\n");
    #endif

    return 0;
}
