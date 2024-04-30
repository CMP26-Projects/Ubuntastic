#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <stdint.h>
#include <math.h>
#include "scheduler.h"

int msgid;
bool receivingFlag=true;
int main(int argc, char *argv[])
{
    //Set the signals handlers
    signal(SIGUSR1,receiveProcesses);
    signal(SIGUSR2,finishProcess);
    signal(SIGINT,clearResources);
    msgid = createMessageQueue();
    initClk();
    createScheduler(argc,argv);
    

    switch(sch->algo)
    {
    case SRTN_t:
        SRTNAlgo();
        break;
    case HPF_t:
        HPFAlgo();
        break;
    case RR_t:
        RRAlgo(atoi(argv[4]));
        break;
    default:
        printError("INVALID SCHEDULING ALGORITM");
        break;
    }
    float* schStatistics=calculateStatistics();
    generatePrefFile(schStatistics);
    exit(sch->totalProcessesNum);
}

void createScheduler(int argc,char** args)
{    
    sch=(struct Scheduler*)malloc(sizeof(struct Scheduler));
    //Initalize the scheduler data members
    sch->algo=atoi(args[2]); 
    sch->totalProcessesNum=atoi(args[1]);
    sch->recievedProcessesNum=0;
    sch->finishedProcessesNum=0;
    sch->runningP=NULL;
    sch->lastRecieved=NULL;
    sch->totalWT=0;
    sch->totalWTAT=0.0;
    sch->busyTime=0;
    int x= createLinkedList();
    //Instantiate the connection with clk & Process_gen & processes
    #ifdef DEBUG
    printf("The scheduler has been created with the algorithm %d\n",sch->algo);
    #endif
    switch(sch->algo)
    {
        case RR_t:
            sch->readyContainer = createQueue();
            break;
        
        case HPF_t:
            sch->readyContainer = createHeap(comparePriority,printProcess);
            break;

        case SRTN_t:
            sch->readyContainer = createHeap(compareRemTime,printProcess);
            break;
            default:
    }
    #ifdef DEBUG
    printf("The ready container has been created\n");
    #endif
    //Displaying the start of scheduler 
    displayScheduler(sch->algo);
}


void receiveProcesses(int signum)
{
    #ifdef DEBUG
    printf("inside recieve Process\n");
    #endif
    processMsg msg;
    while(true)
    {
        //Receive the message from the message queue

        int msgReceiving = msgrcv(msgid, &msg, sizeof(msg.data), 7, IPC_NOWAIT);
        //Check this process is a new one
        #ifdef DEBUG
        printf("msg = %d \n",msgReceiving);
        #endif

        if(msgReceiving == -1) //The queue is empty: no processes arrive at this timestamp
        {
            printf("The queue is empty\n");
            break;
        }

        //Create a process of the recieved data
        process_t* p = createProcess(msg.data);
        

        #ifdef DEBUG
        printf("The process has been created\n");
        printProcess(p);
        #endif

        //Set this process as the last received one 
        sch->lastRecieved=p;

        //Add the new process to the scheduler container  (MinHeap|Queue)
        insertIntoReady(p);
        //Increase the number of recieved processes
        sch->recievedProcessesNum++;
    }
        #ifdef DEBUG
        printf("OutSiede the fukin loop\n");
        #endif
    
    receivingFlag = false; //We finished receiving all the processes of this time clock
    signal(SIGUSR1,receiveProcesses);
}

void startProcess(process_t* p)
{    
    pid_t pid = fork();
    if (pid == -1)
    {
        printError("Error in forking the new process");
        exit(-1);
    }
    else if (pid == 0)
    {
        //Read the data of the process and send them
        char id[5], rt[5];
        sprintf(id, "%d", p->ID);
        sprintf(rt, "%d", p->RT);
        char *args[] = {"process.out", id, rt, (char *)NULL};
        execv(realpath(args[0],NULL),args);
        printError("Execl process has failed for creating the process\n");
        exit(-1);
    }
    else
    {
        sch->runningP=p;
        insertIntoPCB(p,pid);
        updateOutfile(p);
    }
}

void stopProcess(process_t* p)
{
    #ifdef DEBUG
    printf("The process %d will be stopped\n",p->ID);
    #endif
    sch->runningP=NULL;
    updatePCB(p,STOPPED);
    insertIntoReady(p);
    updateOutfile(p);
    kill(getPID(p),SIGTSTP); //Send SIGSTOP to stop the process from execution
}

void continueProcess(process_t* p)
{
    #ifdef DEBUG
    printf("The process %d will be resumed\n",p->ID);
    #endif
    sch->runningP=p;
    if(p->RT==p->RemT) //The first time to run this process
        startProcess(p);
    else
    {
        //It has run before

        if(p->RemT<=0) //It has finished(wouldn't be handled with the SIGCHLD ??) 
            updatePCB(p,FINISHED);
        else
        {
            updatePCB(p,RESUMED);
            kill(getPID(p),SIGCONT);
        }
        updateOutfile(p);
    }
}


float* calculateStatistics()
{
    float* schStatistics=(float*)malloc(4*sizeof(float));
    schStatistics[0] = (sch->busyTime*100) / (float)getClk(); //CPU_Utiliziation
    schStatistics[1] =sch->totalWTAT/sch->totalProcessesNum; //Avg_WTA
    schStatistics[2] =(float)sch->totalWT/sch->totalProcessesNum; //Avg_Waiting
    schStatistics[3] = (float)sqrt(sch->totalWTAT/ (float)(sch->totalProcessesNum- 1)); //StdWTAT
}

void updatePCB(process_t* p,state_t state) 
{
    //For phase 1 -> ARRIVED==STARTED (since this function is only invoked if the process is about to get executed)
    //There's not a case where the process can
    switch (state)
    {
        case STARTED: //For phase 2 (I guess in this phase I think start process do this)
        {
            break;
        }
        case RESUMED:
        {
            //Set this timestamp as the last time this process has run
            int busyTime=(p->RT)-(p->RemT);
            p->WT=getClk()-busyTime-(p->AT);
            p->lastRun=getClk();
            break;
        }
        case STOPPED:
        {
            break;
        }
        case FINISHED:
        {
            //Set the turnaround time of the process
            p->TAT=getClk()-p->AT;
            p->WTAT=(float)p->TAT/p->RT;
            p->WT=getClk()-(p->RT)-(p->AT);
            break;
        }
    
        //case WAITING: (phase2)
        //     {
        //         pcb[index].id=index;
        //         pcb[index].state=Pstate;
        //         pcb[index].AT=p->AT;
        //         pcb[index].RemT=(p->RT);
        //         pcb[index].RT=p->RT;
        //         pcb[index].state=Pstate;
        //         pcb[index].Priority=p->Priority;
        //     }
        //     break;
    }
    p->state=state;
}
void updateOutfile(process_t* p)
{
    float* info=(float*)malloc(4*sizeof(float));
    info[0]=p->ID;
    info[1]=p->RT;
    info[2]=p->RemT;
    info[3]=p->WT;
    if(p->state!=FINISHED)
    {
        insertIntoLog(p->state,info);
    }
    else
    {
        info=(float*)realloc(info,6*sizeof(float));
        info[4]=p->TAT; 
        info[5]=p->WTAT; 
        insertIntoLog(p->state,info);
    }
}


void finishProcess(int signum)
{
    #ifdef DEBUG
    printf("inside finish process\n");
    #endif
    //NOTE: NOT FINISHED YET!!!
    int exitCode;
    wait(&exitCode);
    if (WIFEXITED(exitCode)) {
        int processID=WEXITSTATUS(exitCode);
        #ifdef DEBUG
        printf("process with id %d has ended at time clock %d \n",processID, getClk());
        #endif
        //Get the process that has finished
        process_t* p=getProcessByID(processID);
        updatePCB(p,FINISHED); //Update the pcb (state & TAT & WTAT)
        sch->totalWT+=p->WT; //Updating the total waiting time
        sch->busyTime+=p->RT; //Updating the total running time
        sch->totalWTAT+=p->WTAT; //Updating the total weighted turnaround time
        sch->finishedProcessesNum++; //Increment the finished processes count
        updateOutfile(p); //Update the output file 
        void* apid = (void*)(intptr_t)processID;
        freeSlot(sch->pcb);
        sch->runningP=NULL; //Free the running process to choose the next one
    }
    else{
        printError("Something went wrong with the exit code of a process.");
    }
    //Reassign the SIGCHLD signal to this function as a handler
    signal(SIGUSR2,finishProcess);
}

void SRTNAlgo()
{
    #ifdef DEBUG
    printf("SRTN Algorithm\n");
    #endif
    int lastClk = getClk();

    while (true)
    {

        if (lastClk == getClk()) //It's the same timeclk, so no need to process anything
            continue;
        
        #ifdef DEBUG
        printf("new time %d\n" ,getClk());
        #endif

        lastClk++;
        while (receivingFlag); //Wait to get all the processes arriving at this time stamp 
        receivingFlag = true; //Reset the flag to true to receive the new processes at the next time stamp

        process_t* shortest = getNextReady();
        if(!isReadyEmpty())
        {
            printf("ready is not empty \n");
            #ifdef DEBUG
            #endif
            if(sch->runningP!=NULL && sch->runningP->state != FINISHED && shortest->RemT < sch->runningP->RemT)
            {
                #ifdef DEBUG
                printf("it's not the shortest\n");
                #endif
                stopProcess(sch->runningP);
                sch->runningP = shortest;
                removeFromReady();
                continueProcess(sch->runningP);
            }
            else if(sch->runningP==NULL)
            {
                #ifdef DEBUG
                printf("No running processes\n");
                #endif
                sch->runningP = shortest;
                continueProcess(shortest);
                #ifdef DEBUG
                printf("Let's remove from ready\n");
                #endif
                removeFromReady();
                #ifdef DEBUG
                printf("we have removed ready\n");
                #endif
                
            }
            else
            {
                #ifdef DEBUG
                printf("just update\n");
                #endif
                sch->runningP->RemT--; //Decrement the remaining time of the running process
            }
        }
        else
        {
            printf("ready is empty\n");
            #ifdef DEBUG
            #endif
            if(sch->runningP!=NULL)
            {
                #ifdef DEBUG
                printf("just decrement\n");
                #endif
                sch->runningP->RemT--; //Decrement the remaining time of the running process

            }
        }
        #ifdef DEBUG
        printf("we finished a loop for a timeclk %d\n",getClk());
        #endif
    }
}

void HPFAlgo()
{
    #ifdef DEBUG
    printf("HPF Algorithm\n");
    #endif
    int lastClk = getClk();
    while (true)
    {
        if (lastClk == getClk()) //It's the same timeclk, so no need to process anything
            continue;
        
        while (receivingFlag); //Wait to get all the processes arriving at this time stamp 
        receivingFlag = true; //Reset the flag to true to receive the new processes at the next time stamp

        if (isReadyEmpty() && sch->runningP==NULL) //There is no ready processes to run, so no need to process anything
            continue;

        process_t* shortest = getNextReady();

        if (sch->runningP==NULL) //There is no process running process, so run the next ready process if exists.
        {
            if(!isReadyEmpty())
            {
            sch->runningP = shortest;
            removeFromReady();
            continueProcess(sch->runningP);
            }
            else
            {
                continue;
            }
        }
        else
        {
            sch->runningP->RemT--; //Decrement the remaining time of the running process
        }
        lastClk++;
    }
}

void RRAlgo(int timeSlice)
{
    #ifdef DEBUG
    printf("SRTN Algorithm\n");
    #endif
    int lastClk = getClk();
    while (true)
    {
        if (lastClk == getClk()) //It's the same timeclk, so no need to process anything
            continue;
        
        while (receivingFlag); //Wait to get all the processes arriving at this time stamp 
        receivingFlag = true; //Reset the flag to true to receive the new processes at the next time stamp

        if (isReadyEmpty() && sch->runningP==NULL) //There is no ready processes to run, so no need to process anything
            continue;

        process_t* nextP = getNextReady();

        if (sch->runningP==NULL)
        {
            if(!isReadyEmpty())
            {
            sch->runningP = nextP;
            removeFromReady();
            continueProcess(sch->runningP);
            }
            else
            {
                continue;
            }
        }
        else
        {
            //Check the process has finished the timeSlice or not.
            bool finishedQuanta= (getClk()-(sch->runningP->lastRun))>timeSlice;
            if(!isReadyEmpty() && sch->runningP->state != FINISHED && finishedQuanta)
            {
            stopProcess(sch->runningP);
            sch->runningP = nextP;
            removeFromReady();
            continueProcess(sch->runningP);
            }
            else
            {            
            sch->runningP->RemT--; //Decrement the remaining time of the running process
            }
        }
        lastClk++;
    }
}


void insertIntoReady(process_t* p)
{
    (sch->algo==RR_t)?enqueue(sch->readyContainer,p):insert(sch->readyContainer,p);
}

void removeFromReady()
{
    #ifdef DEBUG
        printf("inside dremove from ready\n");
    #endif
    if(sch->algo==RR_t)
    {
        #ifdef DEBUG
            printf("delete from RR\n");
        #endif
        dequeue(sch->readyContainer);
        #ifdef DEBUG
            printf("deleted from RR\n");
        #endif
    }
    else
    {
        #ifdef DEBUG
            printf("delete from heap\n");
        #endif
        deleteMin(sch->readyContainer);
        #ifdef DEBUG
            printf("deleted from heap\n");
        #endif
    }
    #ifdef DEBUG
    printf("we have removed\n");
    #endif            
}

process_t* getNextReady()
{
    #ifdef DEBUG
    printf("inside getNext = %d\n",sch->algo);
    #endif
    if(sch->algo==RR_t)
        return front(sch->readyContainer);
    else
        return getMin(sch->readyContainer);
}

bool isReadyEmpty()
{
    #ifdef DEBUG
    printf("inside isReady = %d\n",sch->algo);
    #endif
    bool is;
    if(sch->algo==RR_t)
        is= isEmptyQueue(sch->readyContainer);
    else
        is= isEmptyHeap(sch->readyContainer);
    #ifdef DEBUG
    printf("is is %d\n",is);
    #endif
    
    return is;
}

void destroyReady()
{
    if(sch->algo==RR_t)
        destroyQueue(sch->readyContainer);
    else
        destroyHeap(sch->readyContainer);   
}

void clearResources(int signum)
{
    destroyReady();
    destroyList(sch->pcb);
    destroyClk(true);
}