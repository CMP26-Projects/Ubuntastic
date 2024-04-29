#pragma once
#include "headers.h"
#define TYPE_CHECK(ptr, type) _Generic((ptr), type: 1, default: 0) //For searching by val function 

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

typedef struct pcb_slot
{
    pid_t pid;
    process_t* process;
}pcb_slot;

process_t* createProcess(int info[]);

void printProcess(void* data);

int comparePriority(void* a ,void* b);

int compareRemTime(void* a ,void* b);

pcb_slot* createSlot(int id, process_t* p);

int compareSlot(void* data ,void* key);

void freeSlot(void*);