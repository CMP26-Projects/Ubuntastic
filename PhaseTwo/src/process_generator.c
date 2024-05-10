#include "UI.h"

typedef struct
{
    long mtype;
    int data[5];
} processMsg;

pid_t clk;
int msgid;
queue_t *pQueue;
void clearResources(int);

processMsg createProcessMessage(process_t *); // Create a message of the process data to send it to the scheduler

int main(int argc, char *argv[])
{
    system("clear");
    char *lineToPrint = (char *)malloc(LINE_SIZE * sizeof(char));
    int timeSlice = -1;
    int schedAlgo = atoi(argv[2]);
    if (argc > 3 && schedAlgo == RR_t)
        timeSlice = atoi(argv[3]);

    msgid = createMessageQueue();
    signal(SIGUSR2, clearResources); // If it gets interrupted, clear the resources

    pQueue = createQueue();
    pQueue = readFile(argv[1], pQueue);
    int totalProcesses = --pQueue->size;
    sprintf(lineToPrint, "The total number of processes is %d\n", totalProcesses);
    printLine(lineToPrint, GRN);

    // Initiate and  create clock process.
    clk = fork();
    if (clk == -1)
    {
        printError("Error in Forking the Clock\n");
        exit(-1);
    }
    else if (clk == 0)
    {
#ifdef DEBUG
        sprintf(lineToPrint, "I'm the clock after being forked\n");
        printLine(lineToPrint, GRN);
#endif
        char *args[] = {"./clk.out", (char *)NULL};
        execv(args[0], args);
        printError("Execl process has failed for creating the clock\n");
        exit(-1);
    }
    else
    {

        // Initialize the clock
        // Initiate and create scheduler process.
        pid_t scheduler = fork();
        if (scheduler == -1)
        {
            printError("Error in Forking the scheduler\n");
            exit(-1);
        }
        else if (scheduler == 0)
        {

            // Read the scheduler input to send it
            char n[5], s[5], t[5];
            sprintf(n, "%d", totalProcesses);
            sprintf(s, "%d", schedAlgo);
            sprintf(t, "%d", timeSlice);

#ifdef DEBUG
            sprintf(lineToPrint, "I'm the scheduler after being forked\n");
            printLine(lineToPrint, GRN);
#endif
            char *args[] = {"./scheduler.out", n, s, t, (char *)NULL};
            execv(args[0], args);
            printError("Execl process has failed for creating the scheduler\n");
            exit(-1);
        }
        else
        {
            initClk();
            clk_t lastClk = -1;
            process_t *P = front(pQueue);
            bool flag=false;
            while (true)
            {
                if (getClk() == lastClk)
                    continue;
                lastClk=getClk();
                while (!isEmptyQueue(pQueue) && P != NULL && lastClk >= P->AT && P->ID != 0)
                {
                    processMsg sendingProcess = createProcessMessage(P);
                    int msgSending = msgsnd(msgid, &sendingProcess, sizeof(sendingProcess.data), !IPC_NOWAIT);

                    if (msgSending == -1)
                        printError("No message to send\n");

                    dequeue(pQueue);
                    P = front(pQueue);
                }
                kill(scheduler, SIGUSR1);
            }
        // Waits for the scheduler to terminate
        #ifdef DEBUG
            sprintf(lineToPrint, "generator has finished sending and the queue size is %d\n", pQueue->size);
            printLine(lineToPrint, GRN);
            printProcess(P, YEL);
        #endif
        }
    }
}

processMsg createProcessMessage(process_t *P)
{
    processMsg sendingProcess;
    sendingProcess.mtype = 12;
    sendingProcess.data[0] = P->ID;
    sendingProcess.data[1] = P->AT;
    sendingProcess.data[2] = P->RT;
    sendingProcess.data[3] = P->priority;
    sendingProcess.data[4] = P->size;
    return sendingProcess;
}

void clearResources(int signum)
{
    // Delete the processes shared memory
    msgctl(msgid, IPC_RMID, (struct msqid_ds *)0);
    destroyQueue(pQueue);
    destroyClk(false);
    kill(clk, SIGINT);
    exit(0);
}