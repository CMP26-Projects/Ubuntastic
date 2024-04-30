#include <stdlib.h>
#include <stdbool.h>
#include "minHeap.h"

// Function to create a new heap
minHeap_t *createHeap(int (*compare)(void *,void *), void (*print)(void *)) {
    minHeap_t *heap = (minHeap_t *)malloc(sizeof(minHeap_t));
    if (heap == NULL) {
        return NULL;
    }
    heap->arr = (void **)malloc(MAX_SIZE * sizeof(void *));
    if (heap->arr == NULL) {
        return NULL;
    }
    heap->capacity = MAX_SIZE;
    heap->size = 0;
    heap->compare = compare;
    heap->print = print;
    return heap;
}

bool isEmptyHeap(minHeap_t* heap)
{
    return (heap->size==0);
}

void swap(void **a, void **b) {
    void *temp = *a;
    *a = *b;
    *b = temp;
}

size_t parent(size_t index) {
    return (index - 1) / 2;
}

size_t leftChild(size_t index) {
    return 2 * index + 1;
}

size_t rightChild(size_t index) {
    return 2 * index + 2;
}

void insert(minHeap_t *heap, void *data) {
    if (heap->size >= heap->capacity) {
        return;
    }
    size_t index = heap->size;
    heap->arr[index] = data;
    heap->size++;

    // Bubble up the newly inserted element
    while (index > 0 && heap->compare(heap->arr[index], heap->arr[parent(index)]) < 0) {
        swap(&heap->arr[index], &heap->arr[parent(index)]);
        index = parent(index);
    }
}

void heapify(minHeap_t *heap, size_t index) {    
    size_t smallest = index;
    
    size_t left = leftChild(index);
    size_t right = rightChild(index);

    if (left < heap->size && heap->compare(heap->arr[left], heap->arr[smallest]) < 0)
        smallest = left;
    if (right < heap->size && heap->compare(heap->arr[right], heap->arr[smallest]) < 0)
        smallest = right;

    
    if (smallest != index) {
        swap(&heap->arr[index], &heap->arr[smallest]);
        heapify(heap, smallest);
    }
}

void *getMin(minHeap_t *heap) {
    if (heap->size == 0)
        return NULL;
    else
    {
        #ifdef DEBUG
        printf("The min here\n");
        #endif  
        return heap->arr[0];
    } 
}

void deleteMin(minHeap_t* heap)
{
     if (heap->size == 0) {

        return;
    }

    heap->arr[0] = heap->arr[heap->size - 1];
    heap->size--;
    heapify(heap, 0);
    return;
}

void printHeap(minHeap_t *heap) {
    for (size_t i = 0; i < heap->size; i++) {
        heap->print(heap->arr[i]);
    }
}

void destroyHeap(minHeap_t* heap)
{
    while(!isEmptyHeap(heap))
        deleteMin(heap);
}
