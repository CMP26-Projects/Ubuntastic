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
    exit(0);
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

struct Process *ReadProcessInfo(int shmid, void *shmaddr)
{
    struct Process *P = (struct Process *)malloc(sizeof(struct Process));

    // printf("\nReader: Shared memory attached at address %p\n", shmaddr);

    P = (struct Process *)shmaddr;

    printf("the message reseved in : %d %d %d %d %d %d \n", P->AT, P->RT, P->ID, P->Priority, P->RemT, P->state);

    strcpy((char *)shmaddr, "quit");
    // shmdt(shmaddr);

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
    printf("hi from sceduler ");
    // initiate Clock
    initClk();
    int x = getClk();
    printf("Scheduler current time is %d\n", x);

    // =======Schedular Attribute==========//

    struct Process *ActivatedProcess = NULL; // pointer on the ruccing Process
    struct Queue ProcessQueue;               // receved Process from process generator
    struct Queue RR;
    //===========schedular attributes========//
    int timeslice;
    int numProcesses = atoi(argv[1]); // total num of process in the CPU
    int type = atoi(argv[2]);
    int switchTime = atoi(argv[3]); // Type of schedular

    if (argc == 5)
        timeslice = atoi(argv[4]);

    signal(SIGINT, HandlerINT);
    signal(SIGCHLD, HandlerCHILD);
    // printf("hi from sceduler ");

    int Pid = fork();
    if (Pid != 0)
    {

        struct QNode *node;
        struct Process *P;

        while (1)
        {
            if (!isEmpty(&ProcessQueue))
            {
                sleep(2);
                node = ProcessQueue.front;
                P = &node->data;

                int Processid = fork();
                if (Processid == -1)
                    perror("there is an error in forking ");

                if (Processid == 0)
                {
                    //
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
                    dequeue(&ProcessQueue, &P);
                }
                sleep(5);
                printf("the process receved : ID :%d  AT: %d\n", P->ID, P->AT);
            }
        }
    }
    else
    {
        // int sem2;
        // int sem1 = Creatsem(&sem2);
        // // int msgid = createMessageQueue();
        // // type = getSchedularType(msgid);
        // int shmid = creatShMemory();
        int msgid = createMessageQueue();

        struct Process *P;
        // void *shmaddr = shmat(shmid, (void *)0, 0);
        // if (shmaddr == (void *)-1)
        // {
        //     perror("Error in attach in reader");
        //     exit(-1);
        // }
        struct msgbuf revievingProcess;
        while (1)
        {
            printf("el7a2ona tany\n");

            int msgReciver = msgrcv(msgid, &revievingProcess, sizeof(revievingProcess), 0, !IPC_NOWAIT);
            //printf("sah\n");
            // down(sem2);
            //printf("sah\n");
            // memcpy(P, shmaddr, sizeof(struct Process));
            printf("I am herer");
            // memcpy(P, revievingProcess.msgProcess, sizeof(struct Process));
            printf("bla bla lolo\n");

            enqueue(&ProcessQueue, *P);
            // up(sem1);
        }
    }

    // Signals Handlers

    /// get type of schedular from process_generator using message queue

    // TODO implement the scheduler :)
    // upon termination release the clock resources.

    // destroyClk(true);
    return 0;
}