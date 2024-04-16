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
    printf("queue size is : %d \n", Pqueue->count);

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
    return numProcesses;
}

void getUserInput(int *schedAlgo, int *switchTime, int *timeSlice)
{
    
    Get_Scheduling_Algorithm:
    system("clear");
    printf("|| Choose a Scheduling Algorithm ||\n");
    printf("For RR, Enter 0\nFor SRTF, Enter 1\nFor HPF, Enter 2\nAlgorithm: ");
    scanf("%d", schedAlgo);
    printf("schedAlgo: %d\n", *schedAlgo);
    if (*schedAlgo> 2)
    {
        printf("Invalid Option ->\n");
        sleep(1);
        goto Get_Scheduling_Algorithm;
    }

    Get_Context_Switshing_Time:
    printf("|| Enter the Context Switching Time ||\nTime: ");
    scanf("%d", switchTime);
    if (*switchTime < 0)
    {
        printf("\nInvalid Option -> ");
        goto Get_Context_Switshing_Time;
    }

    if(*schedAlgo == 0)
    {
        Get_RR_timeSlice:
            printf("||Enter the Time Slice||\nTime: ");
            scanf("%d", timeSlice);
            if (*timeSlice < 0)
            {
                printf("\nInvalid Option -> ");
                goto Get_RR_timeSlice;
            }
    }
    if(*schedAlgo!=0)
        printf("schedAlgo: %d\nswitchTime: %d\n", *schedAlgo, *switchTime);
    else
        printf("schedAlgo: %d\nswitchTime: %d\ntimeSlice: %d\n", *schedAlgo, *switchTime, *timeSlice);
}

void createProcessMessage(struct msgbuf sendingProcess, struct Process *P)
{
    sendingProcess.mtype = 1;
    sendingProcess.data[0] = P->AT;
    sendingProcess.data[1] = P->ID;
    sendingProcess.data[2] = P->RT;
    sendingProcess.data[3] = P->RemT;
    sendingProcess.data[4] = P->state;
    sendingProcess.data[5] = P->Priority;
}

int main(int argc, char *argv[])
{
    int sem2,schedAlgo,switchTime,timeSlice;
    int sem1 = Creatsem(&sem2);
    struct schdularType schedularmessage;
    struct Queue ProcessQueue;
    initializeQueue(&ProcessQueue);

    signal(SIGINT, clearResources);

    // 1. Read the input files.
    int numProcesses = readFile(argv[1], &ProcessQueue);

    // 2. Ask the user for the chosen scheduling algorithm and its parameters, if there are any.
    getUserInput(&schedAlgo, &switchTime, &timeSlice);
    

    // 3. Initiate and create the scheduler and clock processes.
    printf("I'm the Generator Process and I will fork the clock\n"); // Test, To be deleted.
    
    pid_t clk = fork();
    if (clk == -1)
    {
        perror("Error in Forking the Clock Process.\n");
        exit(-1);
    }
    else if (clk == 0)
    {
        printf("I am the clock processes and I will be created now\n");
        system("gcc clk.c -o clk.out");      // Compiling the clock file
        execl("clk.out", "./clk.out", NULL); // exchange the current process with the clock's

        perror("Execl process has failed\n");
        exit(-1);
    }
    else if(clk < 0)
    {
        //  4. Use this function after creating the clock process to initialize clock
        initClk();

        printf("I'm the Generator Process and I will fork the scheduler\n"); // Test, To be deleted.
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
            //  4. Use this function after creating the clock process to initialize clock To get time use this
            int cuurTimeStamp = getClk();
            printf("I'm the process_generator and the current time is %d\n", cuurTimeStamp);

            int msgid = createMessageQueue();

            // processesShmAddr = shmat(processesShmID, (void *)0, 0);
            // if (processesShmAddr == (void *)-1)
            // {
            //     perror("Error in attach in process_generator");
            //     exit(-1);
            // }

            printf("\nProcess_generator: Shared memory attached at address %d\n", processesShmID);
            
            
            // 6. Send the information to the scheduler at the appropriate time.
            struct Process *P=NULL;
            struct msgbuf sendingProcess;
            while (!isEmpty(&ProcessQueue))
            {
                dequeue(&ProcessQueue, &P);
                int currTimeStamp = getClk();
                printf("current time is %d\n", currTimeStamp);

                while (P->AT == currTimeStamp)
                {
                    printf("process %d will be sent to the scheduler \n",P->ID);
                    createProcessMessage(sendingProcess,P);
                    int msgSending = msgsnd(msgid, &sendingProcess, sizeof(sendingProcess), !IPC_NOWAIT);
                    printf("process %d was sent to the scheduler successfully \n",P->ID);
                    dequeue(&ProcessQueue, &P);
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
