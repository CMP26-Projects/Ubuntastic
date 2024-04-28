#include "scheduler.h"
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include<math.h>
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
        RR_Algo(atoi(argv[4]));
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
    runningP=NULL;
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
        printf("Process %d was sent to the scheduler succesfully at time clock %d \n",P->ID,getClk());
        lastRecieved=P;
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
        runningP=p;
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
    updatePCB(p,STARTED); //To be changed in phase2 
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
    runningP=NULL;
    updatePCB(p,STOPPED);
    kill(getProcessID(p),SIGTSTP); //Send SIGSTOP to stop the process from execution
    updateOutfile(p);
}

void continueProcess(process_t* p)
{
    runningP=p;
    if(p->RT==p->RemT) //The first time to run this process
        startProcess(p);
    else
    {
        //It has run before
        if(pcb[p->ID].process->RemT<=0) //It has finished(wouldn't be handled with the SIGCHLD ??) 
            updatePCB(p,FINISHED);
        else
        {
            updatePCB(p,RESUMED);
            kill(getProcessID(p),SIGCONT);
        }
        updateOutfile(p);
    }
}
void updateOutfile(process_t* p)
{
    FILE* file = fopen("Scheduler.log", "a");
    if (!file)
    {
        printf("Error in opening the file.. ");
        exit(-1);
    }

    char st[10];
    switch (p->state)
    {
    case STARTED:
        strcpy(st, "started");
        break;
    case ARRIVED:
        strcpy(st, "arrived");
        break;
    case STOPPED:
        strcpy(st, "stopped");
        break;
    case FINISHED:
        strcpy(st, "finished");
        break;
    case RESUMED:
        strcpy(st, "resumed");
        break;
    }

    // Emoji representations
    const char* emoji;
    switch (p->state)
    {
    case STARTED:
        emoji = "⏳"; // Hourglass emoji for "started"
        break;
    case ARRIVED:
        emoji = "🚀"; // Rocket emoji for "arrived"
        break;
    case STOPPED:
        emoji = "⛔️"; // Stop sign emoji for "stopped"
        break;
    case FINISHED:
        emoji = "✅"; // Check mark emoji for "finished"
        break;
    case RESUMED:
        emoji = "▶️"; // Play button emoji for "resumed"
        break;
    default:
        emoji = ""; // Empty string if state is not recognized
        break;
    }

    fprintf(file, "AT time %d process %d %s %s %s total %d remain %d wait %d ", getClk(), p->ID, emoji, st, emoji, p->RT, p->RemT, p->WT);
    
    if (p->state == FINISHED)
    {
        fprintf(file, "TA %d WTA %.2f", p->TAT, p->WTAT);
    }
    fprintf(file, "\n");
    fclose(file);
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

pid_t getProcessID(process_t* p)
{
    return pcb[(p->ID)-1].pid;
}

void updatePCB(process_t* p,state_t state) 
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
        updatePCB(p,FINISHED); //Update the pcb (state & TAT & WTAT)
        totalWT+=p->WT; //Updating the total waiting time
        totalRT+=p->RT; //Updating the total running time
        totalWTAT+=p->WTAT; //Updating the total weighted turnaround time
        finishedProcesses++; //Increment the finished processes count
        updateOutfile(p); //Update the output file 
    }
    else{
        perror("Something went wrong with the exit code of a process.");
    }
    

    runningP=NULL; //Free the running process to choose the next one
    //Reassign the SIGCHLD signal to this function as a handler
    signal(SIGUSR1,finishProcessHandler);

}
void destroyPCB()
{
    free(pcb);
}
void SRTN_Algo()
{
    minHeap_t PQ = initializeHeap(SRTN);
    system("clear");
    printf("\033[0;34m"); // ANSI escape code for blue color
    printf("(--------------------SRTN ALGORITHM--------------------)\n");
    printf("\033[0m");    // ANSI escape code to reset color
    while (finishedProcesses < totalProcesses) //Loop until all the processes are finished
    {
        // Checking for arriving processes
        while (checkNewProcesses(&PQ))
        {
            // Print the heap after inserting a new process
            printHeap(&PQ);
        }

        // Print the running process
        printProcess(runningP);

        // Get the shortest process in the heap
        process_t* shortestP = getMin(&PQ);

        // No process is running, so run the shortest one if it exists
        if (runningP == NULL && shortestP != NULL)
        {
            pop(&PQ);
            continueProcess(shortestP);
        }
        else if (runningP != NULL && shortestP != NULL)
        {
            // If there is a running process, check if it's the shortest one
            if (shortestP->RemT < runningP->RemT)
            {
                // Stop the running process and run the shortest one
                stopProcess(runningP);
                push(&PQ, *runningP);
                continueProcess(shortestP);
            }
        }
    }
    destroyHeap(&PQ); //Deallocate the processes container
}

void HPF_Algo()
{
    minHeap_t PQ=initializeHeap(HPF);
    system("clear");
    printf("\033[0;34m"); // ANSI escape code for blue color
    printf("(--------------------HPF ALGORITHM--------------------)\n");
    printf("\033[0m"); // ANSI escape code to reset color
    ////////////Scheduling Algo implementation//////////////
    while(finishedProcesses < totalProcesses)
    {
        process_t* topPriority = runningP;
        while(checkNewProcesses(&PQ)){
            printHeap(&PQ);
        }
        
        //No process is running so just run the one with the higest priority one if it exists
        if(runningP==NULL)
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

void RR_Algo(int timeSlice)
{
    system("clear");
    printf("\033[0;34m"); // ANSI escape code for blue color
    printf("(--------------------RR ALGORITHM--------------------)\n"); 
    printf("\033[0m"); // ANSI escape code to reset color
    process_t* P;
    queue_t ProcessQueue;
    initializeQueue(&ProcessQueue);
    ////////////Scheduling Algo implementation//////////////
    while (finishedProcesses < totalProcesses)
    {
        // Checking for arriving processes
        while (checkNewProcesses(&ProcessQueue))
        {
            printQueue(&ProcessQueue);
        }
        //If there isn't any running processes, run the next process
        if (runningP == NULL && !isEmpty(&ProcessQueue))
        {
            dequeue(&ProcessQueue, &P);
            continueProcess(P);
        }
        else if(runningP != NULL && (getClk()-(runningP->lastRun)>=timeSlice)) //There is a running process, check time slice
        {
            P=runningP;
            stopProcess(P);    //Send a STOP signal to the process
            enqueue(&ProcessQueue, *P); //Put it in the queue
            if (!isEmpty(&ProcessQueue)) //If there is any processes in queue, run it
            {
                dequeue(&ProcessQueue, &P);
                continueProcess(P);
            }
            // if (runningProcess->RemT == 0)
            // {
            //     stopProcess(runningProcess);
            // }
            // else if (getClk() - runningProcess->AT == atoi(timeSlice))
            // {
            //     stopProcess(runningProcess);
            //     enqueue(&ProcessQueue, *runningProcess);
            //     if (!isEmpty(&ProcessQueue))
            //     {
            //         dequeue(&ProcessQueue, &P);
            //         startProcess(P);
            //     }
            // }
        }
    }
    destroyQueue(&ProcessQueue);
}

