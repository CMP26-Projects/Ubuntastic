#include "UI.h"
char *lineToPrint;

int processID, runTime, remainingTime, startTime;
void stop(int);
void resume(int);

int main(int agrc, char *argv[])
{

    // Read the process data
    initClk();
    startTime = getClk();
    processID = atoi(argv[1]);
    runTime = atoi(argv[2]);
    remainingTime = runTime;
    // Set the signals' handlers
    signal(SIGTSTP, stop);
    signal(SIGCONT, resume);
    lineToPrint = (char *)malloc(LINE_SIZE * sizeof(char));
    #ifdef DEBUG
        sprintf(lineToPrint, "The process started with pid= %d \n", processID);
        printLine(lineToPrint, GRN);
    #endif
    clk_t lastClk = getClk();
    while (remainingTime > 0)
    {
        if (getClk() == lastClk)
            continue;
        lastClk++;
        remainingTime = runTime + startTime - getClk();
    }
    raise(SIGINT);
}

// Stop the process from being executed
void stop(int signum)
{
    // Set the runtime of the process of the currecnt remaining time
    runTime = remainingTime;

#ifdef DEBUG
    sprintf(lineToPrint, "I'm the process %d will stop ", processID);
    print(lineToPrint, RED);
    sprintf(lineToPrint, "at time %d", getClk());
    printLine(lineToPrint, RED);
#endif

    raise(SIGSTOP);
    signal(SIGTSTP, stop);
}

// Set the start time of the process to be the current time stamp
void resume(int signum)
{
#ifdef DEBUG
    sprintf(lineToPrint, "I'm the process %d will resume ", processID);
    print(lineToPrint, GRN);
    sprintf(lineToPrint, "at time %d", getClk());
    printLine(lineToPrint, GRN);
#endif
    startTime = getClk();
    signal(SIGCONT, resume);
}
