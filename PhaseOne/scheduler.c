#include "headers.h"
#include "scheduler.h"
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include<math.h>
int msgid;
pcb_t* pcb;
process_t* runningProcess;
process_t* lastRecieved;
int Algo;
int timeSlice;
int finishedProcesses;
int recievedProcesses;
int totalProcesses;
int totalWT;
int totalRT;
float totalWTAT;

//Functions definition
void initializeScheduler();
void handlerINT(int);
bool checkNewProcesses(void*);
void pushIntoConatainer(void*,int,process_t*);
void startProcess(process_t*);
void insertPCBSlot(pcb_t*,process_t*,pid_t);             
process_t* getProcessByID(pid_t);
void stopProcess(process_t*);
void continueProcess(process_t*);
void updateOutfile(process_t*);
pid_t getProcessID(process_t*);
void updatePCB(pcb_t*,process_t*,state_t);    
void finishProcessHandler(int);
void generatePrefFile();
void SRTN_Algo();
void HPF_Algo();
void RR_Algo(char*);
void destroyPCB();
int main(int argc, char *argv[])
{
    initializeScheduler(argc,argv);
    clk_t startTime=getClk();
    switch(Algo){
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
        perror("Invalid Scheduler Type\n");
        break;
    }
    generatePrefFile();
    destroyPCB();
    int totalElapsedtime = getClk()-startTime;
    printf("the scheduler has finished in time : %d\n", totalElapsedtime);
    destroyClk(true);
}

void initializeScheduler(int argc,char** args)
{    
    //Initalize the scheduler data members
    Algo=atoi(args[2]); 
    totalProcesses=atoi(args[1]);
    recievedProcesses=0;
    finishedProcesses=0;
    runningProcess=NULL;
    lastRecieved=NULL;
    totalWT=0;
    totalWTAT=0;
    totalRT=0;
    pcb=(struct PCB*)malloc(totalProcesses*sizeof(struct PCB));
    //instantiate the connection with clk & Process_gen & processes
    initClk();
    clk_t startTime = getClk();
    msgid = createMessageQueue();
    signal(SIGUSR1,finishProcessHandler);
    //Clear the scheduler.log file
    system("rm Scheduler.log");
}


bool checkNewProcesses(void* processContainer)
{
        msgbuf revievingProcess;
        process_t* P;
        int msgReciver = msgrcv(msgid, &revievingProcess, sizeof(revievingProcess.data), 7, IPC_NOWAIT);
        if(msgReciver==-1||(lastRecieved!=NULL&&lastRecieved->ID==revievingProcess.data[0])) //The queue is empty: no processes arrive at this timestamp
        {
            return false;
        }
        //Create a process of the recieved data
        P = createProcess(revievingProcess.data);
        lastRecieved=P;
        printf("process %d was sent to the scheduler successfully \n",P->ID);
        //Add the new process to the scheduler container  (MinHeap|Queue)
        pushIntoConatainer(processContainer,Algo,P);
        recievedProcesses++;
        return true;
}

void pushIntoConatainer(void*container,int type,process_t* p)
{
 if(type!=RR)
    push((minHeap_t*)container,*p);
 else
    enqueue((queue_t*)container,*p);
}

void startProcess(process_t* p)
{    
    pid_t pid = fork();
    if (pid == -1)
    {
        perror("Error in forking the new process\n");
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
        perror("Execl process has failed for creating the process\n");
        exit(-1);
    }
    else
    {
        runningProcess=p;
        insertPCBSlot(pcb,p,pid);
        updateOutfile(p);
    }
}

void insertPCBSlot(pcb_t* pcb, process_t* p, pid_t pid)
{
    int index=(p->ID)-1;
    pcb[index].pid=pid;
    pcb[index].process=p;
    pcb[index].process->lastRun=getClk();
    pcb[index].process->WT=getClk()-(p->AT); //Initialize the waiting time
    updatePCB(pcb,p,STARTED); //To be changed in phase2 
}

process_t* getProcessByID(pid_t pid){
    //Get the process info using the UNIX pid
    for(int i=0;i<recievedProcesses;i++)
        if(pcb[i].pid==pid)
            return pcb[i].process;
    return NULL;
}

void stopProcess(process_t* p)
{
    runningProcess=NULL;
    updatePCB(pcb,p,STOPPED);
    kill(getProcessID(p),SIGSTOP); //Send SIGSTOP to stop the process from execution
    updateOutfile(p);
}

void continueProcess(process_t* p)
{
    runningProcess=p;
    if(p->RT==p->RemT) //The first time to run this process
        startProcess(p);
    else
    {
        //It has run before
        if(pcb[p->ID].process->RemT<=0) //It has finished(wouldn't be handled with the SIGCHLD ??) 
            updatePCB(pcb,p,FINISHED);
        else
        {
            updatePCB(pcb,p,RESUMED);
            kill(getProcessID(p),SIGCONT);
        }
        updateOutfile(p);
    }
}

void updateOutfile(process_t* p)
{
    FILE* file=fopen("Scheduler.log","a");
    if(!file)
    {
        printf("Error in opening the file.. ");
        exit(-1);
    }
    char st[10];
    switch (p->state)
    {
    case STARTED:
        strcpy(st,"started");
        break;
    case ARRIVED:
        strcpy(st,"arrived");
        break;
    case STOPPED:
        strcpy(st,"stopped");
        break;
    case FINISHED:
        strcpy(st,"finished");
        break;
    case RESUMED:
        strcpy(st,"resumed");
        break;
    }
    fprintf(file, "AT time %d process %d %s total %d remain %d wait %d",getClk(), p->ID,st,p->RT,p->RemT,p->WT);
    if(p->state==FINISHED)
    {
        fprintf(file," TA %d WTA %.2f",p->TAT,p->WTAT);
    }
    fprintf(file,"\n");
    fclose(file);
}

pid_t getProcessID(process_t* p)
{
    return pcb[(p->ID)-1].pid;
}

void updatePCB(pcb_t* pcb,process_t* p,state_t state) 
{
    //For phase 1 -> ARRIVED==STARTED (since this function is only invoked if the process is about to get executed)
    //There's not a case where the process can
    int index=(p->ID)-1;
    p->state=state;
    switch (state)
    {
        case STARTED: //For phase 2 (I guess in this phase I think start process do this)
        {
            break;
        }
        case RESUMED:
        {
            //Set this timestamp as the last time this process has run
            p->lastRun=getClk();
            int busyTime=(p->RT)-(p->RemT);
            p->WT=getClk()-busyTime-(p->AT);
            break;
        }
        case STOPPED:
        {
            //Decrement the remaining time from the last time this process has run
            int elapsed=getClk()-(p->lastRun);
            p->RemT-=elapsed;
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
        // case ARRIVED:
    // //case WAITING: (phase2)
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
}


void generatePrefFile()
{
    FILE *file=fopen("Scheduler.pref","w");
    if(!file)
    {
        printf("can't open the file\n");
        exit(-1);
    }
    float cpuUtil = (totalRT*100) / (float)getClk();
    float StdWTAT = (float)sqrt(totalWTAT/ (float)(totalProcesses - 1)); 
    fprintf(file, "CPU utilization = %.2f %%\n", cpuUtil);
    fprintf(file, "Avg WTA = %.2f\n", totalWTAT/totalProcesses);
    fprintf(file, "Avg Waiting = %.2f\n", (float)totalWT/totalProcesses);
    fprintf(file, "Std WTA = %.2f\n", StdWTAT);
    fclose(file);
}


void finishProcessHandler(int signum)
{
    //NOTE: NOT FINISHED YET!!!
    int exitCode;
    wait(&exitCode);
    if (WIFEXITED(exitCode)) {
        int processID=WEXITSTATUS(exitCode);
        printf("process with id %d has ended at time clock %d \n",processID, getClk());
        //Get the process that has finished
        process_t* p=pcb[processID-1].process;
        updatePCB(pcb,p,FINISHED); //Update the pcb (state & TAT & WTAT)
        totalWT+=p->WT; //Updating the total waiting time
        totalRT+=p->RT; //Updating the total running time
        totalWTAT+=p->WTAT; //Updating the total weighted turnaround time
        finishedProcesses++; //Increment the finished processes count
        updateOutfile(p); //Update the output file 
    }
    else{
        perror("Something went wrong with the exit code of a process.");
    }
    

    runningProcess=NULL; //Free the running process to choose the next one
    //Reassign the SIGCHLD signal to this function as a handler
    signal(SIGUSR1,finishProcessHandler);

}
void destroyPCB()
{
    free(pcb);
}
void SRTN_Algo()
{

        minHeap_t PQ=initializeHeap(SRTN);
    system("clear");
    printf("(--------------------SRTN ALGORITHM--------------------)\n");
    while(finishedProcesses<totalProcesses) //Loop until all the processes are finished
    {
        //Checking for arriving processes
        while(checkNewProcesses(&PQ))
        { 
            printHeap(&PQ);
        }

        process_t* shortestProcess=getMin(&PQ);
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
            //If there is a running process, check it's the shortest one
            // if it isn't the shortest, stop the running one and run the shortest. 
            if((shortestProcess!=NULL) && (runningProcess->RemT > shortestProcess->RemT))
            {
                pop(&PQ);
                push(&PQ,*runningProcess);
                stopProcess(runningProcess);
                continueProcess(shortestProcess);
            }  
        }
    }
    destroyHeap(&PQ); //Deallocate the processes container
}

void HPF_Algo()
{

    minHeap_t PQ=initializeHeap(HPF);
    system("clear");
    printf("(Finished processes = %d , total Processes = %d )\n",finishedProcesses,totalProcesses); 
    printf("(--------------------HPF ALGORITHM--------------------)\n"); 
    ////////////Scheduling Algo implementation//////////////
    sleep(2);
    while(finishedProcesses < totalProcesses)
    {
        process_t* topPriority = runningProcess;
        while(checkNewProcesses(&PQ)){
            printHeap(&PQ);
        }
        
        //No process is running so just run the one with the higest priority one if it exists
        if(runningProcess==NULL)
        {
            process_t* topPriority=getMin(&PQ);
            if(topPriority!=NULL)
            {
                pop(&PQ);
                continueProcess(topPriority);
            }
        }
    }
    destroyHeap(&PQ); //Deallocate the processes container
}

void RR_Algo(char* timeSlice)
{
    printf("RR\n");
    queue_t ProcessQueue;
    process_t P;
    msgbuf revievingProcess;
    initializeQueue(&ProcessQueue);    
    printf("the number of processes is : %d\n", totalProcesses);

    ////////////Scheduling Algo implementation//////////////

    printf("the scheduler has finished in time : %d\n", getClk());
    destroyQueue(&ProcessQueue);
}
