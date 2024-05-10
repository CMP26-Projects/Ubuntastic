#include "scheduler.h"

int msgid;
int quanta;
bool receivingFlag = true;
char lineToPrint[1000];

int main(int argc, char *argv[])
{
    // Set the signals handlers
    signal(SIGUSR1, receiveProcesses);
    signal(SIGUSR2, finishProcess);
    signal(SIGINT, finishScheduling);
    
    // Create scheduler 
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
    sc->busyTime = 0;
    sc->totalWT = 0;
    sc->PCB = (process_t **)malloc((sc->pCount + 1) * sizeof(process_t *));
    sc->memory = initializeMemory();
    sc->waitingContainer = createHeap(MEM_t);
    sc->WTATList=(float*)malloc(sc->pCount*sizeof(float));
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
    // Set the connnection with the message queue and the clock
    initClk();
    msgid = createMessageQueue();

    return sc;
}
void SRTNAlgo()
{
    int lastClk = -1;
    while (true)
    {
        if (lastClk == getClk())    // It's the same timeclk, so no need to process anything
            continue;
        lastClk=getClk();

        while (receivingFlag);      // Wait to get all the processes arriving at this time stamp
        receivingFlag = true;       // Reset the flag to true to receive the new processes at the next time stamp

        if (sch->runningP != NULL)
            handleRemT();


        if (!isReadyEmpty())
        {
            process_t *shortest = getNextReady();
            if (sch->runningP == NULL)
            {
                removeFromReady();
                continueProcess(shortest);
            }
            else if ( shortest->RemT < sch->runningP->RemT+1)
            {
                removeFromReady();
                stopProcess(sch->runningP);
                continueProcess(shortest);
            }
        }
    }
}

void HPFAlgo()
{
    int lastClk = -1;
    while (true)
    {
        //Check if this is a new time stamp
        if (lastClk == getClk())    // It's the same timeclk, so no need to process anything
            continue;
        lastClk=getClk();

        //Recieve New Processes
        while (receivingFlag);      // Wait to get all the processes arriving at this time stamp
        receivingFlag = true;       // Reset the flag to true to receive the new processes at the next time stamp
        
        //Handle the remainig time of the running process
        if (sch->runningP != NULL)
            handleRemT();

        //Run next ready process if there is no running process 
        if (sch->runningP == NULL&&!isReadyEmpty()) // There is no process running process, so run the next ready process if exists.
        {
            process_t *shortest = getNextReady();
            removeFromReady();
            continueProcess(shortest);
        }
    }
}

void RRAlgo(int timeSlice)
{
    int lastClk = -1;
    while (true)
    {
        if (lastClk == getClk())    // It's the same timeclk, so no need to process anything
            continue;
        lastClk=getClk();

        while (receivingFlag);      // Wait to get all the processes arriving at this time stamp
        receivingFlag = true;       // Reset the flag to true to receive the new processes at the next time stamp

        if (sch->runningP != NULL)
            handleRemT();

        if (!isReadyEmpty())        // There is no ready processes to run, so no need to process anything
        {
            process_t *nextP = getNextReady();
            if (sch->runningP == NULL)
            {
                removeFromReady();
                continueProcess(nextP);
            }
            else if (getClk() - sch->runningP->lastRun >= timeSlice)
            {
                removeFromReady();
                stopProcess(sch->runningP);
                continueProcess(nextP);
            }
            //     if (quanta == timeSlice || sch->runningP->RemT == 0)
            //     {
            //         quanta = 0;
            //         stopProcess(sch->runningP);
            //         removeFromReady();
            //         continueProcess(nextP);
            //     }
            // }
            // else
            // {
            //     quanta = 0;
            //     removeFromReady();
            //     continueProcess(nextP);
            // }
        }
    }
}

void handleRemT()
{
    printf("the remianing time for process %d is %d at time %d\n",sch->runningP->ID,sch->runningP->RemT,getClk());
    switch(sch->algo)
    {
        case HPF_t:
        {
            if(sch->runningP->RemT>=1)
                sch->runningP->RemT--;
            else
                finishProcess(true);
            break;
        }
        case RR_t:
        {
            if(sch->runningP->RemT>=1)
                sch->runningP->RemT--;
            else
                finishProcess(true);
            break;
        }
        case SRTN_t:
        {
            if(sch->runningP->RemT>=1)
                sch->runningP->RemT--;
            else
                finishProcess(true);
            break;
        }
        
    }
}

void receiveProcesses(int signum)
{
    processMsg msg;
    while (true)
    {
        // Receive the message from the message queue
        int msgReceiving = msgrcv(msgid, &msg, sizeof(msg.data), 12, IPC_NOWAIT);

        if (msgReceiving == -1)
            break;

        // Create a process of the recieved data
        process_t *p = createProcess(msg.data);
        p->state = WAITING;

        ///[Author: Mariam]
        if (allocateProcess(sch->memory, p))
        {
            insertIntoReady(p);
            updateOutfile(p);
        }
        else
            insertIntoWait(p);
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
        p->lastRun = getClk();
        sch->PCB[p->ID] = p;
        sch->PCB[p->ID]->PID = pid;
        updateOutfile(p);
        sch->runningP->RemT--;
    }
}

void stopProcess(process_t *p)
{
    insertIntoReady(p);                  // Insert the process into the ready container
    p->state = STOPPED;
    int busyTime = (p->RT) - (p->RemT);    // Get the total time this process has run
    p->WT = getClk() - busyTime - (p->AT)+1; // Update the waiting time of the process
    updateOutfile(p);                    // Update the log file
    kill(sch->PCB[p->ID]->PID, SIGTSTP); // Send SIGSTOP to stop the process from execution
}

void continueProcess(process_t *p)
{
    sch->runningP = p;
    p->WT = getClk() -(p->AT); // Update the waiting time of the process
    printf("we will start process %d at time = %d \n",sch->runningP->ID,getClk());
    if (p->RT == p->RemT) // The first time to run this process
        startProcess(p);
    else
    {
        // It has run before
        int busyTime = (p->RT) - (p->RemT);    // Get the total time this process has run
        p->WT = getClk() - busyTime - (p->AT)+1; // Update the waiting time of the process
        p->lastRun = getClk();                 // Set the last time this process has run
        p->state = RESUMED;
        kill(sch->PCB[p->ID]->PID, SIGCONT); // Send SIGCONT to resume the process
        updateOutfile(p);
    }
}

void finishProcess(int signum)
{
    process_t *p = sch->runningP; // Update the state of the process
    p->state = FINISHED;          // Update the state
    p->RemT=0;
    sch->busyTime += p->RT;       // Updating the total running time
    p->TAT = getClk() - p->AT;            // Set the turnaround time of the process
    p->WTAT = (float)p->TAT / p->RT;      // Set the weighted turnaround time of the process
    p->WT = getClk() - (p->RT) - (p->AT); // Update the waiting time of the process
    sch->WTATList[sch->finishedPCount]=p->WTAT;
    printf("The process WT is %f \n",sch->WTATList[sch->finishedPCount]);
    sch->totalWT+=p->WT;
    sch->finishedPCount++;        // Increment the finished processes count
    int state;
    int pid = wait(&state);

    updateOutfile(p);     // Update the output file
    sch->runningP = NULL; // Free the running process to choose the next one
    // insertIntoReady(getNextWait()); // Insert into the heap the next waiting process
    freeMemory(sch->memory, p); // free the memory of that process
    checkWaiting();

    if (sch->finishedPCount == sch->pCount)
        finishScheduling(0);
}

void updateOutfile(process_t *p)
{
    float *info = (float *)malloc(4 * sizeof(float));
    if (p->state == READY||p->state == FINISHED)
    {
        info[0] = p->ID;
        info[1] = p->size;
        info[2] = p->interval->start;
        info[3] = p->interval->end;
        addMemoryEvent(p->state, info);
    }
    info[0] = p->ID;
    info[1] = p->RT;
    info[2] = p->RemT;
    if(p->state== STOPPED||p->state== RESUMED)
        info[2]++;
    info[3] = p->WT;
    if(p->state!= READY)
    {
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
    free(info);
}

float calculateAvgWTA()
{
    float totalWTA=0.0;
    for(int i=0;i<sch->pCount;i++)
        totalWTA+=sch->WTATList[i];
    return totalWTA/(float)sch->pCount;
}

float calculateStdWTA(float avgWTA)
{
    float MSE=0.0;
    for(int i=0;i<sch->pCount;i++)
        MSE+=pow((avgWTA-(sch->WTATList[i])),2);
    return sqrt(MSE/sch->pCount);
}
float *calculateStatistics()
{
    float *schStatistics = (float *)malloc(4 * sizeof(float));
    schStatistics[0] =(sch->pCount==0)? 0 : (sch->busyTime * 100) / (float)getClk();                // CPU_Utiliziation
    schStatistics[1] =(sch->pCount==0)? 0 : calculateAvgWTA();                           // Avg_WTA
    schStatistics[2] =(sch->pCount==0)? 0 : (float)sch->totalWT / sch->pCount;                      // Avg_Waiting
    schStatistics[3] =(sch->pCount==0)? 0 : calculateStdWTA(schStatistics[1]); // StdWTAT
    return schStatistics;
}

//[Author: Mariam]
void checkWaiting()
{
    process_t *shortest = getNextWait();

    while (shortest != NULL && allocateProcess(sch->memory, shortest))
    {
        insertIntoReady(shortest);
        updateOutfile(shortest);
        removeFromWait();
        shortest = getNextWait();
    }
}

void insertIntoReady(process_t *p)
{
    p->state=READY;
    if (sch->algo == RR_t)
        enqueue(sch->readyContainer, p);
    else
        insert(sch->readyContainer, p);
}

void insertIntoWait(process_t *p)
{
    insert(sch->waitingContainer, p);
}

void removeFromReady()
{
    if (sch->algo == RR_t)
        dequeue(sch->readyContainer);
    else
        deleteMin(sch->readyContainer);
}

void removeFromWait()
{
    deleteMin(sch->waitingContainer);
}

process_t *getNextReady()
{
    if (sch->algo == RR_t)
        return front(sch->readyContainer);
    else
        return getMin(sch->readyContainer);
}

process_t *getNextWait()
{
    return getMin(sch->waitingContainer);
}

bool isReadyEmpty()
{
    if (sch->algo == RR_t)
        return isEmptyQueue(sch->readyContainer);
    else
        return isEmptyHeap(sch->readyContainer);
}

bool isWaitEmpty()
{
    return isEmptyHeap(sch->waitingContainer);
}

void destroyReady()
{
    if (sch->algo == RR_t)
        destroyQueue(sch->readyContainer);
    else
        destroyHeap(sch->readyContainer);
}

void destroyWait()
{
    destroyHeap(sch->waitingContainer);
}

void finishScheduling(int signum)
{
    destroyReady();
    destroyWait();

    float *schStatistics = calculateStatistics();
    generatePrefFile(schStatistics);

    printf("FINISHED\n");
    destroyClk(false);
    kill(getppid(), SIGUSR2);
    exit(0);
}