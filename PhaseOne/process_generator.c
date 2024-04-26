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
    printf("queue size is : %d \n", Pqueue->count);
    printf("Number of Processes: %d\n", numProcesses);
    return numProcesses;
}

void getUserInput(int *schedAlgo, int *switchTime, int *timeSlice)
{
    
    Get_Scheduling_Algorithm:
    system("clear");
    printf("|| Choose a Scheduling Algorithm ||\n");
    printf("For SRTN, Enter 0\nFor HPF, Enter 1\nFor RR, Enter 2\nAlgorithm: ");
    scanf("%d", schedAlgo);
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

    if(*schedAlgo == 2)
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
}

struct msgbuf  createProcessMessage(struct Process *P)
{
    struct msgbuf sendingProcess;
    sendingProcess.mtype = 7;
    sendingProcess.data[0] = P->ID;
    sendingProcess.data[1] = P->AT;
    sendingProcess.data[2] = P->RT;
    sendingProcess.data[3] = P->Priority;
    return sendingProcess;
}

int main(int argc, char *argv[])
{
    int sem2,schedAlgo,switchTime,timeSlice=-1;
    int sem1 = Creatsem(&sem2);
    struct Queue ProcessQueue;

    signal(SIGINT, clearResources);

    // 1. Read the input files.
    int numProcesses = readFile(argv[1], &ProcessQueue);
    // 2. Ask the user for the chosen scheduling algorithm and its parameters, if there are any.
    getUserInput(&schedAlgo, &switchTime, &timeSlice);
    
    printf("Number of Processes: %d\n", numProcesses);

    // 3. Initiate and create the scheduler and clock processes.    
    pid_t clk = fork();
    if (clk == -1)
    {
        perror("Error in Forking the Clock Process.\n");
        exit(-1);
    }
    else if (clk == 0)
    {
        char command[200];
        execl("clk.out", "./clk.out", NULL); // exchange the current process with the clock's
        perror("Execl process has failed\n");
        exit(-1);
    }
    else
    {
        //  4. Use this function after creating the clock process to initialize clock
        initClk();
        clk_t time = getClk();
        pid_t scheduler = fork();
        if (scheduler == -1)
        {
            perror("Error in Forking the Scheduler Process.\n");
            exit(-1);
        }
        else if (scheduler == 0)
        {
            char n[5], s[5], sw[5], t[5];
            sprintf(n, "%d", numProcesses);
            sprintf(s, "%d", schedAlgo);
            sprintf(sw, "%d", switchTime);
            sprintf(t, "%d", timeSlice);
            char *args[] = {"scheduler.out", n, s, sw, t, (char *)NULL};
            execv(realpath(args[0],NULL),args);
            perror("Execl process has failed for creating the scheduler\n");
            exit(-1);
        }
        else
        {
            //  4. Use this function after creating the clock process to initialize clock To get time use this
            int msgid = createMessageQueue();
            // 6. Send the information to the scheduler at the appropriate time.
            struct Process *P=NULL;
            struct msgbuf sendingProcess;
            while (!isEmpty(&ProcessQueue))
            {
                printf("Time in generator: %d\n",getClk());
                int currTime=getClk();
                int currTimeStamp = getClk();
                P = &ProcessQueue.front->data;
                if(P->AT==currTimeStamp)
                {
                    printf("process is being sent");
                    sendingProcess=createProcessMessage(P);
                    int msgSending = msgsnd(msgid, &sendingProcess, sizeof(sendingProcess.data), IPC_NOWAIT);
                    dequeue(&ProcessQueue, &P);
                }
                while(getClk()==currTime){}
            }
        }
    }   
    return 0;
}
void clearResources(int signum) // may not be complete. will be edited if something is missing <MAY BE EDITED>
{
    // TODO Clears all resources in case of interruption
    shmdt(processesShmAddr);
    shmctl(processesShmID, IPC_RMID, NULL);
    destroyClk(true);
    exit(-1);
}
