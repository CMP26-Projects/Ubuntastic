#include <stdio.h>
#include "headers.h"

int main(int agrc, char *argv[])
{
    // TODO it needs to get the remaining time from somewhere
    int processID = atoi(argv[1]);
    int runTime = atoi(argv[2]);
    int remainingTime = runTime;
    initClk();
    int Time = getClk();
    printf("Process %d started at %d\n", processID, Time);
     while (remainingTime>0)
    {   
        remainingTime--;
    }

    destroyClk(false);
    exit(processID);
}
