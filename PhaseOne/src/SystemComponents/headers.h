//=================================//
#pragma once
#include <stdio.h> //if you don't use scanf/printf change this include
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <time.h>
#define NRM  "\x1B[0m"
#define RED  "\x1B[31m"
#define GRN  "\x1B[32m"
#define YEL  "\x1B[33m"
#define BLU  "\x1B[34m"
#define MAG  "\x1B[35m"
#define CYN  "\x1B[36m"
#define WHT  "\x1B[37m"
#define true 1
#define false 0
#define SRTN_t 0
#define HPF_t 1
#define RR_t 2
#define SHKEY 3000
#define MAX_SIZE 10000
typedef short bool;
typedef int clk_t;

// Uncomment the following line to enable debugging
//#define DEBUG


///Time Functions///

///==============================
// don't mess with this variable//
int *shmaddr; //
//===============================

clk_t getClk()
{
    return *shmaddr;
}

/*
 * All process call this function at the beginning to establish communication between them and the clock module.
 * Again, remember that the clock is only emulation!
 */
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

/*
 * All process call this function at the end to release the communication
 * resources between them and the clock module.
 * Again, Remember that the clock is only emulation!
 * Input: terminateAll: a flag to indicate whether that this is the end of simulation.
 *                      It terminates the whole system and releases resources.
 */

void destroyClk(bool terminateAll)
{
    shmdt(shmaddr);
    if (terminateAll)
    {
        killpg(getpgrp(), SIGINT);
    }
}

void sleepMilliseconds(long milliseconds) {
    struct timespec ts;
    ts.tv_sec = milliseconds / 1000;
    ts.tv_nsec = (milliseconds % 1000) * 1000000;
    nanosleep(&ts, NULL);
}

//===============MessageQueue===============//

typedef struct
{
    long mtype;
    int data[4];
}processMsg;


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