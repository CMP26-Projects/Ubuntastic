#include "scheduler.h"
int msgid;
bool receivingFlag=true;

int main(int argc, char *argv[])
{
    //Set the signals handlers
    signal(SIGUSR1,receiveProcesses);
    signal(SIGUSR2,finishProcess);
    signal(SIGINT,finishScheduling);

    msgid = createMessageQueue();
    
    initClk();
    sch=createScheduler(argc,argv);
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
}

scheduler_t* createScheduler(int argc,char* args[])
{    
    scheduler_t* sc=(scheduler_t*)malloc(sizeof(scheduler_t));
    //Initalize the scheduler data members

    sc->pCount=atoi(args[1]);
    sc->algo=atoi(args[2]); 
    sc->receivedPCount=0;
    sc->finishedPCount=0;
    sc->runningP=NULL;
    sc->lastRecieved=NULL;
    sc->totalWT=0;
    sc->totalWTAT=0.0;
    sc->busyTime=0;
    
    sc->pcbList= createList();
    sc->PCB=(PCB_t*)malloc((sc->pCount+1)*sizeof(PCB_t));
    
    //Instantiate the connection with clk & Process_gen & processes

    switch(sc->algo)
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
        //Inform the user that the scheduler has been created with the chosen algorithm 
        // displayScheduler(sc->algo);
    return sc;
}


void receiveProcesses(int signum)
{
    processMsg msg;
    printf("receiving processes\n");
    while(true)
    {
        //Receive the message from the message queue
        int msgReceiving = msgrcv(msgid, &msg, sizeof(msg.data), 12,IPC_NOWAIT);
        //Check this process is a new one
        int x= getClk();

        if (msgReceiving == -1)
        {
            break;
        }
        //Create a process of the recieved data
        process_t* p = createProcess(msg.data);
        
        //Set this process as the last received one 
        printf("received process %d at timeClk%d\n",p->ID,getClk());
        printProcess(p,GRN);
        //Add the new process to the scheduler container  (MinHeap|Queue)
        insertIntoReady(p);

        //Increase the number of recieved processes
        sch->receivedPCount++;
        if(sch->algo==RR_t)
            printQueue(sch->readyContainer,RED);
        else
            printHeap(sch->readyContainer,RED);
    }
    printf("DONE receiving processes\n");

    receivingFlag = false; //We finished receiving all the processes of this time clock
    signal(SIGUSR1,receiveProcesses);
}

PCB_t* createPCB(pid_t id,process_t* p)
{
    PCB_t* pcb=(PCB_t*)malloc(sizeof(PCB_t));
    pcb->pid=id;
    pcb->process=p;
    // insertSlot(sch->pcbList,id,pcb);
    return pcb;
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
        char *args[] = {"./process.out", id, rt, (char *)NULL};
        execv(args[0],args);
        printError("Execl process has failed for creating the process\n");
        exit(-1);
    }
    else
    {
        sch->runningP=p;
        p->state=STARTED;
        // insertSlot(sch->pcbList,pid,p);
        sch->PCB[p->ID].process=p;        
        sch->PCB[p->ID].pid=pid;        
        updateOutfile(p);
    }
}

void stopProcess(process_t* p)
{
    p->state=STOPPED;
    insertIntoReady(p); //Insert the process into the ready container
    updateOutfile(p);   //Update the log file
    // PCB_t* pcb=getByProcess(sch->pcbList,p); //get the pcb pf this process
    // pid_t pid=pcb->pid; //get the real process ID
    kill(sch->PCB[p->ID].pid,SIGTSTP); //Send SIGSTOP to stop the process from execution
}

void continueProcess(process_t* p)
{
    sch->runningP=p;
    if(p->RT==p->RemT) //The first time to run this process
        startProcess(p);
    else
    {
        //It has run before        else
        int busyTime=(p->RT)-(p->RemT); //Get the total time this process has run
        p->WT=getClk()-busyTime-(p->AT); //Update the waiting time of the process
        p->lastRun=getClk(); //Set the last time this process has run
        p->state=RESUMED;
        // PCB_t* pcb=getByProcess(sch->pcbList,p); //get the pcb pf this process
        // pid_t pid=pcb->pid; //get the real process ID
        kill(sch->PCB[p->ID].pid,SIGCONT); //Send SIGCONT to resume the process
        updateOutfile(p);
    }
}


float* calculateStatistics()
{
    float* schStatistics=(float*)malloc(4*sizeof(float));
    schStatistics[0] = (sch->busyTime*100) / (float)getClk(); //CPU_Utiliziation
    schStatistics[1] =sch->totalWTAT/sch->pCount; //Avg_WTA
    schStatistics[2] =(float)sch->totalWT/sch->pCount; //Avg_Waiting
    schStatistics[3] = (float)sqrt(sch->totalWTAT/ (float)(sch->pCount- 1)); //StdWTAT
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
    process_t* p= sch->runningP; //Update the state of the process
    p->TAT=getClk()-p->AT; //Set the turnaround time of the process
    p->WTAT=(float)p->TAT/p->RT; //Set the weighted turnaround time of the process
    p->WT=getClk()-(p->RT)-(p->AT); //Update the waiting time of the process
    p->state=FINISHED; //Update the state
    sch->totalWT+=p->WT; //Updating the total waiting time
    sch->busyTime+=p->RT; //Updating the total running time
    sch->totalWTAT+=p->WTAT; //Updating the total weighted turnaround time
    sch->finishedPCount++; //Increment the finished processes count
    updateOutfile(p); //Update the output file 
    free(sch->PCB[p->ID].process);
    sch->runningP=NULL; //Free the running process to choose the next one
    printf("end finish process\n");
    if(sch->finishedPCount==sch->pCount)
    {
        printf("FINISHED at timeClk =%d\n",getClk());
        sleep(1);
        finishScheduling(0);
    }
    //Reset the SIGCHLD signal to this function as a handler
    signal(SIGUSR2,finishProcess);
}

void SRTNAlgo()
{
    int lastClk = getClk();
    while (sch->finishedPCount < sch->pCount)
    {

        if (lastClk == getClk()) //It's the same timeclk, so no need to process anything
            continue;
        

        lastClk++;
        while (receivingFlag); //Wait to get all the processes arriving at this time stamp 
        receivingFlag = true; //Reset the flag to true to receive the new processes at the next time stamp

        process_t* shortest = getNextReady();

        if(!isReadyEmpty())
        {

            if(sch->runningP!=NULL && sch->runningP->state != FINISHED && shortest->RemT < sch->runningP->RemT)
            {
                stopProcess(sch->runningP);
                continueProcess(sch->runningP);
                removeFromReady();
            }
            else if(sch->runningP==NULL)
            {
                continueProcess(shortest);
                removeFromReady();
            }
            else
            {
                sch->runningP->RemT--; //Decrement the remaining time of the running process
            }
        }
        else
        {

            if(sch->runningP!=NULL)
            {
                sch->runningP->RemT--; //Decrement the remaining time of the running process
            }
        }

    }
}

void HPFAlgo()
{
    int lastClk = getClk();
    while (sch->finishedPCount < sch->pCount)
    {
        if (lastClk == getClk()) //It's the same timeclk, so no need to process anything
            continue;
        while (receivingFlag); //Wait to get all the processes arriving at this time stamp 
        receivingFlag = true; //Reset the flag to true to receive the new processes at the next time stamp

        printf("pCount= %d pFinished= %d\n",sch->finishedPCount , sch->pCount);   
        if (isReadyEmpty() && sch->runningP==NULL) //There is no ready processes to run, so no need to process anything
            continue;


        if (sch->runningP==NULL) //There is no process running process, so run the next ready process if exists.
        {
            process_t* shortest = getNextReady();
            removeFromReady();
            continueProcess(shortest);
        }
        else
        {
            sch->runningP->RemT--; //Decrement the remaining time of the running process
        }
        lastClk++;
    }
    printf("FINISHED\n");
    sleep(1);
}

void RRAlgo(int timeSlice)
{
    int lastClk = getClk();
    while (sch->finishedPCount < sch->pCount)
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
    if(sch->algo==RR_t)
        enqueue(sch->readyContainer,p);
    else 
        insert(sch->readyContainer,p);
}

void removeFromReady()
{
    if(sch->algo==RR_t)
    {
        dequeue(sch->readyContainer);
    }
    else
    {
        deleteMin(sch->readyContainer);
    }
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

void finishScheduling(int signum)
{
    destroyReady();
    float* schStatistics=calculateStatistics();
    generatePrefFile(schStatistics);
    destroyClk(true);
}