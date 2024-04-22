#include "headers.h"
#include "scheduler.h"
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

void initializeScheduler(int argc,char** args)
{    
    
    //initalize the scheduler data members
    sch.Algo=atoi(args[2]); 
    sch.switchTime=atoi(args[3]);
    totalProcesses=atoi(args[1]);
    recievedProcesses=0;
    finishedProcesses=0;
    runningProcess=NULL;
    newProcess=NULL;
    totalWaitingTime=0;
    totalWTA=0;
    pcb=(struct PCB*)malloc(sizeof(struct PCB));

    //instantiate the connection with clk & Process_gen & processes
    initClk();
    msgid = createMessageQueue();
    signal(SIGINT, handlerINT);
    signal(SIGUSR1, finishProcessHandler);
    clk_t startTime = getClk();
    printf("the scheduler has started in time slice : %d\n", startTime);
}

void handlerINT(int signum)
{
    //send int signal to all the group to kill them ALL!!
    kill(getpgrp(),SIGINT);
    destroyClk(true);
    exit(-1); //Indicates the unexpected exit and notify the process genetrator 
}
void pushIntoConatainer(void*container,int type,struct Process* p){
 if(type!=RR)
    push((struct minHeap*)container,*p);
 else
    enqueue((struct Queue*)container,*p);
}

bool checkNewProcesses(void* processContainer)
{
        struct msgbuf revievingProcess;
        struct Process* P;
        int msgReciver = msgrcv(msgid, &revievingProcess, sizeof(revievingProcess.data), 7, IPC_NOWAIT);
        if(msgReciver==-1)//The queue is empty: no processes arrive at this timestamp
            return false;
        P = createProcess(revievingProcess.data);
        printf("process %d was sent to the scheduler successfully \n",P->ID);
        pushIntoConatainer(processContainer,sch.Algo,P);
        recievedProcesses++;
        sleep(1);
        return true; //return the new process
}

void startProcess(struct Process* p)
{    
    pid_t pid = fork();
    if (pid == -1)
    {
        perror("Error in Forking the New Process.\n");
        exit(-1);
    }
    else if (pid == 0)
    {
        char id[5], rt[5];
        sprintf(id, "%d", p->ID);
        sprintf(rt, "%d", p->RT);
        char *args[] = {"process.out", id, rt, (char *)NULL};
        printf("process %d will be forked\n",p->ID);
        execv(realpath(args[0],NULL),args);
        perror("Execl process has failed for creating the process\n");
        exit(-1);
    }
    else
    {
        insertPCBSlot(pcb,p,pid);
    }
}

void insertPCBSlot(struct PCB * pcb,struct Process* p,pid_t pid)
{
    updatePCB(pcb,p,STARTED);// should we add another state for the process "arrived"?
//TODO :implement this function(insert the new process into the PCB with the real UNIX pid and other data memebers)
}

struct Process* getProcessByID(pid_t pid){
    //TODO: implement this function (get the process info using the real time process ID)
    return NULL;
}

void stopProcess(struct Process* p)
{
    runningProcess=NULL;
    updatePCB(pcb,p,STOPPED);
    updateOutfile(p,STOPPED);
    kill(getProcessID(p),SIGSTOP);
    sleep(sch.switchTime);
}

void continueProcess(struct Process* p)
{
    runningProcess=p;
    if(p->RT==p->RemT)
        startProcess(p);
    else
    {
        updateOutfile(p,RESUMED);
        kill(getProcessID(p),SIGCONT);
    }
}

void updateOutfile(struct Process *P, enum processState state)
{
    //TODO: implement this function(update the log file with the new state of the process and its data members)
}

pid_t getProcessID(struct Process* P)
{
    //TODO:implement this function(loop over the pcb array and return the UNIX pid of the given process)
}

void updatePCB(struct PCB * pcb,struct Process* P,enum processState Pstate)
{
    //TODO: implement this function(update the pcb of this process byy the correct state)
}

void generatePrefFile()
{
    //TODO: implement this function(create the pref file)
}

void finishProcessHandler(int signum)
{
    //NOTE: NOT FINISHED YET!!!
    int status;
    int pid = wait(&status);
    int ProcessID=WEXITSTATUS(status);
    printf("process with id %d has ended \n",ProcessID);
    struct Process* P=getProcessByID(pid);
    updatePCB(pcb,P,FINISHED);
    updateOutfile(P,FINISHED);
    
    /* TODO:
        * Calculate the WTA of the process from the PCB  and the total waiting time using the following formula:
        * TA = finish time - arrival time
        * WTA = TA/RT
        * totalWTA+= getClk()-P->AT;
        * totalWaitingTime+= waitingtime of the process;
    */
    
    sleep(sch.switchTime);
    runningProcess=NULL;
    finishedProcesses++;
    signal(SIGUSR1,finishProcessHandler);
}

void SRTN_Algo()
{
    clk_t startTime=getClk();
    struct minHeap PQ=initializeHeap(SRTN);
    struct msgbuf revievingProcess;
    while(finishedProcesses!=totalProcesses){
        //CHecking for arriving processes
        while(checkNewProcesses(&PQ))
        { 
            printHeap(&PQ);
        }

        struct Process* shortestProcess=getMin(&PQ);
        printProcess(*shortestProcess);
        //No process is running so just run the shortest one if it exists
        if(runningProcess==NULL)
        {
            if(shortestProcess!=NULL)
            {
                pop(&PQ);
                continueProcess(shortestProcess);
            }
        }
        else
        {
            //if 
            if(shortestProcess!=NULL&&runningProcess->RemT>shortestProcess->RemT)
            {
                pop(&PQ);
                push(&PQ,*runningProcess);
                stopProcess(runningProcess);
                continueProcess(shortestProcess);
            }  
        }
        
    }
    int totalElapsedtime = getClk()-startTime;
    printf("the scheduler has finished in time : %d\n", totalElapsedtime);
    destroyHeap(&PQ);
}
void HPF_Algo()
{
    printf("HPF\n");
    struct minHeap PQ=initializeHeap(HPF);
    struct msgbuf revievingProcess;
    printf("the number of processes is : %d\n", totalProcesses);
    
    ////////////Scheduling Algo implementation//////////////

    printf("the scheduler has finished in time : %d\n", getClk());
    destroyHeap(&PQ);
}
void RR_Algo(char* timeSlice)
{
    printf("RR\n");
    struct Queue ProcessQueue;
    struct Process P;
    struct msgbuf revievingProcess;
    initializeQueue(&ProcessQueue);    
    printf("the number of processes is : %d\n", totalProcesses);

    ////////////Scheduling Algo implementation//////////////

    printf("the scheduler has finished in time : %d\n", getClk());
    destroyQueue(&ProcessQueue);
}

int main(int argc, char *argv[])
{
    // =======Initializing Scheduler========== //
    initializeScheduler(argc,argv);
    switch(sch.Algo){
    case SRTN:
        SRTN_Algo();
        break;
    case HPF:
        HPF_Algo();
        break;
    case RR:
        RR_Algo((argv[4]));
        break;
    default:
        printf("Invalid Scheduler Type\n");
        exit(-1);
        break;
    }
    generatePrefFile();
    destroyClk(true);
    //TODO : deallocate the PCB
}
