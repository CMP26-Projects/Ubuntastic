#pragma once
#include <stdbool.h>
#define MAX_SIZE 10000
typedef struct {
    void **arr;
    size_t capacity;
    size_t size;
    int (*compare)(void *, void *);
    void (*print)(void *);
} minHeap_t;

// Function to create a new heap
minHeap_t *createHeap(int (*compare)(void *, void *), void (*print)(void *)) ;
// Function to swap two elements in the heap
void swap(void **a, void **b);
// Function to get index of the parent of a node
size_t parent(size_t index);
// Function to get index of left child of a node
size_t leftChild(size_t index) ;
// Function to get index of right child of a node
size_t rightChild(size_t index) ;
// Function to insert an element into the heap
void insert(minHeap_t *heap, void *data) ;
// Function to heapify a subtree with the given root
void heapify(minHeap_t *heap, size_t index);
// Function to remove and return the minimum element from the heap
void deleteMin(minHeap_t *heap);

bool isEmptyHeap(minHeap_t* heap);

void* getMin(minHeap_t* heap);
// Function to print the elements of the heap
void printHeap(minHeap_t *heap) ;

void destroyHeap(minHeap_t* heap);