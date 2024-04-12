// Author: Somia
//=============Contents============//
// Queue Struct
// Process Struct <MAY BE EDITED>
//=================================//
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

typedef short bool;
#define true 1
#define false 0

#define SHKEY 300

///==============================
// don't mess with this variable//
int *shmaddr; //
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
        // Make sure that the clock exists
        printf("Wait! The clock not initialized yet!\n");
        sleep(1);
        shmid = shmget(SHKEY, 4, 0444);
    }
    shmaddr = (int *)shmat(shmid, (void *)0, 0);
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

//==============Process==============//
struct Process
{
    int ID;
    int BT;
    int AT;
    int Priority;
    int RT;
    int state; // 0=Started, 1=Resumed, 2=Stopped, 3=Finished
};
///////////////////////////////////////

//===============Queue===============//
// Define the structure for a queue node

struct QNode
{
    struct Process data;
    struct QNode *next;
};

struct Queue
{
    struct QNode *front;
    struct QNode *rear;
    int count;
};

bool isEmpty(struct Queue *q)
{
    return q->front == NULL;
}

void initializeQueue(struct Queue *q)
{
    q->front = q->rear = NULL;
}

void enqueue(struct Queue *q, struct Process x)
{
    struct QNode *temp = (struct QNode *)malloc(sizeof(struct QNode));
    temp->data = x;
    temp->next = NULL;

    if (q->rear == NULL)
    {
        q->front = q->rear = temp;
    }
    else
    {
        q->rear->next = temp;
        q->rear = temp;
    }
    (q->count)++;
}

bool dequeue(struct Queue *q, struct Process *p)
{
    if (q->front == NULL)
    {
        p = NULL;
        return 0;
    }

    struct QNode *temp = q->front;
    q->front = q->front->next;

    if (q->front == NULL)
    {
        q->rear = NULL;
    }
    (q->count)--;
    *p = temp->data;
    free(temp);
    return 1;
}
union Semun
{
    int val;
    struct semid_ds *buf;
    unsigned short *array;
    struct seminfo *__buf;
};

void down(int sem)
{
    struct sembuf op;

    op.sem_num = 0;
    op.sem_op = -1;
    op.sem_flg = !IPC_NOWAIT;

    if (semop(sem, &op, 1) == -1)
    {
        perror("Error in down()");
        exit(-1);
    }
}

void up(int sem)
{
    struct sembuf op;

    op.sem_num = 0;
    op.sem_op = 1;
    op.sem_flg = !IPC_NOWAIT;

    if (semop(sem, &op, 1) == -1)
    {
        perror("Error in up()");
        exit(-1);
    }
}