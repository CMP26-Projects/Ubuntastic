#include "headers.h"
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

struct schdularType
{
    int mtype;
    char schedType[70];
};
struct processData
{

    int id, arrival, runtime, priority;
};

void HandlerINT(int signum)
{
}
void HandlerCHILD(int signum)
{
}
void generateLogFile()
{
}
void generatePrefFile()
{
}
void createProcess()
{
}
void getState()
{
}
void updateOutfile()
{
}

int createMessageQueue()
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

int main(int argc, char *argv[])
{
    // Schedular Attribute
    int totalProcessNum, activateProcess;
    // initiate Clock
    // initClk();
    // Signals Handlers
    // signal(SIGINT,HandlerINT);
    // signal(SIGCHLD,HandlerCHILD);

    /// get type of schedular from process_generator using message queue

    struct schdularType schedularmessage;
    int done;

    int msgid = createMessageQueue();
    done = msgrcv(msgid, &schedularmessage, sizeof(schedularmessage.schedType), 0, !IPC_NOWAIT);

    if (done == -1)
    {
        perror("error in reciving ?");
    }
    else
    {
        printf("the message reseved in : %s\n", schedularmessage.schedType);
    }
    msgctl(msgid, IPC_RMID, (struct msqid_ds *)0);

    struct processData *P = (struct processData *)malloc(sizeof(struct processData));

    int shmid = creatShMemory();
    void *shmaddr = shmat(shmid, (void *)0, 0);
    if (shmaddr == (void *)-1)
    {
        perror("Error in attach in reader");
        exit(-1);
    }

    printf("\nReader: Shared memory attached at address %p\n", shmaddr);

    P = (struct processData *)shmaddr;

    printf("the message reseved in : %d %d %d %d \n", P->arrival, P->id, P->priority, P->runtime);

    strcpy((char *)shmaddr, "quit");
    shmdt(shmaddr);

    // TODO implement the scheduler :)
    // upon termination release the clock resources.

    // destroyClk(true);
    return 0;
}
