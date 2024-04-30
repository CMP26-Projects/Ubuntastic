#pragma once
#include "headers.h"

#define TYPE_CHECK(ptr, type) _Generic((ptr), type: 1, default: 0) //For searching by val function 

process_t* createProcess(int info[]);

void printProcess(void* data);

int comparePriority(void* a ,void* b);

int compareRemTime(void* a ,void* b);

struct PCB* createSlot(int id, process_t* p);

int compareSlot(void* data ,void* key);

void freeSlot(void*);

