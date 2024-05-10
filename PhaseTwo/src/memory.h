#ifndef _MEMORY_H
#define _MEMORY_H
#include "./ds/pair.h"
#include "UI.h"

typedef struct memoryNode
{
    pair_t *interval;          // have the start and the end of the allocation
    int size;                  // size of the memory
    struct memoryNode *parent; // pointer to parent node
    process_t *process;        // pointer to the porcess that  have the memory
    struct memoryNode *right;  // right pointer
    struct memoryNode *left;   // left pointer

} memoryNode;

typedef struct
{
    memoryNode *root;   // root node 1024
    int size;           // the initial size is 1024
    int totalAllocated; // Tracking the total allocated memory
} memory_t;

memoryNode *initializeMemoryNode(int size, pair_t *pair, memoryNode *parent);
void deleteMemoryNode(memoryNode *node);
void divideMemory(memoryNode *root);
memory_t *initializeMemory();
bool allocateProcess(memory_t *memory, process_t *process);
void addProcess(memoryNode *root, process_t *process, bool *flag);
memoryNode *search(memoryNode *root, process_t *process);
void freeMemory(memory_t *memory, process_t *process);
#endif