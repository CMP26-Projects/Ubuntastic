// Author: Somia
//=============Contents============//
// readFile()
// clearResources
// Scheduler Forking
// Clock Forking
//=================================//

#include <string.h>
#include <signal.h>
#include "headers.h"
void clearResources(int);
void *processesShmAddr;
int processesShmID;
//=========Read File=============//
int readFile(char *filePath, struct Queue *Pqueue)
{
    int numProcesses = 0;
    initializeQueue(Pqueue);
    int id, bt, at, pri;
    char dummy[100];
    FILE *inputFile = fopen(filePath, "r");
    if (!inputFile)
    {
        printf("Error in opening the file\n");
        return -1;
    }
    fgets(dummy, sizeof(dummy), inputFile); // ignoring the first line

    while (fscanf(inputFile, "%d\t%d\t%d\t%d\n", &id, &at, &bt, &pri) == 4)
    {
        struct Process pd = {id, bt, at, pri, bt};
        enqueue(Pqueue, pd);
        numProcesses++;
    }
    fclose(inputFile);
    printf("Number of Processes: %d\n", numProcesses);
    struct QNode *node = Pqueue->front;
    struct Process *P = &node->data;
    printf("THE NUM OF PROCESSESS IS %d\n", numProcesses);

    // while (!isEmpty(Pqueue))
    // {
    //     printf("the message reseved in : %d %d %d %d %d\n", P->AT, P->ID, P->Priority, P->RemT, P->state);
    //     dequeue(Pqueue, P);
    //     node = node->next;
    //     P = &node->data;
    // }
    // node = Pqueue->front;
    // P = &node->data;
    // dequeue(Pqueue, P);

    return numProcesses;
}

int main(int argc, char *argv[])
{
    int sem2;
    int sem1 = Creatsem(&sem2);
    struct schdularType schedularmessage;
    struct Queue ProcessQueue;
    signal(SIGINT, clearResources);

    // 1. Read the input files.
    int numProcesses = readFile(argv[1], &ProcessQueue);

    // 2. Ask the user for the chosen scheduling algorithm and its parameters, if there are any.
    int schedAlgo = -1;
    int switchTime = -1;
    int timeSlice = -1; // RR

    do
    {
        printf("|| Choose a Scheduling Algorithm ||\n");
        printf("For RR, Enter 0\nFor SRTF, Enter 1\nFor HPF, Enter 2\nAlgorithm: ");
        scanf("%d", &schedAlgo);
        if (schedAlgo != 0 && schedAlgo != 1 && schedAlgo != 2)
            printf("Invalid Option -> ");
    } while (schedAlgo != 0 && schedAlgo != 1 && schedAlgo != 2);

    do
    {
        printf("|| Enter the Context Switching Time ||\nTime: ");
        scanf("%d", &switchTime);
        if (switchTime < 0)
            printf("\nInvalid Option -> ");
    } while (switchTime < 0);

    switch (schedAlgo)
    {
    case 0:
    {
        printf("You've Chosen RR.\n");
        do
        {
            printf("||Enter the Time Slice||\nTime: ");
            scanf("%d", &switchTime);
            if (switchTime < 0)
                printf("\nInvalid Option -> ");
        } while (switchTime < 0);
    }
    break;
    default:
        break;
    }
    printf("hina\n");
    // 3. Initiate and create the scheduler and clock processes.
    pid_t clk = fork();
    if (clk == -1)
    {
        perror("Error in Forking the Clock Process.\n");
        exit(-1);
    }
    else if (clk == 0)
    {
        // printf("I'm the Clock Process\n"); //<TEST>
        printf("I am the clock processes and I will be created now\n");
        system("gcc clk.c -o clk.out");      // Compiling the clock file
        execl("clk.out", "./clk.out", NULL); // exchange the current process with the clock's

        perror("Execl process has failed\n");
        exit(-1);
    }
    else
    {
        initClk();
        // printf("I'm the Generator Process and I will fork the scheduler\n"); // Test, To be deleted.
        // //  4. Use this function after creating the clock process to initialize clock

        pid_t scheduler = fork();
        if (scheduler == -1)
        {
            perror("Error in Forking the Scheduler Process.\n");
            exit(-1);
        }
        else if (scheduler == 0)
        {
            printf("I'm the scheduler Processe and I will be created now\n"); //<TEST>
            system("gcc scheduler.c -o scheduler.out");                       // Compiling the scheduler file
            char n[5], s[5], sw[5], t[5];
            sprintf(n, "%d", numProcesses);
            sprintf(s, "%d", schedAlgo);
            sprintf(sw, "%d", switchTime);
            sprintf(t, "%d", timeSlice);
            if (schedAlgo == 0)
            {
                execl("scheduler.out", "./scheduler.out", n, s, sw, t, (char *)NULL); // exchange the current process with the scheduler's & passing it required Arguments
            }
            else
            {
                execl("scheduler.out", "./scheduler.out", n, s, sw, (char *)NULL);
            }
            perror("Execl process has failed for creating the scheduler\n");
            exit(-1);
        }
        else
        {
            //  4. Use this function after creating the clock process to initialize clock
            // To get time use this
            // sleep(5);
            int x = getClk();
            printf("I'm the process_generator and the current time is %d\n", x);

            int msgid = createMessageQueue();

            // processesShmAddr = shmat(processesShmID, (void *)0, 0);
            // if (processesShmAddr == (void *)-1)
            // {
            //     perror("Error in attach in process_generator");
            //     exit(-1);
            // }

            printf("\nProcess_generator: Shared memory attached at address %d\n", processesShmID);
            struct QNode *node;
            struct Process *P;
            // 6. Send the information to the scheduler at the appropriate time.
            struct msgbuf sendingProcess;
            while (!isEmpty(&ProcessQueue))
            {
                node = ProcessQueue.front;
                P = &node->data;
                int x = getClk();
                printf("current time is %d\n", x);
                printf("el7a2ona ba2a\n");

                while (P->AT == x)
                {
                    printf("el7a2ona\n");
                    sendingProcess.data[0] = P->AT;
                    sendingProcess.data[1] = P->ID;
                    sendingProcess.data[2] = P->RT;
                    sendingProcess.data[3] = P->RemT;
                    sendingProcess.data[4] = P->state;
                    sendingProcess.data[5] = P->Priority;

                    int msgSending = msgsnd(msgid, &sendingProcess, sizeof(sendingProcess), !IPC_NOWAIT);
                    printf("bla bla\n");
                    dequeue(&ProcessQueue, P);
                    printf("%d", ProcessQueue.count);
                    node = ProcessQueue.front;
                    P = &node->data;
                }
            }
        }
    }
}
void clearResources(int signum) // may not be complete. will be edited if something is missing <MAY BE EDITED>
{
    // TODO Clears all resources in case of interruption
    shmdt(processesShmAddr);
    shmctl(processesShmID, IPC_RMID, NULL);
    printf("\nebl3\n"); //<TEST>
    destroyClk(true);
    killpg(getpgrp(), SIGKILL);
    exit(-1);
}
