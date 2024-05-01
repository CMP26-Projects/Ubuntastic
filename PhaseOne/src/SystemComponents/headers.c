#include "headers.h"
///Time Functions///
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

void sleepMilliseconds(long milliseconds) {
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

//===============MessageQueue===============//


processMsg createMsg(int info[])
{
    processMsg msg;
    msg.mtype=7;
    msg.data[0]=info[0];
    msg.data[1]=info[1];
    msg.data[2]=info[2];
    msg.data[3]=info[3];
    return msg;
}
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
