#ifndef _MINHEAP_H
#define _MINHEAP_H
#include "../SystemComponents/headers.h"
#define MAX_SIZE 10000
typedef struct
{
    process_t **arr;
    int size;
    int criteria;
} minHeap_t;

minHeap_t *createHeap(int crit);
void swap(process_t **a, process_t **b);
int parent(int index, int size);
int leftChild(int index, int size);
int rightChild(int index, int size);
void insert(minHeap_t *heap, process_t *p);
void heapify(minHeap_t *heap, int index, int size);
void deleteMin(minHeap_t *heap);
bool isEmptyHeap(minHeap_t *heap);
process_t *getMin(minHeap_t *heap);
void destroyHeap(minHeap_t *heap);

#endif
