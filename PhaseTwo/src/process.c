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
        sprintf(lineToPrint, "The process %d started \n", processID);
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

// Set the runtime of the process of the currecnt remaining time
void stop(int signum)
{
    #ifdef DEBUG
        sprintf(lineToPrint, "The process %d will stop ", processID);
        printLine(lineToPrint, RED);
    #endif
    runTime = remainingTime;
    raise(SIGSTOP);
    signal(SIGTSTP, stop);
}

// Set the start time of the process to be the current time stamp
void resume(int signum)
{
    #ifdef DEBUG
        sprintf(lineToPrint, "The process %d will resume ", processID);
        printLine(lineToPrint, GRN);
    #endif
    startTime = getClk();
    signal(SIGCONT, resume);
}
