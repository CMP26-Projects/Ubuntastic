#include "./src/SystemComponents/headers.h"

int processID,runTime,remainingTime,startTime; 
void stop(int);
void resume(int);

int main(int agrc, char *argv[])
{
    //Read the process data
    initClk();
    startTime = getClk();
    processID = atoi(argv[1]);
    runTime = atoi(argv[2]);
    remainingTime = runTime;
    //Set the signals' handlers 
    signal(SIGTSTP,stop);
    signal(SIGCONT,resume);

    #ifdef DEBUG
    printf("The process started with pid= %d \n", processID);
    #endif    

    sleepMilliseconds(100);
    while (remainingTime>0)
    { 
        remainingTime=runTime+startTime-getClk();
    }
    //Clear Resources
    destroyClk(false);
    //Notify scheduler that the process has finished(it will send SIGCHLD to the scheduler)
    
    //Return the real process ID to remove its slot from the PCB
    exit(processID);
}

//Stop the process from being executed
void stop(int signum)
{  
    //Set the runtime of the process of the currecnt remaining time 
    runTime=remainingTime;
    
    #ifdef DEBUG
    printf("I'm process %d will stop and remaining %d \n",processID,remainingTime);
    #endif    
    
    raise(SIGSTOP);
    signal(SIGTSTP,stop);
}

//Set the start time of the process to be the current time stamp
void resume(int signum)
{
    #ifdef DEBUG
    printf("I'm process %d will resume and remaining %d \n",processID,remainingTime);
    #endif        
    startTime=getClk();
    signal(SIGCONT,resume);
}
