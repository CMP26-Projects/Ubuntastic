#include "memory.h"
typedef struct
{
    long mtype;
    int data[5];
} processMsg;

typedef struct Scheduler
{
    memory_t *memory;
    process_t *runningP;
    process_t *lastRecieved;
    int algo;
    int busyTime;
    int finishedPCount;
    int receivedPCount;
    int pCount;
    int totalWT;
    float totalWTAT;
    int* WTATList;
    void *readyContainer;
    void *waitingContainer;
    process_t **PCB;
} scheduler_t;

scheduler_t *sch;

// Scheduler Functions
scheduler_t *createScheduler(int argc, char *argv[]);
void finishScheduling(int signum);

// Scheduling Functions
void receiveProcesses(int signum);
void startProcess(process_t *p);
void stopProcess(process_t *p);
void continueProcess(process_t *p);
void finishProcess(int signum);

//Statistics Functions
void updateOutfile(process_t* p);
float calculateAvgWTA();
float calculateStdWTA(float avgWTA);
float* calculateStatistics();

// Ready Container Functions
void insertIntoReady(process_t *p);
void removeFromReady();
process_t *getNextReady();
bool isReadyEmpty();
void destroyReady();

//Memory Container Functions
void insertIntoWait(process_t *p);
void removeFromWait();
process_t *getNextWait();
bool isWaitEmpty();
void destroyWait();
void checkWaiting();

// Scheduling Algorithms Functions
void SRTNAlgo();
void HPFAlgo();
void RRAlgo(int);