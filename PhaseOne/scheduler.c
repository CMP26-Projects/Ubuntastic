#include "headers.h"
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

void handlerINT(int signum)
{
    exit(0);
    // destroyClk(true);
}
void handlerCHILD(int signum)
{
}
void stopProcess(int pid)
{
    kill(pid, SIGSTOP);
}
void continueProcess(int pid)
{
    kill(pid, SIGCONT);
}
void generateLogFile()
{
    exit(0);
}
void generatePrefFile()
{
}
void startProcess()
{
}
void getState()
{
}
void updateOutfile(struct Process *P, int state)
{

}




void SRTN_Algo(int numProcesses, int switchTime){
    printf("SRTN\n");
    struct minHeap PQ=initializeHeap(SRTN);
    struct Process P;
    int msgid = createMessageQueue();
    struct msgbuf revievingProcess;
    while (1)
    {
        int msgReciver = msgrcv(msgid, &revievingProcess, sizeof(revievingProcess.data), 7, !IPC_NOWAIT);
        P = createProcess(revievingProcess.data);
        push(&PQ, P);
    }
    printf("the heap is : \n");
    printHeap(&PQ);
    // while (!isEmptyHeap(&PQ))
    // {
    //     P = getMin(&PQ);
    //     int Processid = fork();
    //     if (Processid == -1)
    //         perror("there is an error in forking ");
    //     if (Processid == 0)
    //     {
    //         // execl("/process","process",P->BT,NULL);
    //     }
    //     else
    //     {
    //         pop(&PQ);
    //     }
    // }
    printf("the scheduler has finished in time : %d\n", getClk());
    destroyHeap(&PQ);
}

void HPF_Algo(int numProcesses, int switchTime){
    printf("HPF\n");
    struct minHeap PQ=initializeHeap(HPF);
    struct Process P;
    int msgid = createMessageQueue();
    struct msgbuf revievingProcess;
    printf("the number of processes is : %d\n", numProcesses);
    while (1)
    {
        int msgReciver = msgrcv(msgid, &revievingProcess, sizeof(revievingProcess.data), 7, !IPC_NOWAIT);
        P = createProcess(revievingProcess.data);
        push(&PQ, P);
        printProcess(PQ.arr[(P.ID)-1]);
    }
    // while (!isEmptyHeap(&PQ))
    // {
    //     P = getMin(&PQ);
    //     int Processid = fork();
    //     if (Processid == -1)
    //         perror("there is an error in forking ");
    //     if (Processid == 0)
    //     {
    //         char *args[] = {"process.out", P.ID,P.RT, (char *)NULL};
    //         execv(realpath("process.out",NULL),args);
    //         perror("Execl process has failed for creating the process\n");
    //         exit(-1);
    //     }
    //     else
    //     {
    //         pop(&PQ);
    //     }
    // }
    printf("the scheduler has finished in time : %d\n", getClk());
    destroyHeap(&PQ);
}
void RR_Algo(int numProcesses, int switchTime, char *timeSlice){
    printf("RR\n");
    struct Queue ProcessQueue;
    struct Process P;
    struct msgbuf revievingProcess;
    initializeQueue(&ProcessQueue);
    int msgid = createMessageQueue();    
    printf("the number of processes is : %d\n", numProcesses);
    while (1)
    {
        int msgReciver = msgrcv(msgid, &revievingProcess, sizeof(revievingProcess.data), 7, !IPC_NOWAIT);
        P = createProcess(revievingProcess.data);
        enqueue(&ProcessQueue, P);
        // printProcess(P);
        struct Process *tmp;
        dequeue(&ProcessQueue, &tmp);
        printProcess(*tmp);
        //printf("recievedProcesses : %d\n", recievedProcesses);
        //ProcessQueue.front=ProcessQueue.front->next;
    }

    // while (!isEmpty(&ProcessQueue))
    // {
    //     struct QNode *node;
    //     struct Process *P;
    //     node = ProcessQueue.front;
    //     P = &node->data;
    //     int Processid = fork();
    //     if (Processid == -1)
    //         perror("there is an error in forking ");
    //     if (Processid == 0)
    //     {
    //         // execl("/process","process",P->BT,NULL);
    //     }
    //     else
    //     {
    //         dequeue(&ProcessQueue, &P);
    //     }
    // }

    printf("the scheduler has finished in time : %d\n", getClk());
    destroyQueue(&ProcessQueue);
}

int main(int argc, char *argv[])
{
    // initiate Clock
    initClk();
    int clock = getClk();

    // =======Initializing Scheduler========== //
    int numProcesses = atoi(argv[1]); // total num of process in the CPU
    int type = atoi(argv[2]);// type of scheduler
    int switchTime = atoi(argv[3]); 

    system("gcc process.c -o process.out");          //Testing 

    signal(SIGINT, handlerINT);
    signal(SIGCHLD, handlerCHILD);    
    printf("the scheduler has started in time slice : %d\n", clock);
    switch(type){
    case SRTN:
        SRTN_Algo(numProcesses,switchTime);
        break;
    case HPF:
        HPF_Algo(numProcesses,switchTime);
        break;
    case RR:
        RR_Algo(numProcesses,switchTime,(argv[4]));
        break;
    default:
        printf("Invalid Scheduler Type\n");
        break;
    }
    int totalElapsedtime = getClk();
    printf("the scheduler has finished in time : %d\n", totalElapsedtime);
    generatePrefFile();
    destroyClk(true);
    exit(numProcesses);
}
    // int Pid = fork();
    // if (Pid != 0)
    // {

    //     struct QNode *node;
    //     struct Process *P;

    //     while (1)
    //     {
    //         if (!isEmpty(&ProcessQueue))
    //         {
    //             sleep(2);
    //             node = ProcessQueue.front;
    //             P = &node->data;

    //             int Processid = fork();
    //             if (Processid == -1)
    //                 perror("there is an error in forking ");

    //             if (Processid == 0)
    //             {
    //                 //
    //                 // execl("/process","process",P->BT,NULL);
    //             }
    //             else
    //             {
    //                 if (type == 1)
    //                 {
    //                     // enqueue(&ProcessQueue, *P);
    //                 }
    //                 else if (type == 2)
    //                 {
    //                     // enqueue(&ProcessQueue, *P);
    //                 }
    //                 else
    //                 {
    //                     // enqueue(&ProcessQueue, *P);
    //                 }
    //                 dequeue(&ProcessQueue, &P);
    //             }
    //             sleep(5);
    //             printf("the process receved : ID :%d  AT: %d\n", P->ID, P->AT);
    //         }
    //     }
    // }
    // else
    // {
    //     // int sem2;
    //     // int sem1 = Creatsem(&sem2);
    //     // // int msgid = createMessageQueue();
    //     // // type = getSchedulerType(msgid);
    //     // int shmid = creatShMemory();
    //     int msgid = createMessageQueue();

    //     struct Process *P;
    //     // void *shmaddr = shmat(shmid, (void *)0, 0);
    //     // if (shmaddr == (void *)-1)
    //     // {
    //     //     perror("Error in attach in reader");
    //     //     exit(-1);
    //     // }
    //     struct msgbuf revievingProcess;
    //     while (1)
    //     {
    //         printf("el7a2ona tany\n");

    //         int msgReciver = msgrcv(msgid, &revievingProcess, sizeof(revievingProcess), 0, !IPC_NOWAIT);
    //         //printf("sah\n");
    //         // down(sem2);
    //         //printf("sah\n");
    //         // memcpy(P, shmaddr, sizeof(struct Process));
    //         printf("I am herer");
    //         // memcpy(P, revievingProcess.msgProcess, sizeof(struct Process));
    //         printf("bla bla lolo\n");

    //         enqueue(&ProcessQueue, *P);
    //         // up(sem1);
    //     }
    // }

    // // Signals Handlers

    // /// get type of schedular from process_generator using message queue

    // // TODO implement the scheduler :)
    // // upon termination release the clock resources.

    // // destroyClk(true);
//     return 0;
// }