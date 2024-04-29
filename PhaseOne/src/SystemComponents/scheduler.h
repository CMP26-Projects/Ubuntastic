#pragma once
#include "./src/SystemComponents/headers.h"
#include "./src/SystemComponents/UI.h"
#include "./src/SystemComponents/processUnits.h"
#include "./src/dataStructures/linkedList.h"

typedef struct 
{
int algo;
list_t* pcb;
process_t* runningP;
process_t* lastRecieved;
int timeSlice;
int busyTime;
int finishedProcessesNum;
int recievedProcessesNum;
int totalProcessesNum;
int totalWT;
float totalWTAT;
void* readyContainer;
}scheduler_t;

scheduler_t* sch;

//Functions definition
scheduler_t* createScheduler();
void receiveProcesses(int signum);
void startProcess(process_t* p);
void insertIntoPCB(process_t* p,pid_t pid);
process_t* getProcessByID(pid_t pid);
void stopProcess(process_t* p);
void continueProcess(process_t* p);
pid_t getPID(process_t* p);
void updatePCB(process_t* p,state_t state);    
void finishProcess(int signum);
void updateOutfile(process_t* p);
void clearResources(int signum);

//Ready container functions
void insertIntoReady(process_t* p);
void removeFromReady();
process_t* getNextReady();
bool isReadyEmpty();
void destroyReady();

///TO DO             
void SRTNAlgo();
void HPFAlgo();
void RRAlgo(int);
int generateOutputImages();