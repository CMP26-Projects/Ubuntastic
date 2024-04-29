#include <string.h>
#include <signal.h>
#include "UI.h"
#include "../dataStructures/queue.h"

int msgid;
queue_t* ProcessQueue;
pid_t scheduler;
int readFile(char* filePath, queue_t* q);
void clearResources(int);
int readFile(char*,queue_t*);
void getUserInput(int*, int*); //Will get updated and support GUI 
void sendProcess(int*); //Create a message of the process data and send it to the scheduler

int main(int argc, char *argv[])
{
    msgid = createMessageQueue();
    signal(SIGINT, clearResources); //If it gets interrupted, clear the resources 
    int timeSlice=2;
    int schedAlgo=atoi(argv[2]);
    ProcessQueue=createQueue();
    if(argc>3 && schedAlgo==RR_t)
        timeSlice=atoi(argv[3]);
    

    //Read the input files.
    int numProcesses = readFile(argv[1],ProcessQueue);
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
        scheduler = fork();
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
            sleepMilliseconds(100);
            printf("hi fterasd as e\n");

            //Create the message queue to send the process info to the scheduler
            int* processInfo =front(ProcessQueue);
            int lastClk = getClk();

            while (true)
            {
                if (lastClk == getClk())  //It's the same timeclk, so no need to process anything
                {
                    continue;
                }

                //Send All the processes at this timeStamp
                while (processInfo != NULL && getClk() >= processInfo[1]) //Check Arrival
                {
                    sendProcess(processInfo);
                    dequeue(ProcessQueue);
                    processInfo=front(ProcessQueue);
                }

                //Send signal to the scheduler to receive processes
                kill(scheduler, SIGUSR1);
                sleepMilliseconds(100); //Make a small delay to ensure the processes has been received in the scheduler
                lastClk++;
            }
        }
    }
}

void sendProcess(int* processInfo)
{
    processMsg sendingProcess;
    sendingProcess.mtype = 7;
    sendingProcess.data[0]=processInfo[0];
    sendingProcess.data[1]=processInfo[1];
    sendingProcess.data[2]=processInfo[2];
    sendingProcess.data[3]=processInfo[3];
    int msgSending = msgsnd(msgid, &sendingProcess, sizeof(sendingProcess.data), !IPC_NOWAIT);
    #ifdef DEBUG
    printf("the process_gen has sent the process %d to the scheduler at time %d \n", processInfo[0], getClk());
    #endif    

    if(msgSending==-1)
        printError("there is an error in sending");   
 }

void clearResources(int signum)
{
    //Delete the processes shared memory
    msgctl(msgid, IPC_RMID, (struct msqid_ds *)0);
    destroyQueue(ProcessQueue);
    kill(scheduler,SIGINT);
    destroyClk(true);
    signal(SIGINT,clearResources);
}
int readFile(char* filePath, queue_t* q)
{
    int numProcesses = 0;
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
        int* info=(int*) malloc(4*sizeof(int));
        info[0]=id;
        info[1]=arrivalTime;
        info[2]=runTime;
        info[3]=priority;
        processMsg msg=createMsg(info);
        enqueue(q,&msg);
        numProcesses++;
    }
    fclose(inputFile);
    //Testing
    
    #ifdef DEBUG
    printf("Number of processes: %d\n", numProcesses);
    #endif
    return numProcesses;
}

