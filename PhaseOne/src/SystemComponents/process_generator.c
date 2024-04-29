#include <string.h>
#include <signal.h>
#include "headers.h"
#include "processUnits.h"
#include "../dataStructures/queue.h"
int msgid;
queue_t* ProcessQueue;
void clearResources(int);
int readFile(char*,queue_t*);
void getUserInput(int*, int*); //Will get updated and support GUI 
processMsg createProcessMessage(process_t*); //Create a message of the process data to send it to the scheduler

int main(int argc, char *argv[])
{
    // int schedAlgo=atoi(argv[2]);
    // if(argc>3&&schedAlgo==RR)
    // timeSlice=atoi(argv[3]);

    msgid = createMessageQueue();
    int timeSlice,schedAlgo;
    signal(SIGINT, clearResources); //If it gets interrupted, clear the resources 

    ProcessQueue=createQueue();
    //Read the input files.
    int numProcesses = readFile(argv[1], ProcessQueue);
    //Ask the user for the chosen scheduling algorithm and its parameters, if there are any.
    getUserInput(&schedAlgo, &timeSlice);
    
    //Initiate and create clock process.    
    pid_t clk = fork();
    if (clk == -1)
    {
        perror("Error in Forking the Clock\n");
        exit(-1);
    }
    else if (clk == 0)
    {
        #ifdef DEBUG
            printf("I'm the clock\n");
        #endif
        char *args[] = {"./clk.out",(char *)NULL};
        execv(args[0],args);
        perror("Execl process has failed for creating the clock\n");
        exit(-1);
    }
    else
    {
        //Initialize the clock
        //Initiate and create scheduler process.    
        pid_t scheduler = fork();
        if (scheduler == -1)
        {
            perror("Error in Forking the scheduler\n");
            exit(-1);
        }
        else if (scheduler == 0)
        {
            #ifdef DEBUG
                printf("I'm the scheduler\n");
            #endif
            //Read the scheduler input to send it
            char n[5], s[5], sw[5], t[5];
            sprintf(n, "%d", numProcesses);
            sprintf(s, "%d", schedAlgo);
            sprintf(t, "%d", timeSlice);
            char *args[] = {"./scheduler.out", n, s, t, (char *)NULL};
            execv(args[0],args);
            perror("Execl process has failed for creating the scheduler\n");
            exit(-1);
        }
        else
        {   
            //Initiate and create the process generator process.
            initClk();
            clk_t lastClk=getClk();
            #ifdef DEBUG
                printf("I'm the generator and the Ids of scheduler and clk are %d - %d \n",clk,scheduler);
            #endif
            process_t* P=front(ProcessQueue);
            printProcess(P);
            while (true)
            {
                clk_t curTime = getClk();
                
                if(curTime==lastClk)
                    continue;

                lastClk++;

                while (P != NULL && curTime == P->AT)
                {
                    processMsg sendingProcess=createProcessMessage(P);
                    int msgSending = msgsnd(msgid, &sendingProcess, sizeof(sendingProcess.data),IPC_NOWAIT);
                    #ifdef DEBUG
                        printf("generator sent process at time clk %d\n", getClk());
                        printProcess(P);
                    #endif

                    if(msgSending==-1)
                        printf("there is an error in sending");
                    dequeue(ProcessQueue);
                    P = front(ProcessQueue);
                    printProcess(P);
                }
                    kill(scheduler, SIGUSR1);

                #ifdef DEBUG
                    printf("generator at time clk %d\n", getClk());
                #endif
            }
            //Waits for the scheduler to terminate
        }
        waitpid(scheduler, NULL, 0);
        }
}

int readFile(char* filePath, queue_t* Pqueue)
{
    int numProcesses = 0;
    //Initialize the Process Queue

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
        enqueue(Pqueue,pd);
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

processMsg createProcessMessage(process_t* P)
{
    processMsg sendingProcess;
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
    destroyQueue(ProcessQueue);
    signal(SIGINT,clearResources);
}