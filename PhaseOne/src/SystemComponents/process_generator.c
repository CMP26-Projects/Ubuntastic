#include "UI.h"
#include "../dataStructures/queue.h"

int msgid;
queue_t* pQueue;
void clearResources(int);
processMsg createProcessMessage(process_t*); //Create a message of the process data to send it to the scheduler

int main(int argc, char *argv[])
{
    char* lineToPrint=(char*)malloc(LINE_SIZE*sizeof(char));
    // int schedAlgo=atoi(argv[2]);
    // if(argc>3&&schedAlgo==RR)
    // timeSlice=atoi(argv[3]);

    msgid = createMessageQueue();
    int timeSlice=-1,schedAlgo=5;
    signal(SIGINT, clearResources); //If it gets interrupted, clear the resources 

    pQueue=createQueue();
    //Read the input files.
    pQueue = readFile(argv[1], pQueue);
    //Ask the user for the chosen scheduling algorithm and its parameters, if there are any.
    getUserInput(&schedAlgo, &timeSlice);
    
    int totalProcesses = pQueue->size;
    #ifdef DEBUG
        sprintf(lineToPrint,"The total number of processes is %d\n",totalProcesses);
        printLine(lineToPrint,GRN);
    #endif
    
    //Initiate and create clock process.    
    pid_t clk = fork();
    if (clk == -1)
    {
        printError("Error in Forking the Clock\n");
        exit(-1);
    }
    else if (clk == 0)
    {
        #ifdef DEBUG
            sprintf(lineToPrint,"I'm the clock after being forked\n");
            printLine(lineToPrint,GRN);
        #endif
        char *args[] = {"./clk.out",(char *)NULL};
        execv(args[0],args);
        printError("Execl process has failed for creating the clock\n");
        exit(-1);
    }
    else
    {

        //Initialize the clock
        //Initiate and create scheduler process.
        pid_t scheduler = fork();
        if (scheduler == -1)
        {
            printError("Error in Forking the scheduler\n");
            exit(-1);
        }
        else if (scheduler == 0)
        {

            //Read the scheduler input to send it
            char n[5], s[5], sw[5], t[5];
            sprintf(n, "%d", totalProcesses-1);
            sprintf(s, "%d", schedAlgo);
            sprintf(t, "%d", timeSlice);

            #ifdef DEBUG
                sprintf(lineToPrint,"I'm the scheduler after being forked\n");
                printLine(lineToPrint,GRN);
            #endif
            char *args[] = {"./scheduler.out", n, s, t, (char *)NULL};
            execv(args[0],args);
            printError("Execl process has failed for creating the scheduler\n");
            exit(-1);
        }
        else
        {   
            initClk();
            clk_t lastClk=getClk();
            process_t* P=front(pQueue);
            #ifdef DEBUG
                sprintf(lineToPrint,"I'm the generator after forking the clock with pid= %d",clk);
                print(lineToPrint,GRN);
                sprintf(lineToPrint,"and the scheduler with pid = %d\n",scheduler);
                print(lineToPrint,GRN);
                sprintf(lineToPrint,"The first process to be sent %d\n", getClk());
                printLine(lineToPrint,BLU);
                printProcess(P,YEL);
            #endif
            while (true)
            {
                clk_t curTime = getClk();
                
                if(curTime==lastClk)
                    continue;
                else
                {
                    
                #ifdef DEBUG
                    sprintf(lineToPrint,"generator at time clk %d\n", getClk());
                    printLine(lineToPrint,GRN);
                #endif
                    lastClk++;

                while (!isEmptyQueue(pQueue) &&P != NULL && curTime == P->AT)
                {
                    processMsg sendingProcess=createProcessMessage(P);
                    int msgSending = msgsnd(msgid, &sendingProcess, sizeof(sendingProcess.data),!IPC_NOWAIT);

                    if(msgSending==-1)
                    {
                        printf("error in sending\n");
                        printError("No message to send\n");
                    }
                    
                    #ifdef DEBUG
                        sprintf(lineToPrint,"generator sent process at time clk %d\n", getClk());
                        printLine(lineToPrint,GRN);
                        printProcess(P,YEL);
                    #endif
                    dequeue(pQueue);
                    P = front(pQueue);
                    #ifdef DEBUG
                        sprintf(lineToPrint,"The next process to be sent %d\n", getClk());
                        printLine(lineToPrint,GRN);
                        printProcess(P,YEL);
                    #endif
                }
                    kill(scheduler, SIGUSR1);
                }
                
            }
            //Waits for the scheduler to terminate
            #ifdef DEBUG
                        sprintf(lineToPrint,"generator has finished sending and the queue size is %d\n", pQueue->size);
                        printLine(lineToPrint,GRN);
                        printProcess(P,YEL);
            #endif
        }
    }
}

processMsg createProcessMessage(process_t* P)
{
    processMsg sendingProcess;
    sendingProcess.mtype = 12;
    sendingProcess.data[0] = P->ID;
    sendingProcess.data[1] = P->AT;
    sendingProcess.data[2] = P->RT;
    sendingProcess.data[3] = P->priority;
    return sendingProcess;
}

void clearResources(int signum)
{
    //Delete the processes shared memory
    msgctl(msgid, IPC_RMID, (struct msqid_ds *)0);
    destroyQueue(pQueue);
    exit(0);
}