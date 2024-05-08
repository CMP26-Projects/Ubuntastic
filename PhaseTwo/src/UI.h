#ifndef _UI_H
#define _UI_H
#define NRM  "\x1B[0m"
#define RED  "\x1B[31m"
#define GRN  "\x1B[32m"
#define YEL  "\x1B[33m"
#define BLU  "\x1B[34m"
#define MAG  "\x1B[35m"
#define CYN  "\x1B[36m"
#define WHT  "\x1B[37m"
#define BLD  "\x1B[1m"
#include "ds/queue.h"
#include "ds/minHeap.h"


//Input functions
void getUserInput(int *schedAlgo, int *timeSlice);
queue_t* readFile(char*,queue_t*);


//Prints the line with the color clr
void printLine(char line[],char* clr);
void print(char line[],char* clr);
void printError(char errorMsg[]);

//Outputing Scheduling Statistics
void insertIntoLog(state_t state,float* pInfo);
void generatePrefFile(float* statistics);


//Printing Data Structures
void displayScheduler(int algo);
void printProcess(void* data, char* clr);
void printQueue(queue_t* queue, char* clr);
void printHeap(minHeap_t *heap, char* clr);
#endif