#include <stdio.h>
#include "headers.h"
int processID,runTime,remainingTime,startTime; 
void stopProcessHandler(int signum)
{  
    FILE *fp;
    
    fp = fopen("example.txt", "a");
    fprintf(fp, "AT time %d process %d stopped total %d remain %d wait 0\n",getClk(), processID,runTime,remainingTime);
    fclose(fp);
    // printf("AT time %d process %d stopped total %d remain %d wait 0\n",getClk(), processID,runTime,remainingTime); //test
    runTime=remainingTime;
    raise(SIGTSTP);
    signal(SIGSTOP,stopProcessHandler);
}
void contProcessHandler(int signum)
{
    // FILE *fp;
    
    // fp = fopen("example.txt", "a");
    // fprintf(fp, "AT time %d process %d continued total %d remain %d wait 0\n",getClk(), processID,runTime,remainingTime);
    // fclose(fp);
    
    // printf("AT time %d process %d continued total %d remain %d wait 0\n",getClk(), processID,runTime,remainingTime); //test
    startTime=getClk();
    signal(SIGCONT,contProcessHandler);
}

int main(int agrc, char *argv[])
{
    initClk();
    startTime = getClk();
    processID = atoi(argv[1]);
    runTime = atoi(argv[2]);
    remainingTime = runTime;
    signal(SIGSTOP,stopProcessHandler);
    signal(SIGCONT,contProcessHandler);
    FILE *fp;
    
    // fp = fopen("example.txt", "a");
    // fprintf(fp, "AT time %d process %d started total %d remain %d wait 0\n", startTime, processID,runTime,remainingTime);
    // fclose(fp);
    
    // printf("AT time %d process %d started total %d remain %d wait 0\n", startTime, processID,runTime,remainingTime); //test
    // TODO it needs to get the remaining time from somewhere
    while (remainingTime>0)
    { 
        remainingTime=runTime+startTime-getClk();
    }
    //Clear Resources
    destroyClk(false);
    //notify scheduler that the process has finished
    kill(getppid(),SIGUSR1);
    exit(processID);
}
