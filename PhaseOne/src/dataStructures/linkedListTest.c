#include <stdio.h>
#include <stdlib.h>
#include "linkedList.h"
#include "../SystemComponents/processUnits.h"

int main() {
    // Create a linked list
    list_t* linkedList = createLinkedList(freeSlot,compareSlot);

    int processInfo1[] = {1, 0, 5, 3}; // ID, Arrival Time, Run Time, Priority
    int processInfo2[] = {2, 0, 7, 2};
    int processInfo3[] = {3, 0, 3, 1};
    int processInfo4[] = {4, 0, 4, 4};


    #ifdef DEBUG
    printf("Creating slots...\n");
    #endif    
    pcb_slot* pcb1 = createSlot(1100,createProcess(processInfo1));
    pcb_slot* pcb2 = createSlot(2200,createProcess(processInfo2));
    pcb_slot* pcb3 = createSlot(3300,createProcess(processInfo3));
    pcb_slot* pcb4 = createSlot(4400,createProcess(processInfo4));

    #ifdef DEBUG
    printf("Inserting into list...\n");
    #endif
    insertAtEnd(linkedList, pcb1);
    insertAtEnd(linkedList, pcb2);
    insertAtEnd(linkedList, pcb3);
    insertAtEnd(linkedList, pcb4);


    int key=2200;
    
    #ifdef DEBUG
    printf("Get specfic node...\n");
    #endif

    printProcess(((pcb_slot*)((node_t*)getNodeByValue(linkedList,key)->data))->process);
    deleteUsingVal(linkedList,key);
    // Print the linked list of processes
    printf("Linked List of Processes:\n");
    printList(linkedList,printProcess);

    #ifdef DEBUG
    printf("Destroying list...\n");
    #endif

    // Free memory
    destroyList(linkedList);

    return 0;
}
