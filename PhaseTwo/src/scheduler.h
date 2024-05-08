
#include "UI.h"
#include "ds/memory.h"
typedef struct
{
    long mtype;
    int data[6];
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
    int* WTATList;
    float totalWTAT;
    void *readyContainer;
    void *waitingContainer;
    int timeSlice;
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

// Statistics Functions
void updateOutfile(process_t *p);
float *calculateStatistics();
int generateOutputImages();

// Ready Container Functions
void insertIntoReady(process_t *p);
void insertIntoWait(process_t *p);

void removeFromReady();
void removeFromWait();

process_t *getNextReady();
process_t *getNextWait();

bool isReadyEmpty();
bool isWaitEmpty();

void destroyWait();
void destroyReady();

// Scheduling Algorithms Functions
void SRTNAlgo();
void HPFAlgo();
void RRAlgo(int);