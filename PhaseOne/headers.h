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

typedef int clk_t;
typedef short bool;
#define true 1
#define false 0
#define SHKEY 300
#define SRTN 0
#define HPF 1
#define RR 2

///==============================
// don't mess with this variable//
int *shmaddr; //
//===============================

clk_t getClk()
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
        shmid = shmget(SHKEY, 4, 0444);
        sleep(1);
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

///////////////////////////////////////
//////////Structs Implementation///////
///////////////////////////////////////

typedef struct msgbuf
{
    long mtype;
    int data[4];
}msgbuf;

//==============Process==============//
typedef enum processState
{   
    ARRIVED,   
    STARTED,
    RESUMED,
    STOPPED,
    FINISHED,   
    WAITING
}state_t;

typedef struct Process
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

typedef struct PCB{
    pid_t pid; //Real Unix pid
    process_t* process; //To link the process with PCB (Not Sure)
}pcb_t;

//===============Queue===============//
typedef struct QNode
{
    process_t data;
    struct QNode* next;
}node_t;

typedef struct Queue
{
    node_t* front;
    node_t* rear;
    int count;
}queue_t;

bool isEmpty(queue_t*);

void initializeQueue(queue_t*);

void enqueue(queue_t* q, process_t);

bool dequeue(queue_t*,process_t**);

void printQueue(queue_t*);

void destroyQueue(queue_t*);

//===============minHeap===============//
typedef struct minHeap
{
    int size;
    bool criteria;
    struct Process *arr;
}minHeap_t;

minHeap_t initializeHeap(int);

bool lessThan(struct Process a, struct Process b, bool criteria);

bool isEmptyHeap(struct minHeap*);

void push(minHeap_t*,process_t);

void heapify(minHeap_t*,int);

struct Process* getMin(struct minHeap*);

void pop(minHeap_t*);

void printHeap(minHeap_t*);

void destroyHeap(minHeap_t* heap);

//===============Semaphores===============//
int createMessageQueue()
{
    key_t key_id;
    int msgid;

    key_id = ftok("processes.txt", 65);
    msgid = msgget(key_id, 0666 | IPC_CREAT);

    if (msgid == -1)
    {
        perror("there error in creates a queue !");
        exit(-1);
    }
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


union Semun
{
    int val;
    struct semid_ds *buf;
    unsigned short *array;
    struct seminfo *__buf;
};

// void down(int sem)
// {
//     struct sembuf op;

//     op.sem_num = 0;
//     op.sem_op = -1;
//     op.sem_flg = !IPC_NOWAIT;

//     if (semop(sem, &op, 1) == -1)
//     {
//         perror("Error in down()");
//         exit(-1);
//     }
// }

// void up(int sem)
// {
//     struct sembuf op;

//     op.sem_num = 0;
//     op.sem_op = 1;
//     op.sem_flg = !IPC_NOWAIT;

//     if (semop(sem, &op, 1) == -1)
//     {
//         perror("Error in up()");
//         exit(-1);
//     }
// }

// int Creatsem(int *sem2)
// {

//     key_t keyid_sem1 = ftok("file.txt", 62);
//     key_t keyid_sem2 = ftok("file.txt", 63);
//     *sem2 = semget(keyid_sem2, 1, 0666 | IPC_CREAT);
//     int sem1 = semget(keyid_sem1, 1, 0666 | IPC_CREAT);
//     if (sem1 == -1 || *sem2 == -1)
//     {
//         perror("Error in create sem");
//         exit(-1);
//     }
//     return sem1;
// }

process_t* createProcess(int processInfo[])
{
    process_t* P=(process_t*)malloc(sizeof(process_t));
    P->ID = processInfo[0];
    P->AT = processInfo[1];
    P->RT = processInfo[2];
    P->RemT = processInfo[2];
    P->priority = processInfo[3];
    P->state = WAITING;
    P->WT=0;
    return P;
}

void printProcess(process_t* temp)
{
    if(temp!=NULL)
    printf("ID: %d, AT: %d, RT: %d, Priority: %d, RemT: %d, ", temp->ID, temp->AT, temp->RT, temp->priority, temp->RemT);
    switch (temp->state)
    {
    case FINISHED:
        printf("State: FINISHED\n");
        break;
    
    case WAITING:
        printf("State: WAITING\n");
        break;
    
    case RESUMED:
        printf("State: RESUMED\n");
        break;
    
    case STOPPED:
        printf("State: STOPPED\n");
        break;
    
    case ARRIVED:
        printf("State: ARRIVED\n");
        break;
            
    default:
        printf("State: STARTED\n");
        break;
    }
}

minHeap_t initializeHeap(int crit)
{
    minHeap_t heap;
    heap.size = 0;
    heap.criteria = crit;
    return heap;
}

bool lessThan(process_t a, process_t b, bool criteria)
{
    if (criteria == 0)
        return (a.RemT < b.RemT); //For SRTN
    else
        return (a.priority < b.priority); //For Non Pre-emitive HPF
}

bool isEmptyHeap(minHeap_t* heap)
{
    return heap->size == 0;
}

void push(minHeap_t* heap,process_t P)
{
    heap->arr = (heap->size == 0) ? malloc(sizeof(process_t)) : realloc(heap->arr, (heap->size + 1) * sizeof(process_t));
    process_t newProcess = P;
    int i = (heap->size)++;
    while (i > 0 && lessThan(newProcess, heap->arr[(i - 1) / 2], heap->criteria))
    {
        heap->arr[i] = heap->arr[(i - 1) / 2];
        i = (i - 1) / 2;
    }
    heap->arr[i] = newProcess;
}

void heapify(minHeap_t* heap, int i)
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
        process_t temp = heap->arr[i];
        heap->arr[i] = heap->arr[smallest];
        heap->arr[smallest] = temp;
        heapify(heap, smallest);
    }
}

process_t* getMin(minHeap_t* heap)
{
    return (heap->size!=0)? &heap->arr[0] : NULL;
}

void pop(minHeap_t* heap)
{
    if (heap->size == 0)
        return;
    process_t temp = heap->arr[0];
    heap->arr[0] = heap->arr[heap->size - 1];
    heap->size--;
    heapify(heap, 0);
}

void printHeap(minHeap_t* heap)
{
    for (int i = 0; i < heap->size; i++)
    {
        printProcess(&heap->arr[i]);
    }
}

void destroyHeap(minHeap_t* heap)
{
    free(heap->arr);
    heap->size = 0;
}

bool isEmpty(queue_t *q)
{
    return q->front == NULL;
}

void initializeQueue(queue_t* q)
{
    q->front = q->rear = NULL;
    q->count=0;
}

void enqueue(queue_t* q,process_t x)
{
    node_t* temp = (node_t*)malloc(sizeof(node_t));
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

bool dequeue(queue_t* q, process_t** p)
{
    if (q->front == NULL)
    {
        *p = NULL;
        return false;
    }

    node_t* temp = q->front;
    q->front = q->front->next;
    //IT was the only node in the queue
    if (q->front == NULL)
    {
        q->rear = NULL;
    }
    --(q->count);

    // Allocate memory for p and copy the data from temp->data
    if(*p == NULL)
    *p = (process_t*)malloc(sizeof(process_t));
    **p = temp->data;
    free(temp);

    return true;
}

void printQueue(queue_t* q)
{
    node_t* temp = q->front;
    while (temp != NULL)
    {
        printf("ID: %d, AT: %d, RT: %d, Priority: %d, RemT: %d, state: %d\n", temp->data.ID, temp->data.AT, temp->data.RT, temp->data.priority, temp->data.RemT, temp->data.state);
        temp = temp->next;
    }
}

void destroyQueue(queue_t* q)
{
    process_t* p;
    while (dequeue(q, &p))
    {
        free(p);
    }
}
