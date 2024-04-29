#include <stdio.h>
#include <stdlib.h>
#include "./src/dataStructures/minHeap.h"
#include "./src/SystemComponents/processUnits.h"

// Uncomment the following line to enable debugging
// #define DEBUG
int main() {
    minHeap_t* processHeap = createMinHeap(comparePriority);

    int processInfo1[] = {1, 0, 5, 3}; // ID, Arrival Time, Run Time, Priority
    int processInfo2[] = {2, 0, 7, 2};
    int processInfo3[] = {3, 0, 3, 1};
    int processInfo4[] = {4, 0, 4, 4};

    #ifdef DEBUG
    printf("Inserting process one...\n");
    #endif

    insert(processHeap, createProcess(processInfo1));

    #ifdef DEBUG
    printf("Inserting process two...\n");
    #endif
    insert(processHeap, createProcess(processInfo2));

    #ifdef DEBUG
    printf("Inserting process three...\n");
    #endif
    insert(processHeap, createProcess(processInfo3));

    #ifdef DEBUG
    printf("Inserting process four...\n");
    #endif
    insert(processHeap, createProcess(processInfo4));

    // Print the heap
    printf("Heap of Processes:\n");

    while (processHeap->size > 0) {
        #ifdef DEBUG
        printf("Getting the min element...\n");
        #endif
        process_t *minProcess = (process_t *)getMin(processHeap);
        printProcess(minProcess);
        deleteMin(processHeap);
    }
    free(processHeap);
    
    #ifdef DEBUG
    printf("Testing Remaining Time...\n");
    #endif
    processHeap = createMinHeap(compareRemTime);

    int processInfo1[] = {1, 0, 5, 3}; // ID, Arrival Time, Run Time, Priority
    int processInfo2[] = {2, 0, 7, 2};
    int processInfo3[] = {3, 0, 3, 1};
    int processInfo4[] = {4, 0, 4, 4};

    #ifdef DEBUG
    printf("Inserting process one...\n");
    #endif

    insert(processHeap, createProcess(processInfo1));

    #ifdef DEBUG
    printf("Inserting process two...\n");
    #endif
    insert(processHeap, createProcess(processInfo2));

    #ifdef DEBUG
    printf("Inserting process three...\n");
    #endif
    insert(processHeap, createProcess(processInfo3));

    #ifdef DEBUG
    printf("Inserting process four...\n");
    #endif
    insert(processHeap, createProcess(processInfo4));

    // Print the heap
    printf("Heap of Processes:\n");

    while (processHeap->size > 0) {
        #ifdef DEBUG
        printf("Getting the min element...\n");
        #endif
        process_t *minProcess = (process_t *)getMin(processHeap);
        printProcess(minProcess);
        deleteMin(processHeap);
    }

    return 0;
}
