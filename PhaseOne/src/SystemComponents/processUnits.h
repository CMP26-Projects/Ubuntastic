#ifndef _PROCESSUNITS_H
#define _PROCESSUNITS_H
#include "headers.h"

typedef enum
{   
    ARRIVED,   
    STARTED,
    RESUMED,
    STOPPED,
    FINISHED,   
    WAITING //FOR PHASE 2
}state_t;

typedef struct
{
    int ID; //Simulated ID
    int priority; //Priority
    int RT; //Run time
    int RemT; //Remaining time
    int WT; //Waiting time 
    int TAT; //Turnaround time
    float WTAT; //Weighted turnaround time
    clk_t AT; //Arrival time
    clk_t lastRun; //Last time this process has run
    state_t state; //Current state of the process
}process_t;

typedef struct PCB
{
    pid_t pid;
    process_t* process;
}PCB_t;

typedef struct
{
    PCB_t* head;
    PCB_t* tail;
    int size;
}List_t;

process_t* createProcess(int processInfo[]);

List_t* createList();

PCB_t* insertSlot(List_t* list, pid_t id, process_t* p);

PCB_t* getByPid(List_t* list, pid_t key);

PCB_t* getByProcess(List_t* list, process_t* key);

void freeSlotData(PCB_t* slot);

bool deleteSlot(List_t* list,process_t* p);

void destroyList(List_t* list);

#endif