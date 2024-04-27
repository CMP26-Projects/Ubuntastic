#include "headers.h"
int processID,runTime,remainingTime,startTime; 
void stopProcessHandler(int signum)
{  
    //Set the runtime of the process of the currecnt remaining time 
    runTime=remainingTime;
    //Stop the process from being executed
    raise(SIGTSTP);
    signal(SIGSTOP,stopProcessHandler);
}
void contProcessHandler(int signum)
{
    //Set the start time of the process to be the current time stamp
    //In Short, We simualte the process preemtion as we start a new process with this updated start time & rumtime
    startTime=getClk();
    signal(SIGCONT,contProcessHandler);
}

int main(int agrc, char *argv[])
{
    //Read the process data
    initClk();
    startTime = getClk();
    processID = atoi(argv[1]);
    runTime = atoi(argv[2]);
    remainingTime = runTime;
    //Set the signals' handlers 
    signal(SIGSTOP,stopProcessHandler);
    signal(SIGCONT,contProcessHandler);
    printf("The process started with pid= %d ", processID);
    sleep(1);
    while (remainingTime>0)
    { 
        remainingTime=runTime+startTime-getClk();
    }
    //Clear Resources
    destroyClk(false);
    //Notify scheduler that the process has finished
    //Return the real process ID to remove its slot from the PCB
    exit(processID);
}