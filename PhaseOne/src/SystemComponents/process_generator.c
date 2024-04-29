#include <string.h>
#include <signal.h>
#include "./src/SystemComponents/headers.h"
#include "./src/SystemComponents/UI.h"
#include "./src/SystemComponents/processUnits.h"
#include "./src/dataStructures/queue.h"

int msgid;
queue_t* ProcessQueue;
void clearResources(int);
int readFile(char*,queue_t*);
void getUserInput(int*, int*); //Will get updated and support GUI 
void sendProcess(int*,pid_t); //Create a message of the process data and send it to the scheduler

int main(int argc, char *argv[])
{
    int timeSlice=2;
    int schedAlgo=atoi(argv[2]);
    ProcessQueue=createQueue();
    if(argc>3 && schedAlgo==RR_t)
        timeSlice=atoi(argv[3]);
    
    signal(SIGINT, clearResources); //If it gets interrupted, clear the resources 

    //Read the input files.
    int numProcesses = readFile(argv[1], &ProcessQueue);
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
            clearResources(true);
            exit(-1);
        }
        else
        {
            //Create the message queue to send the process info to the scheduler
            msgid = createMessageQueue();
            int* processInfo =front(ProcessQueue);
            int lastClk = getTime();

            while (!isEmptyQueue(ProcessQueue))
            {
                if (lastClk == getClk())  //It's the same timeclk, so no need to process anything
                {
                    continue;
                }

                //Send All the processes at this timeStamp
                while (processInfo != NULL && getClk >= processInfo[0]) //Check Arrival
                {
                    sendProcess(processInfo, scheduler);
                    dequeue(ProcessQueue);
                    processInfo=front(ProcessQueue);
                }

                //Send signal to the scheduler to receive processes
                kill(scheduler, SIGUSR1);
                lastClk++;
                sleepMilliseconds(100); //Make a small delay to ensure the processes has been received in the scheduler
            }
            //Wait for the scheduler to terminate
            waitpid(scheduler,NULL,0);
        }
    }
}

void sendProcess(int* processInfo,pid_t sch)
{
    processMsg sendingProcess;
    sendingProcess.mtype = 7;
    sendingProcess.data=processInfo;
    int msgSending = msgsnd(msgid, &sendingProcess, sizeof(sendingProcess.data), IPC_NOWAIT);
    #ifdef DEBUG
    printf("the process_gen has sent the process %d to the scheduler at time %d \n", P->ID, getClk(()));
    #endif    

    if(msgSending==-1)
        printError("there is an error in sending");   
 }

void clearResources(int signum)
{
    //Delete the processes shared memory
    msgctl(msgid, IPC_RMID, (struct msqid_ds *)0);
    destroyQueue(ProcessQueue);
    destroyClk(true);
    signal(SIGINT,clearResources);
}
