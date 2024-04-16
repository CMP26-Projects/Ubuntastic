// Author: Somia
//=============Contents============//
// Queue Struct
// Process Struct <MAY BE EDITED>
// MinHeap Struct <by Ahmed>
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
//==============Message Queue =========//

struct schdularType
{
    long mtype;
    int schedType;
};

///////////////////////////////////////
///// Data Structures Implementation///
///////////////////////////////////////

//==============Process==============//
enum processState
{
    STARTED,
    RESUMED,
    STOPPED,
    FINISHED,
    WAITING
};

struct Process
{
    int ID;
    int RT; // running time
    int AT; // arrival time
    int Priority;
    int RemT; // remaining time
    enum processState state;
};

struct msgbuf
{
    // struct Process *msgProcess;
    long mtype;
    int data[6];
};
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
    q->count=0;
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

bool dequeue(struct Queue *q, struct Process **p)
{
    printf("before dequeue : %d \n", q->count);
    if (q->front == NULL)
    {
        *p = NULL;
        return false;
    }

    struct QNode *temp = q->front;
    q->front = q->front->next;

    if (q->front == NULL)
    {
        q->rear = NULL;
    }
    --(q->count);

    // Allocate memory for p and copy the data from temp->data
    if(*p == NULL)
    *p = (struct Process *)malloc(sizeof(struct Process));
    **p = temp->data;
    free(temp);

    return true;
}
void printQueue(struct Queue *q)
{
    struct QNode *temp = q->front;
    while (temp != NULL)
    {
        printf("ID: %d, AT: %d, RT: %d, Priority: %d, RemT: %d, state: %d\n", temp->data.ID, temp->data.AT, temp->data.RT, temp->data.Priority, temp->data.RemT, temp->data.state);
        temp = temp->next;
    }
}
void destroyQueue(struct Queue *q)
{
    struct Process *p;
    while (dequeue(q, &p))
    {
        free(p);
    }
}

//===============minHeap===============//

struct minHeap
{
    int size;
    bool criteria;
    struct Process *arr;
};

struct minHeap createMinHeap(int criteria)
{
    struct minHeap heap;
    heap.size = 0;
    heap.criteria = criteria;
    return heap;
}

bool lessThan(struct Process a, struct Process b, bool criteria)
{
    if (criteria == 0)
        return (a.RemT < b.RemT); // for SRTN
    else
        return (a.Priority < b.Priority); // for non pre-emitive HPF
}

void insertNode(struct minHeap *heap, struct Process P)
{
    heap->arr = (heap->size == 0) ? malloc(sizeof(struct Process)) : realloc(heap->arr, (heap->size + 1) * sizeof(struct Process));
    struct Process newProcess = P;
    int i = (heap->size)++;
    while (i > 0 && lessThan(newProcess, heap->arr[(i - 1) / 2], heap->criteria))
    {
        heap->arr[i] = heap->arr[(i - 1) / 2];
        i = (i - 1) / 2;
    }
    heap->arr[i] = newProcess;
}

void heapify(struct minHeap *heap, int i)
{
    int smallest = i;
    int left = 2 * i + 1;
    int right = 2 * i + 2;
    if (left < heap->size && lessThan(heap->arr[left], heap->arr[smallest], heap->criteria))
    {
        smallest = left;
    }
    if (right < heap->size && lessThan(heap->arr[right], heap->arr[smallest], heap->criteria))
    {
        smallest = right;
    }
    if (smallest != i)
    {
        struct Process temp = heap->arr[i];
        heap->arr[i] = heap->arr[smallest];
        heap->arr[smallest] = temp;
        heapify(heap, smallest);
    }
}

struct Process getMin(struct minHeap *heap)
{
    return heap->arr[0];
}

void pop(struct minHeap *heap)
{
    if (heap->size == 0)
        return;
    struct Process temp = heap->arr[0];
    heap->arr[0] = heap->arr[heap->size - 1];
    heap->size--;
    heapify(heap, 0);
}
void printHeap(struct minHeap *heap)
{
    for (int i = 0; i < heap->size; i++)
    {
        printf("ID: %d, AT: %d, RT: %d, Priority: %d, RemT: %d, state: %d\n", heap->arr[i].ID, heap->arr[i].AT, heap->arr[i].RT, heap->arr[i].Priority, heap->arr[i].RemT, heap->arr[i].state);
    }
}

void destroyHeap(struct minHeap *heap)
{
    free(heap->arr);
    heap->size = 0;
}
//===============Semaphores===============//

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

int createMessageQueue()
{
    key_t key_id;
    int msgid, recval;

    key_id = ftok("file.txt", 65);
    msgid = msgget(key_id, 0666 | IPC_CREAT);

    if (msgid == -1)
    {
        perror("there error in creates a queue !");
        exit(-1);
    }
    printf("Message queue ID is : %d \n", msgid);
    return msgid;
}

int creatShMemory()
{
    key_t key = ftok("file.txt", 67);
    int shmid = shmget(key, 50, IPC_CREAT | 0666);
    if ((long)shmid == -1)
    {
        perror("Error in creating shm!");
        exit(-1);
    }

    return shmid;
}

int Creatsem(int *sem2)
{

    key_t keyid_sem1 = ftok("file.txt", 62);
    key_t keyid_sem2 = ftok("file.txt", 63);
    *sem2 = semget(keyid_sem2, 1, 0666 | IPC_CREAT);
    int sem1 = semget(keyid_sem1, 1, 0666 | IPC_CREAT);
    if (sem1 == -1 || *sem2 == -1)
    {
        perror("Error in create sem");
        exit(-1);
    }
    return sem1;
}