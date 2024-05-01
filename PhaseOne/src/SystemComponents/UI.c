#include "UI.h"
queue_t* readFile(char* filePath, queue_t* Pqueue)
{
    int numProcesses = 0;
    //Initialize the Process Queue

    int id, runTime, arrivalTime, priority;
    char firstLine[100];
    FILE *inputFile = fopen(filePath, "r");
    if (!inputFile)
    {
        printError("Error in opening the processes.txt file\n");
        return NULL;
    }
    fgets(firstLine, sizeof(firstLine), inputFile); //Ignoring the first line

    while (fscanf(inputFile, "%d\t%d\t%d\t%d\n", &id, &arrivalTime, &runTime, &priority) == 4)
    {
        //Read the process data and creating a process
         
        int info[]={id,arrivalTime,runTime,priority};
        process_t* pd = createProcess(info);
        enqueue(Pqueue,pd);
        numProcesses++;
    }
    fclose(inputFile);
    //Testing
    #ifdef DEBUG
        printLine("The processes are read from the file\n",GRN);
        printQueue(Pqueue,MAG);
    #endif
    printf("Number of processes: %d\n", numProcesses);
    int info[]={0,0,0,0};
    process_t* dummy = createProcess(info);
    enqueue(Pqueue,dummy);
    return Pqueue;
}


void getUserInput(int *schedAlgo, int *timeSlice) //Will get updated and support GUI 
{
    Get_Scheduling_Algorithm: 
    system("clear"); //Clear the terminal
    printLine("|| Choose a Scheduling Algorithm ||\n",BLU);
    printLine("For SRTN, Enter 0\nFor HPF, Enter 1\nFor RR, Enter 2\n ",GRN);
    printLine("Algorithm: ",WHT);
    scanf("%d", schedAlgo);
    if (*schedAlgo> 2)
    {
        printLine("Invalid Option ->\n",RED);
        sleepMilliseconds(500); 
        goto Get_Scheduling_Algorithm;
    }


    if(*schedAlgo == 2)
    {
        //For RR get the time slice
        Get_RR_timeSlice:
            printLine("||Enter the Time Slice||\nTime: ",BLU);
            scanf("%d", timeSlice);
            if (*timeSlice < 0)
            {
                printLine("\nInvalid Option -> ",RED);
                goto Get_RR_timeSlice;
            }
    }
    system("clear");
}
void displayScheduler(int algo)
{
    printLine("The Scheduler Algorithm is: ",BLU);
    system("clear");
    char* Line="--------------------SCHEDULER ALGORITHM--------------------";
    switch (algo)
    {
    case RR_t:
        sprintf(Line,"--------------------RR ALGORITHM--------------------\n");
        printLine(Line,BLU);
        break;
    case HPF_t:
        sprintf(Line,"--------------------HPF ALGORITHM--------------------\n");
        printLine(Line,BLU);
        break;
    case SRTN_t:
        sprintf(Line,"--------------------SRTN ALGORITHM--------------------\n");
        printLine(Line,BLU);
        break;
    default:
        printLine("INVALID SCHEDULING ALGORITHM\n",RED);
        break;
    }
}

void printLine(char line[],char* clr)
{
    printf("%s%s%s\n", clr,line,NRM);

}

void print(char line[],char* clr)
{
    printf("%s%s%s", clr,line,NRM);
}

void printError(char errorMsg[])
{
    perror(errorMsg);
}

void insertIntoLog(state_t state,float* pInfo)
{
    FILE* file = fopen("Scheduler.log", "a");
    if (!file)
    {
        printf("Error in opening the file.. ");
        exit(-1);
    }

    char st[30];
    switch (state)
    {
    case STARTED:
        strcpy(st, "⏳ started ⏳");
        break;
    case ARRIVED:
        strcpy(st, "🚀 arrived 🚀");
        break;
    case STOPPED:
        strcpy(st, "⛔️ stopped ⛔️");
        break;
    case FINISHED:
        strcpy(st, "✅ finished ✅");
        break;
    case RESUMED:
        strcpy(st, "▶️ resumed ▶️");
        break;
    }
    fprintf(file, "AT time %d process %d %s total %d remain %d wait %d ", getClk(), (int)pInfo[0], st, (int)pInfo[1], (int)pInfo[2], (int)pInfo[3]);
    
    if (state == FINISHED)
    {
        fprintf(file, "TA %d WTA %.2f",(int)pInfo[4] ,pInfo[5]);
    }
    fprintf(file, "\n");
    fclose(file);
}


void generatePrefFile(float* statistics)
{
    FILE *file=fopen("Scheduler.pref","w");
    if(!file)
    {
        printf("can't open the file\n");
        exit(-1);
    }

    printLine("The Scheduler Statistics:\n",BLU);
    printLine("CPU utilization = ",GRN);
    printf("%.2f %%\n", statistics[0]);
    printLine("Avg WTA = ",GRN);
    printf("%.2f\n", statistics[1]);
    printLine("Avg Waiting = ",GRN);
    printf("%.2f\n", statistics[2]);
    printLine("Std WTA = ",GRN);
    printf("%.2f\n", statistics[3]);
    fprintf(file, "CPU utilization = %.2f %%\n", statistics[0]);
    fprintf(file, "Avg WTA = %.2f\n", statistics[1]);
    fprintf(file, "Avg Waiting = %.2f\n", statistics[2]);
    fprintf(file, "Std WTA = %.2f\n", statistics[3]);
    fclose(file);
}


void printProcess(void* data, char* clr)
{
    process_t* temp=(process_t*)data;
    if(temp!=NULL)
    printf("%sID: %d, AT: %d, RT: %d, Priority: %d, RemT: %d State:%s",clr,temp->ID, temp->AT, temp->RT, temp->priority, temp->RemT,RED);
    switch (temp->state)
    {
    case FINISHED:
        printf("FINISHED");
        break;
    
    case WAITING:
        printf("WAITING");
        break;
    
    case RESUMED:
        printf("RESUMED");
        break;
    
    case STOPPED:
        printf("STOPPED");
        break;
    
    case ARRIVED:
        printf("ARRIVED");
        break;
            
    default:
        printf("STARTED");
        break;
    }
    printf("%s\n",NRM);
}

void printQueue(queue_t* queue, char* clr)
{
    if(!isEmptyQueue(queue))
    {
        printLine("The Queue is Empty\n",RED);
        return;
    }
    printf("the queue size is %d and has:\n",queue->size);
    node_t* temp=queue->front;
    while(temp!=NULL)
    {
        printProcess(temp->data,clr);
        temp=temp->next;
    }
}


void printHeap(minHeap_t *heap, char* clr) 
{
        if(isEmptyHeap(heap))
        {
            printLine("The Heap is Empty\n",RED);
            return;
        }
    
    printf("The heap size is %d and has:\n",heap->size);
    for (int i = 0; i < heap->size; i++) 
    {
        printProcess(heap->arr[i],clr);
    }
}
