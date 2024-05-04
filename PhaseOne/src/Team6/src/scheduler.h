#include "UI.h"
typedef struct
{
    long mtype;
    int data[4];
}processMsg;

typedef struct Scheduler{
int algo;
process_t* runningP;
process_t* lastRecieved;
int timeSlice;
int busyTime;
int finishedPCount;
int receivedPCount;
int pCount;
int totalWT;
float totalWTAT;
void* readyContainer;
process_t** PCB;
}scheduler_t;

scheduler_t* sch;

//Scheduler Functions
scheduler_t* createScheduler(int argc, char* argv[]);
void finishScheduling(int signum);

//Scheduling Functions
void receiveProcesses(int signum);
void startProcess(process_t* p);
void stopProcess(process_t* p);
void continueProcess(process_t* p);
void finishProcess(int signum);

//Statistics Functions
void updateOutfile(process_t* p);
float* calculateStatistics();
int generateOutputImages();

//Ready Container Functions
void insertIntoReady(process_t* p);
void removeFromReady();
process_t* getNextReady();
bool isReadyEmpty();
void destroyReady();

//Scheduling Algorithms Functions
void SRTNAlgo();
void HPFAlgo();
void RRAlgo(int);