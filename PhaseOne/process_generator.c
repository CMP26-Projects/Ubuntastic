#include "headers.h"
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "func.c"

struct schdularType
{
    int mtype;
    char schedType[70];
};
struct processData
{

    int id, arrival, runtime, priority;
};

struct messageProcess
{
    int mtype;
    struct processData Process;
};

int createMessageQueue1()
{
    key_t key_id;
    int msgid, recval;

    key_id = ftok("file.txt", 65);
    msgid = msgget(key_id, 0666 | IPC_CREAT);

    if (msgid == -1)
    {
        perror("there error in creates a queue !");
        exit(-1);
    }
    printf("Message queue ID is : %d \n", msgid);
    return msgid;
}

int creatShMemory()
{
    int key = ftok("file.txt", 67);
    int shmid = shmget(key, 50, IPC_CREAT | 0666);
    if ((long)shmid == -1)
    {
        perror("Error in creating shm!");
        exit(-1);
    }

    return shmid;
}
int Creatsem(int sem2)
{

    int keyid_sem = ftok("file.txt", 62);
    int keyid_sem = ftok("file.txt", 63);
    sem2 = semget(keyid_sem, 1, 0666 | IPC_CREAT);
    int sem = semget(keyid_sem, 1, 0666 | IPC_CREAT);
    if (sem == -1)
    {
        perror("Error in create sem");
        exit(-1);
    }
    return sem;
}
// void clearResources(int);

int main(int argc, char *argv[])
{
    // signal(SIGINT, clearResources);
    //  TODO Initialization
    //  1. Read the input files.
    //  2. Ask the user for the chosen scheduling algorithm and its parameters, if there are any.
    //  3. Initiate and create the scheduler and clock processes.
    //  4. Use this function after creating the clock process to initialize clock
    // initClk();
    // To get time use this
    // int x = getClk();
    // printf("current time is %d\n", x);
    int sem2;
    int sem1 = Creatsem(&sem2);

    struct schdularType schedularmessage;
    int done;
    strcpy(schedularmessage.schedType, "RR");

    int msgid1 = createMessageQueue1();
    done = msgsnd(msgid1, &schedularmessage, sizeof(schedularmessage.schedType), !IPC_NOWAIT);

        if (done == -1)
    {
        perror("error in reciving ?");
    }
    struct processData *P = (struct processData *)malloc(sizeof(struct processData));
    P->arrival = 1;
    P->id = 2;
    P->priority = 3;
    P->runtime = 4;

    int shmid = creatShMemory();

    void *shmaddr = shmat(shmid, NULL, 0);
    if (shmaddr == (void *)-1)
    {
        perror("Error in attach in writer");
        exit(-1);
    }

    printf("\nWriter: Shared memory attached at address %p\n", shmaddr);
    memcpy(shmaddr, P, sizeof(struct processData));

    shmdt(shmaddr);

    return 0;

    // TODO Generation Main Loop
    // 5. Create a data structure for processes and provide it with its parameters.
    // 6. Send the information to the scheduler at the appropriate time.
    // 7. Clear clock resources
    // destroyClk(true);
}

// void clearResources(int signum)
//{
// TODO Clears all resources in case of interruption
//}
