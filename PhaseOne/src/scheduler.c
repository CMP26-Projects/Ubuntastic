#include "scheduler.h"
int msgid;
bool receivingFlag = true;
char lineToPrint[1000];
int main(int argc, char *argv[])
{
    // Set the signals handlers
    signal(SIGUSR1, receiveProcesses);
    signal(SIGUSR2, finishProcess);
    signal(SIGINT, finishScheduling);

    // Set the connnection with the message queue and the clock
    msgid = createMessageQueue();
    initClk();
    sch = createScheduler(argc, argv);

    switch (sch->algo)
    {
    case SRTN_t:
        SRTNAlgo();
        break;
    case HPF_t:
        HPFAlgo();
        break;
    case RR_t:
        RRAlgo(atoi(argv[3]));
        break;
    default:
        printError("INVALID SCHEDULING ALGORITM");
        break;
    }
}

scheduler_t *createScheduler(int argc, char *args[])
{
    scheduler_t *sc = (scheduler_t *)malloc(sizeof(scheduler_t));
    // Initalize the scheduler data members
    sc->pCount = atoi(args[1]);
    sc->algo = atoi(args[2]);
    sc->receivedPCount = 0;
    sc->finishedPCount = 0;
    sc->runningP = NULL;
    sc->lastRecieved = NULL;
    sc->totalWT = 0;
    sc->totalWTAT = 0.0;
    sc->busyTime = 0;
    sc->PCB = (process_t **)malloc((sc->pCount + 1) * sizeof(process_t *));
    switch (sc->algo)
    {
    case RR_t:
        sc->readyContainer = createQueue();
        break;

    case HPF_t:
        sc->readyContainer = createHeap(HPF_t);
        break;

    case SRTN_t:
        sc->readyContainer = createHeap(SRTN_t);
        break;

    default:
        printError("INVALID SCHEDULING ALGORITM");
        break;
    }
    return sc;
}

void receiveProcesses(int signum)
{
    processMsg msg;
    while (true)
    {
        // Receive the message from the message queue
        int msgReceiving = msgrcv(msgid, &msg, sizeof(msg.data), 12, IPC_NOWAIT);
        // Check this process is a new one
        if (msgReceiving == -1)
        {
            break;
        }
        // Create a process of the recieved data
        process_t *p = createProcess(msg.data);
        // Set this process as the last received one
        sprintf(lineToPrint, "Scheduler received process %d at timeClk%d\n", p->ID, getClk());
        printLine(lineToPrint, GRN);
        printProcess(p, NRM);
        // Add the new process to the scheduler container  (MinHeap|Queue)
        insertIntoReady(p);
        // Increase the number of recieved processes
        sch->receivedPCount++;
    }
    receivingFlag = false; // We finished receiving all the processes of this time clock
    signal(SIGUSR1, receiveProcesses);
}

void startProcess(process_t *p)
{
    pid_t pid = fork();
    if (pid == -1)
    {
        printError("Error in forking the new process");
        exit(-1);
    }
    else if (pid == 0)
    {
        // Read the data of the process and send them
        char id[5], rt[5];
        sprintf(id, "%d", p->ID);
        sprintf(rt, "%d", p->RT);
        char *args[] = {"./process.out", id, rt, (char *)NULL};
        execv(args[0], args);
        printError("Execl process has failed for creating the process\n");
        exit(-1);
    }
    else
    {
        sch->runningP = p;
        p->state = STARTED;
        sch->PCB[p->ID] = p;
        sch->PCB[p->ID]->PID = pid;
        updateOutfile(p);
    }
}

void stopProcess(process_t *p)
{
    if (p->RemT > 0)
    {
        p->state = STOPPED;
        insertIntoReady(p);                  // Insert the process into the ready container
        updateOutfile(p);                    // Update the log file
        kill(sch->PCB[p->ID]->PID, SIGTSTP); // Send SIGSTOP to stop the process from execution
    }
    else
    {
        sleepMilliseconds(100);
    }
}

void continueProcess(process_t *p)
{
    sch->runningP = p;
    if (p->RT == p->RemT) // The first time to run this process
        startProcess(p);
    else
    {
        // It has run before
        int busyTime = (p->RT) - (p->RemT);    // Get the total time this process has run
        p->WT = getClk() - busyTime - (p->AT); // Update the waiting time of the process
        p->lastRun = getClk();                 // Set the last time this process has run
        p->state = RESUMED;
        kill(sch->PCB[p->ID]->PID, SIGCONT); // Send SIGCONT to resume the process
        updateOutfile(p);
    }
}

float *calculateStatistics()
{
    float *schStatistics = (float *)malloc(4 * sizeof(float));
    schStatistics[0] = (sch->pCount == 0) ? 0 : (sch->busyTime * 100) / (float)getClk();                  // CPU_Utiliziation
    schStatistics[1] = (sch->pCount == 0) ? 0 : (sch->totalWTAT / sch->pCount);                           // Avg_WTA
    schStatistics[2] = (sch->pCount == 0) ? 0 : (float)sch->totalWT / sch->pCount;                        // Avg_Waiting
    schStatistics[3] = (sch->pCount <= 1) ? 0 : ((float)sqrt(sch->totalWTAT / (float)(sch->pCount - 1))); // StdWTAT
    return schStatistics;
}

void updateOutfile(process_t *p)
{
    float *info = (float *)malloc(4 * sizeof(float));
    info[0] = p->ID;
    info[1] = p->RT;
    info[2] = p->RemT;
    info[3] = p->WT;
    if (p->state != FINISHED)
        insertIntoLog(p->state, info);
    else
    {
        info = (float *)realloc(info, 6 * sizeof(float));
        info[4] = p->TAT;
        info[5] = p->WTAT;
        insertIntoLog(p->state, info);
    }
}

void finishProcess(int signum)
{

    process_t *p = sch->runningP;         // Update the state of the process
    p->TAT = getClk() - p->AT;            // Set the turnaround time of the process
    p->WTAT = (float)p->TAT / p->RT;      // Set the weighted turnaround time of the process
    p->WT = getClk() - (p->RT) - (p->AT); // Update the waiting time of the process
    p->state = FINISHED;                  // Update the state
    sch->totalWT += p->WT;                // Updating the total waiting time
    sch->busyTime += p->RT;               // Updating the total running time
    sch->totalWTAT += p->WTAT;            // Updating the total weighted turnaround time
    sch->finishedPCount++;                // Increment the finished processes count
    int state;
    int pid = wait(&state);
    updateOutfile(p); // Update the output file
    free(sch->PCB[p->ID]);
    sch->runningP = NULL; // Free the running process to choose the next one
    if (sch->finishedPCount == sch->pCount)
        finishScheduling(0);
    // Reset the SIGCHLD signal to this function as a handler
    signal(SIGUSR2, finishProcess);
}

void SRTNAlgo()
{
    int lastClk = getClk();
    while (true)
    {
        if (lastClk == getClk() && lastClk != 1) // It's the same timeclk, so no need to process anything
            continue;

        lastClk++;
        sleepMilliseconds(180);
        if (sch->runningP != NULL)
            sch->runningP->RemT--;

        while (receivingFlag)
            ;                 // Wait to get all the processes arriving at this time stamp
        receivingFlag = true; // Reset the flag to true to receive the new processes at the next time stamp
        process_t *shortest = getNextReady();

        if (!isReadyEmpty())
        {
            if (sch->runningP != NULL && sch->runningP->state != FINISHED && shortest->RemT < sch->runningP->RemT)
            {

                stopProcess(sch->runningP);
                continueProcess(shortest);
                removeFromReady();
            }
            else if (sch->runningP == NULL)
            {
                continueProcess(shortest);
                removeFromReady();
            }
        }
    }
}

void HPFAlgo()
{
    int lastClk = getClk();
    while (true)
    {
        if (lastClk == getClk() && lastClk != 1) // It's the same timeclk, so no need to process anything
            continue;
        sleepMilliseconds(180);
        if (sch->runningP != NULL)
            sch->runningP->RemT--;
        while (receivingFlag)
            ;                 // Wait to get all the processes arriving at this time stamp
        receivingFlag = true; // Reset the flag to true to receive the new processes at the next time stamp

        if (isReadyEmpty() && sch->runningP == NULL) // There is no ready processes to run, so no need to process anything
            continue;

        if (sch->runningP == NULL) // There is no process running process, so run the next ready process if exists.
        {
            process_t *shortest = getNextReady();
            removeFromReady();
            continueProcess(shortest);
        }
        lastClk++;
    }
}

void RRAlgo(int timeSlice)
{
    int quanta = 0;
    int lastClk = getClk();
    while (true)
    {
        if (lastClk == getClk() && lastClk != 1) // It's the same timeclk, so no need to process anything
            continue;
        sleepMilliseconds(180);
        if (sch->runningP != NULL)
        {
            sch->runningP->RemT--;
            quanta++;
        }

        while (receivingFlag)
            ;                 // Wait to get all the processes arriving at this time stamp
        receivingFlag = true; // Reset the flag to true to receive the new processes at the next time stamp
        process_t *nextP = getNextReady();
        if (!isReadyEmpty()) // There is no ready processes to run, so no need to process anything
        {
            if (sch->runningP != NULL)
            {
                if (quanta == timeSlice || sch->runningP->RemT == 0)
                {
                    quanta = 0;
                    stopProcess(sch->runningP);
                    removeFromReady();
                    continueProcess(nextP);
                }
            }
            else
            {
                quanta = 0;
                removeFromReady();
                continueProcess(nextP);
            }
        }
        lastClk++;
    }
}

void insertIntoReady(process_t *p)
{
    if (sch->algo == RR_t)
        enqueue(sch->readyContainer, p);
    else
        insert(sch->readyContainer, p);
}

void removeFromReady()
{
    if (sch->algo == RR_t)
    {
        dequeue(sch->readyContainer);
    }
    else
    {
        deleteMin(sch->readyContainer);
    }
}

process_t *getNextReady()
{
    if (sch->algo == RR_t)
        return front(sch->readyContainer);
    else
        return getMin(sch->readyContainer);
}

bool isReadyEmpty()
{
    if (sch->algo == RR_t)
        return isEmptyQueue(sch->readyContainer);
    else
        return isEmptyHeap(sch->readyContainer);
}

void destroyReady()
{
    if (sch->algo == RR_t)
        destroyQueue(sch->readyContainer);
    else
        destroyHeap(sch->readyContainer);
}

void finishScheduling(int signum)
{
    destroyReady();
    float *schStatistics = calculateStatistics();
    generatePrefFile(schStatistics);
    printf("FINISHED\n");
    destroyClk(false);
    kill(getppid(), SIGUSR2);
    exit(0);
}