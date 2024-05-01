//=================================//
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
#include <math.h>
#include <time.h>
#include <stdbool.h>
#include <errno.h>

// Uncomment the following line to enable debugging
#define DEBUG
#define true 1
#define false 0
#define SRTN_t 0
#define HPF_t 1
#define RR_t 2
#define SHKEY 3000
#define LINE_SIZE 1000
typedef int clk_t;


///==============================
// don't mess with this variable//
static int *shmaddr; //
//===============================

///Time Functions///
clk_t getClk();
void initClk();
void sleepMilliseconds(long milliseconds);
void destroyClk(bool terminateAll);


/*
 * All process call this function at the beginning to establish communication between them and the clock module.
 * Again, remember that the clock is only emulation!
 */

//===============MessageQueue===============//

typedef struct
{
    long mtype;
    int data[4];
}processMsg;


//===============MessageQueue===============//


processMsg createMsg(int info[]);
int createMessageQueue();
int creatShMemory();