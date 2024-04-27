#include <string.h>
#include <signal.h>
#include "headers.h"
int msgid;
queue_t ProcessQueue;
void clearResources(int);
int readFile(char*,queue_t*);
void getUserInput(int*, int*); //Will get updated and support GUI 
msgbuf createProcessMessage(process_t*); //Create a message of the process data to send it to the scheduler

int main(int argc, char *argv[])
{
    int schedAlgo,timeSlice=-1;

    signal(SIGINT, clearResources); //If it gets interrupted, clear the resources 

    //Read the input files.
    int numProcesses = readFile(argv[1], &ProcessQueue);
    //Ask the user for the chosen scheduling algorithm and its parameters, if there are any.
    getUserInput(&schedAlgo, &timeSlice);
    
    printf("Number of Processes: %d\n", numProcesses);
    //Initiate and create clock process.    
    pid_t clk = fork();
    if (clk == -1)
    {
        perror("Error in Forking the Clock\n");
        exit(-1);
    }
    else if (clk == 0)
    {
        char *args[] = {"clk.out",(char *)NULL};
        execv(realpath(args[0],NULL),args);
        perror("Execl process has failed for creating the clock\n");
        exit(-1);
    }
    else
    {
        //Initialize the clock
        initClk();
        //Initiate and create scheduler process.    
        pid_t scheduler = fork();
        if (scheduler == -1)
        {
            perror("Error in Forking the scheduler\n");
            exit(-1);
        }
        else if (scheduler == 0)
        {
            //Read the scheduler input to send it
            char n[5], s[5], sw[5], t[5];
            sprintf(n, "%d", numProcesses);
            sprintf(s, "%d", schedAlgo);
            sprintf(t, "%d", timeSlice);
            char *args[] = {"scheduler.out", n, s, t, (char *)NULL};
            execv(realpath(args[0],NULL),args);
            perror("Execl process has failed for creating the scheduler\n");
            exit(-1);
        }
        else
        {
            //Create the message queue to send the process info to the scheduler
            msgid = createMessageQueue();
            process_t* P=NULL;
            msgbuf sendingProcess;
            while (!isEmpty(&ProcessQueue)) //If there any remainig processes send them at the appropriate time
            {
                clk_t currTimeStamp = getClk();
                P = &ProcessQueue.front->data;
                if(P->AT<=currTimeStamp)
                {
                    sendingProcess=createProcessMessage(P);
                    int msgSending = msgsnd(msgid, &sendingProcess, sizeof(sendingProcess.data), IPC_NOWAIT);
                    if(msgSending==-1)
                        printf("there is an error in sending");
                    printf("Process %d has sent to the scheduler succesfully at time clock %d \n",P->ID,currTimeStamp);
                    dequeue(&ProcessQueue, &P);
                }
            }
            //Waits for the scheduler to terminate
            waitpid(scheduler,NULL,0);
        }
    }
}


int readFile(char* filePath, queue_t* Pqueue)
{
    int numProcesses = 0;
    //Initialize the Process Queue
    initializeQueue(Pqueue);

    int id, runTime, arrivalTime, priority;
    char dummy[100];
    FILE *inputFile = fopen(filePath, "r");
    if (!inputFile)
    {
        perror("Error in opening the processes.txt file\n");
        return -1;
    }
    fgets(dummy, sizeof(dummy), inputFile); //Ignoring the first line

    while (fscanf(inputFile, "%d\t%d\t%d\t%d\n", &id, &arrivalTime, &runTime, &priority) == 4)
    {
        //Read the process data and creating a process 
        int info[]={id,arrivalTime,runTime,priority};
        process_t* pd = createProcess(info);
        enqueue(Pqueue, *pd);
        numProcesses++;
    }
    fclose(inputFile);
    //Testing
    printf("Number of processes: %d\n", numProcesses);
    return numProcesses;
}


void getUserInput(int *schedAlgo, int *timeSlice) //Will get updated and support GUI 
{
    Get_Scheduling_Algorithm: 
    system("clear"); //Clear the terminal
    printf("|| Choose a Scheduling Algorithm ||\n");
    printf("For SRTN, Enter 0\nFor HPF, Enter 1\nFor RR, Enter 2\nAlgorithm: ");
    scanf("%d", schedAlgo);
    if (*schedAlgo> 2)
    {
        printf("Invalid Option ->\n");
        sleep(1); 
        goto Get_Scheduling_Algorithm;
    }


    if(*schedAlgo == 2)
    {
        //For RR get the time slice
        Get_RR_timeSlice:
            printf("||Enter the Time Slice||\nTime: ");
            scanf("%d", timeSlice);
            if (*timeSlice < 0)
            {
                printf("\nInvalid Option -> ");
                goto Get_RR_timeSlice;
            }
    }
    system("clear");
}

msgbuf createProcessMessage(process_t* P)
{
    msgbuf sendingProcess;
    sendingProcess.mtype = 7;
    sendingProcess.data[0] = P->ID;
    sendingProcess.data[1] = P->AT;
    sendingProcess.data[2] = P->RT;
    sendingProcess.data[3] = P->priority;
    return sendingProcess;
}

void clearResources(int signum)
{
    //Delete the processes shared memory
    msgctl(msgid, IPC_RMID, (struct msqid_ds *)0);
    destroyQueue(&ProcessQueue);
    signal(SIGINT,clearResources);
}
