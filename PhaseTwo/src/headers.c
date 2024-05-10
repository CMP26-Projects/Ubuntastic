#include "headers.h"
/// Time Functions///
clk_t getClk()
{
    return *shmaddr;
}

void initClk()
{
    int shmid = shmget(SHKEY, 4, 0444);
    while ((int)shmid == -1)
    {
        // Make sure that the clock exists
        printf("Wait! The clock not initialized yet!\n");
        shmid = shmget(SHKEY, 4, 0444);
        sleep(1);
    }
    shmaddr = (int *)shmat(shmid, (void *)0, 0);
}

void sleepMilliseconds(long milliseconds)
{
    struct timespec ts;
    ts.tv_sec = milliseconds / 1000;
    ts.tv_nsec = (milliseconds % 1000) * 1000000;
    nanosleep(&ts, NULL);
}

void destroyClk(bool terminateAll)
{
    shmdt(shmaddr);
    if (terminateAll)
    {
        killpg(getpgrp(), SIGINT);
    }
}

process_t *createProcess(int *info)
{
    process_t *pd = (process_t *)malloc(sizeof(process_t));
    pd->ID = info[0];
    pd->AT = info[1];
    pd->RT = info[2];
    pd->priority = info[3];
    pd->size = info[4];
    pd->RemT = info[2];
    pd->interval = NULL;
    pd->state = WAITING;
    pd->lastRun = -1;
    pd->PID = -1;
    pd->WT = 0;
    pd->TAT = 0;
    pd->WTAT = 0;
    return pd;
}

//===============MessageQueue===============//

int createMessageQueue()
{
    key_t key_id;
    int msgid;

    key_id = ftok("processes.txt", 65);
    msgid = msgget(key_id, 0666 | IPC_CREAT);

    if (msgid == -1)
    {
        perror("there error in creates a queue !");
        exit(-1);
    }
    return msgid;
}

int creatShMemory()
{
    key_t key = ftok("file.txt", 67);
    int shmid = shmget(key, 50, IPC_CREAT | 0666);
    if ((long)shmid == -1)
    {

        perror("Error in creating shm!");
        exit(-1);
    }

    return shmid;
}
