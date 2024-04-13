#include "headers.h"
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

void HandlerINT(int signum)
{
}
void HandlerCHILD(int signum)
{
}
void generateLogFile()
{
}
void generatePrefFile()
{
}
void createProcess()
{
}
void getState()
{
}
void updateOutfile()
{
}

struct Process *ReadProcessInfo(int shmid)
{
    struct Process *P = (struct Process *)malloc(sizeof(struct Process));

    void *shmaddr = shmat(shmid, (void *)0, 0);
    if (shmaddr == (void *)-1)
    {
        perror("Error in attach in reader");
        exit(-1);
    }

    printf("\nReader: Shared memory attached at address %p\n", shmaddr);

    P = (struct Process *)shmaddr;

    printf("the message reseved in : %d %d %d %d  \n", P->AT, P->BT, P->ID, P->Priority);

    strcpy((char *)shmaddr, "quit");
    shmdt(shmaddr);
    return P;
}

int getSchedularType(int msgid)
{
    struct schdularType schedularmessage;
    int done = msgrcv(msgid, &schedularmessage, sizeof(schedularmessage.schedType), 0, !IPC_NOWAIT);

    if (done == -1)
    {
        perror("error in reciving ?");
    }
    else
    {
        printf("the message reseved in : %d\n", schedularmessage.schedType);
    }
    msgctl(msgid, IPC_RMID, (struct msqid_ds *)0);
    return schedularmessage.schedType;
}

int main(int argc, char *argv[])
{
    // =======Schedular Attribute==========//

    int totalProcessNum = 0;                 // total num of process in the CPU
    int type;                                // Type of schedular
    struct Process *ActivatedProcess = NULL; // pointer on the ruccing Process
    struct Queue ProcessQueue;               // receved Process from process generator
    struct Queue RR;

    signal(SIGINT, HandlerINT);
    signal(SIGCHLD, HandlerCHILD);
    int Pid = fork();
    if (Pid != 0)
    {
        // initiate Clock
        initClk();

        struct QNode *node;
        struct Process *P;

        while (1)
        {
            if (!isEmpty(&ProcessQueue))
            {
                node = ProcessQueue.front;
                P = &node->data;

                int Processid = fork();
                if (Processid == -1)
                    perror("there is an error in forking ");

                if (Processid == 0)
                {
                    // execl("/process","process",P->BT,NULL);
                }
                else
                {
                    if (type == 1)
                    {
                        // enqueue(&ProcessQueue, *P);
                    }
                    else if (type == 2)
                    {
                        // enqueue(&ProcessQueue, *P);
                    }
                    else
                    {
                        // enqueue(&ProcessQueue, *P);
                    }
                    dequeue(&ProcessQueue, P);
                }

                printf("the process receved : ID :%d  AT: %d\n", P->ID, P->AT);
            }
        }
    }
    else
    {
        int sem2;
        int sem1 = Creatsem(&sem2);
        // int msgid = createMessageQueue();
        // type = getSchedularType(msgid);
        int shmid = creatShMemory();

        struct Process *P;

        while (1)
        {
            down(sem2);
            P = ReadProcessInfo(shmid);
            enqueue(&ProcessQueue, *P);
            up(sem1);
        }
    }

    // Signals Handlers

    /// get type of schedular from process_generator using message queue

    // TODO implement the scheduler :)
    // upon termination release the clock resources.

    // destroyClk(true);
    return 0;
}