#include <stdio.h>      //if you don't use scanf/printf change this include
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

typedef short bool;
#define true 1
#define false 0

#define SHKEY 300

///==============================
//don't mess with this variable//
int * shmaddr;                 //
//===============================

int getClk()
{
    return *shmaddr;
}


/*
 * All process call this function at the beginning to establish communication between them and the clock module.
 * Again, remember that the clock is only emulation!
*/
void initClk()
{
    int shmid = shmget(SHKEY, 4, 0444);
    while ((int)shmid == -1)
    {
        //Make sure that the clock exists
        printf("Wait! The clock not initialized yet!\n");
        sleep(1);
        shmid = shmget(SHKEY, 4, 0444);
    }
    shmaddr = (int *) shmat(shmid, (void *)0, 0);
}


/*
 * All process call this function at the end to release the communication
 * resources between them and the clock module.
 * Again, Remember that the clock is only emulation!
 * Input: terminateAll: a flag to indicate whether that this is the end of simulation.
 *                      It terminates the whole system and releases resources.
*/

void destroyClk(bool terminateAll)
{
    shmdt(shmaddr);
    if (terminateAll)
    {
        killpg(getpgrp(), SIGINT);
    }
}
//=============Data Structures===============//

//==============ProcessData==============//
struct ProcessData
{
    int ID;
    int BT;
    int AT;
    int Priority;
};
///////////////////////////////////////

//===============Queue===============//
// Define the structure for a queue node

struct QNode {
    struct ProcessData data;
    struct QNode* next;
};

// Define the structure for the queue itself
struct Queue {
    struct QNode* front;
    struct QNode* rear;
    int count;
};

bool isEmpty(struct Queue* q)
{
    return q->front == NULL;
}

// Initialize an empty queue
void initializeQueue(struct Queue* q) {
    q->front = q->rear = NULL;
}

// Enqueue operation: Add a new element to the rear of the queue
void enQueue(struct Queue* q, struct ProcessData x) {
    struct QNode* temp = (struct QNode*)malloc(sizeof(struct QNode));
    temp->data = x;
    temp->next = NULL;

    if (q->rear == NULL) {
        // If the queue is empty, set both front and rear to the new node
        q->front = q->rear = temp;
    } else {
        // Otherwise, add the new node after the current rear and update rear
        q->rear->next = temp;
        q->rear = temp;
    }
    (q->count)++;
}

// Dequeue operation: Remove the front element from the queue
bool deQueue(struct Queue* q, struct ProcessData *p) {
    if (q->front == NULL) {
        // Queue is empty, nothing to dequeue
        p=NULL;
        return 0;
    }

    struct QNode* temp = q->front;
    q->front = q->front->next;

    if (q->front == NULL) {
        // If the front becomes NULL, the queue is now empty, so update rear
        q->rear = NULL;
    }
    (q->count)--;
    *p = temp->data;
    free(temp);
   return 1;
}
