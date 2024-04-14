// Author: Somia
//=============Contents============//
// readFile()
// clearResources()
// Scheduler Forking

#include "headers.h"
#include <string.h>
void clearResources(int);

//=========Read File=============//
int readFile(char *filePath, struct Queue *Process)
{
    int numProcesses = 0;
    initializeQueue(Process);
    int id, bt, at, pri;
    char dummy[100];
    FILE *inputFile = fopen(filePath, "r");
    if (!inputFile)
    {
        printf("Error in opening the file\n");
        return -1;
    }
    fgets(dummy, sizeof(dummy), inputFile); // ignoring the first line

    while (fscanf(inputFile, "%d\t%d\t%d\t%d\n", &id, &bt, &at, &pri) == 4)
    {
        struct Process pd = {id, bt, at, pri, bt};
        enqueue(Process, pd);
        numProcesses++;
    }
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
    printf("hina");
    // 3. Initiate and create the scheduler and clock processes.
    pid_t scheduler = fork();
    if (scheduler == -1)
        perror("Error in Forking the Scheduler Process.\n");
    else if (scheduler == 0)
    {
        // printf("I'm the scheduler Process\n"); //<TEST>
        char n[5], s[5], sw[5], t[5];
        printf("%d", numProcesses);
        printf("%d", schedAlgo);
        printf("%d", switchTime);
        system("gcc scheduler.c -o scheduler.out"); // Compiling the scheduler file
        if (schedAlgo == 0)
        {
            printf(t, "%d", timeSlice);
            execl("scheduler.out", "./scheduler.out", n, s, sw, t, NULL); // exchange the current process with the scheduler's & passing it required Arguments
        }
        else
        {
            execl("scheduler.out", "./scheduler.out", n, s, sw, NULL);
        }
    }
    else
    {

        int shmid = creatShMemory();

        void *shmaddr = shmat(shmid, NULL, 0);
        if (shmaddr == (void *)-1)
        {
            perror("Error in attach in writer");
            exit(-1);
        }

        printf("\nWriter: Shared memory attached at address %d\n", shmid);
        struct QNode *node;
        struct Process *P;
        while (1)
        {
            if (!isEmpty(&ProcessQueue))
            {
                node = ProcessQueue.front;
                P = &node->data;
                if (P->AT == getClk())
                {
                    printf("send");
                    memcpy(shmaddr, P, sizeof(struct Process));
                    up(sem2);
                    dequeue(&ProcessQueue, P);
                    down(sem1);
                }
            }
        }

        shmdt(shmaddr);
    }

    int x;
    wait(&x); //<TEST>

    // printf("I'm the Generator Process\n"); //Test, To be deleted.
    //  4. Use this function after creating the clock process to initialize clock

    //  initClk();
    // To get time use this
    // int x = getClk();
    // printf("current time is %d\n", x);
    // TODO Generation Main Loop
    // 5. Create a data structure for processes and provide it with its parameters.
    // 6. Send the information to the scheduler at the appropriate time.
    // 7. Clear clock resources
    // destroyClk(true);
}

void clearResources(int signum) // may not be complete. will be edited if something is missing <MAY BE EDITED>
{
    // TODO Clears all resources in case of interruption
    destroyClk(true);
    signal(SIGINT, clearResources); // <Q> i think it cannot be after the raise call.. mot sure yet, will find out later :D
    printf("ebl3\n");               //<TEST>
    raise(SIGKILL);
}
