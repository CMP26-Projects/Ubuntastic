//=============Contents============//
//struct ProcessData
//struct Queue
//readFile()

#include "headers.h"

void clearResources(int);

//==============ProcessData==============//
struct ProcessData
{
    int ID;
    int BT;
    int AT;
    int Priority;
};
///////////////////////////////////////

//===============Queue===============//
// Define the structure for a queue node

struct QNode {
    struct ProcessData data;
    struct QNode* next;
};

// Define the structure for the queue itself
struct Queue {
    struct QNode* front;
    struct QNode* rear;
    int count;
};

bool isEmpty(struct Queue* q)
{
    return q->front == NULL;
}

void peek(struct Queue* q, struct ProcessData *p)
{
     if (q->front == NULL) {
        // Queue is empty, nothing to dequeue
        p=NULL;
        return;
    }
    *p=q->front->data;
}

// Initialize an empty queue
void initializeQueue(struct Queue* q) {
    q->front = q->rear = NULL;
}

// Enqueue operation: Add a new element to the rear of the queue
void enQueue(struct Queue* q, struct ProcessData x) {
    struct QNode* temp = (struct QNode*)malloc(sizeof(struct QNode));
    temp->data = x;
    temp->next = NULL;

    if (q->rear == NULL) {
        // If the queue is empty, set both front and rear to the new node
        q->front = q->rear = temp;
    } else {
        // Otherwise, add the new node after the current rear and update rear
        q->rear->next = temp;
        q->rear = temp;
    }
    (q->count)++;
}

// Dequeue operation: Remove the front element from the queue
bool deQueue(struct Queue* q, struct ProcessData *p) {
    if (q->front == NULL) {
        // Queue is empty, nothing to dequeue
        p=NULL;
        return 0;
    }

    struct QNode* temp = q->front;
    q->front = q->front->next;

    if (q->front == NULL) {
        // If the front becomes NULL, the queue is now empty, so update rear
        q->rear = NULL;
    }
    (q->count)--;
    *p = temp->data;
    free(temp);
   return 1;
}

//=========Read File=============//
void readFile(char *filePath, struct Queue *processData)
{   
    initializeQueue(processData);
    int id, bt, at, pri;
    char dummy[100]; 
    FILE *inputFile= fopen(filePath,"r");
    if(!inputFile)
        {
            printf("Error in opening the file\n");
            return;
        }
    fgets(dummy,sizeof(dummy),inputFile); //ignoring the first line

    while(fscanf(inputFile,"%d\t%d\t%d\t%d\n",&id,&bt,&at,&pri)==4)
        {
            struct ProcessData pd={id,bt,at,pri};
            enQueue(processData,pd);
        }

}

int main(int argc, char * argv[])
{
    signal(SIGINT, clearResources);
    // TODO Initialization

    // 1. Read the input files.
    struct Queue processData;
    readFile(argv[1],&processData);
    
    // 2. Ask the user for the chosen scheduling algorithm and its parameters, if there are any.
    int schedAlgo=-1;
    int switchTime=-1;
    int timeSlice=-1; //RR

    do
    {
        printf("|| Choose a Scheduling Algorithm ||\n");
        printf("For RR, Enter 0\nFor SRTF, Enter 1\nFor HPF, Enter 2\nAlgorithm: ");
        scanf("%d",&schedAlgo);
        if(schedAlgo != 0 && schedAlgo != 1 && schedAlgo != 2)
            printf("Invalid Option -> ");
    } while (schedAlgo != 0 && schedAlgo != 1 && schedAlgo != 2);
    
    do
    {
        printf("|| Enter the Context Switching Time ||\nTime: ");
        scanf("%d",&switchTime);
        if(switchTime<0)
            printf("\nInvalid Option -> ");
    } while (switchTime<0);
    

    switch (schedAlgo)
    {
    case 0:
        {
            printf("You've Chosen RR.\n");
            do
            {
                printf("||Enter the Time Slice||\nTime: ");
                scanf("%d",&switchTime);
                if(switchTime<0)
                    printf("\nInvalid Option -> ");
            } while (switchTime<0);

        }
        break;
    default:
        break;
    }

    // 3. Initiate and create the scheduler and clock processes.
    
    // 4. Use this function after creating the clock process to initialize clock
   
    initClk();
    // To get time use this
    int x = getClk();
    printf("current time is %d\n", x);
    // TODO Generation Main Loop
    // 5. Create a data structure for processes and provide it with its parameters.
    // 6. Send the information to the scheduler at the appropriate time.
    // 7. Clear clock resources
    destroyClk(true);
}

void clearResources(int signum)
{
    //TODO Clears all resources in case of interruption
}
