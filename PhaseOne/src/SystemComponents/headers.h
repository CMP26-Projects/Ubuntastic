//=================================//
#pragma once

#include <stdio.h> //if you don't use scanf/printf change this include
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include "minHeap.h"
#include "linkedList.h"

#define NRM  "\x1B[0m"
#define RED  "\x1B[31m"
#define GRN  "\x1B[32m"
#define YEL  "\x1B[33m"
#define BLU  "\x1B[34m"
#define MAG  "\x1B[35m"
#define CYN  "\x1B[36m"
#define WHT  "\x1B[37m"
#define true 1
#define false 0
#define SRTN_t 0
#define HPF_t 1
#define RR_t 2
#define SHKEY 3000
#define MAX_SIZE 10000
typedef int clk_t;

// Uncomment the following line to enable debugging
#define DEBUG

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

struct PCB
{
    struct PCB* next;
    pid_t pid;
    process_t* process;
};


struct Scheduler{
int algo;
struct PCB* pcb;
process_t* runningP;
process_t* lastRecieved;
int timeSlice;
int busyTime;
int finishedProcessesNum;
int recievedProcessesNum;
int totalProcessesNum;
int totalWT;
float totalWTAT;
void* readyContainer;
};

struct Scheduler* sch;

struct PCB *pcbHead;
struct PCB *pcbTail;
int numOfSlots;
///Time Functions///

///==============================
// don't mess with this variable//
static int *shmaddr; //
//===============================

clk_t getClk();

/*
 * All process call this function at the beginning to establish communication between them and the clock module.
 * Again, remember that the clock is only emulation!
 */
void initClk();

void destroyClk(bool terminateAll);

void sleepMilliseconds(long milliseconds);
//===============MessageQueue===============//

typedef struct
{
    long mtype;
    int data[4];
}processMsg;


processMsg createMsg(int info[]);
int createMessageQueue();
int creatShMemory();

