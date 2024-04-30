#pragma once
#include "UI.h"
#include "headers.h"

//Functions definition
void createScheduler();
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
float* calculateStatistics();

//Ready container functions
void insertIntoReady(process_t* p);
void removeFromReady();
process_t* getNextReady();
bool isReadyEmpty();
void destroyReady();

//Scheduling Algorithms
void SRTNAlgo();
void HPFAlgo();
void RRAlgo(int);
int generateOutputImages();