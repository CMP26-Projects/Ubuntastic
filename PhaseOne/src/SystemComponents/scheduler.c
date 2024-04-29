#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <stdint.h>
#include<math.h>

#include "scheduler.h"
#include "../dataStructures/minHeap.h"
#include "../dataStructures/queue.h"

int msgid;
bool receivingFlag=true;
int main(int argc, char *argv[])
{


    //Set the signals handlers  
    signal(SIGCHLD,finishProcess);
    signal(SIGUSR1,receiveProcesses);
    signal(SIGINT,clearResources);
    msgid = createMessageQueue();
    initClk();
    sch=createScheduler(argc,argv);

    switch(sch->algo){
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

scheduler_t* createScheduler(int argc,char** args)
{    
    scheduler_t* sch=(scheduler_t*)malloc(sizeof(scheduler_t));
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
    sch->pcb=createLinkedList(freeSlot,compareSlot);
    //Instantiate the connection with clk & Process_gen & processes

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
    //Displaying the start of scheduler 
    #ifdef DEUBG
        displayScheduler(sch->algo);
    #endif
}


void receiveProcesses(int signum)
{
    processMsg msg;
    while(true)
    {
        int msgReceiving = msgrcv(msgid, &msg, sizeof(msg.data), 7, IPC_NOWAIT);
        //Check this process is a new one
        bool receivedBefore=( sch->lastRecieved != NULL && sch->lastRecieved->ID == msg.data[0]);
        if(msgReceiving == -1 || receivedBefore) //The queue is empty: no processes arrive at this timestamp
        {
            break;
        }

        //Create a process of the recieved data
        process_t* p = createProcess(msg.data);
        
        //Set this process as the last received one 
        sch->lastRecieved=p;

        //Add the new process to the scheduler container  (MinHeap|Queue)
        insertIntoReady(p);
        //Increase the number of recieved processes
        sch->recievedProcessesNum++;
    }
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

//Create a new slot in the pcb for the process
void insertIntoPCB(process_t* p, pid_t pid)
{
    p->lastRun=getClk();
    p->WT=getClk()-(p->AT);
    pcb_slot* newSlot = createSlot(pid,p);
    insertAtEnd(sch->pcb,newSlot);
    updatePCB(p,STARTED); //To be changed in phase2 
}

//Get the process info using the UNIX pid
process_t* getProcessByID(pid_t pid)
{
    void* apid = (void*)(intptr_t)pid;
    node_t* node= getNodeByValue(sch->pcb,(void*)apid);
    if(node!=NULL)
    {
        return ((pcb_slot*)(node->data))->process;
    }
}

void stopProcess(process_t* p)
{
    sch->runningP=NULL;
    updatePCB(p,STOPPED);
    kill(getPID(p),SIGTSTP); //Send SIGSTOP to stop the process from execution
    updateOutfile(p);
}

void continueProcess(process_t* p)
{
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

pid_t getPID(process_t* p)
{
    
    node_t* node= getNodeByValue(sch->pcb,p);
    if(node!=NULL)
    {
        return ((pcb_slot*)(node->data))->pid;
    }
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
    //NOTE: NOT FINISHED YET!!!
    int exitCode;
    wait(&exitCode);
    if (WIFEXITED(exitCode)) {
        int processID=WEXITSTATUS(exitCode);
        printf("process with id %d has ended at time clock %d \n",processID, getClk());
        //Get the process that has finished
        process_t* p=getProcessByID(processID);
        updatePCB(p,FINISHED); //Update the pcb (state & TAT & WTAT)
        sch->totalWT+=p->WT; //Updating the total waiting time
        sch->busyTime+=p->RT; //Updating the total running time
        sch->totalWTAT+=p->WTAT; //Updating the total weighted turnaround time
        sch->finishedProcessesNum++; //Increment the finished processes count
        updateOutfile(p); //Update the output file 
        void* apid = (void*)(intptr_t)processID;
        freeSlot(getNodeByValue(sch->pcb,(void*)apid));
        sch->runningP=NULL; //Free the running process to choose the next one
    }
    else{
        printError("Something went wrong with the exit code of a process.");
    }
    //Reassign the SIGCHLD signal to this function as a handler
    signal(SIGCHLD,finishProcess);
}

void SRTNAlgo()
{
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
            //Check that the running process is the shortest process
            if(!isReadyEmpty() && sch->runningP->state != FINISHED && shortest->RemT < sch->runningP->RemT)
            {
            stopProcess(sch->runningP);
            sch->runningP = shortest;
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

void HPFAlgo()
{
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
    if(sch->algo==RR_t)
        dequeue(sch->readyContainer);
    else
        deleteMin(sch->readyContainer);
    
}

process_t* getNextReady()
{
    if(sch->algo==RR_t)
        return front(sch->readyContainer);
    else
        return getMin(sch->readyContainer);
}

bool isReadyEmpty()
{
    if(sch->algo==RR_t)
        return isEmptyQueue(sch->readyContainer);
    else
        return isEmptyHeap(sch->readyContainer);
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