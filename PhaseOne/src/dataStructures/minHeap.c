#include "minHeap.h"
#include <stdlib.h>
// Function to create a new heap
minHeap_t *createHeap(int crit) {
    minHeap_t *heap = (minHeap_t *)malloc(sizeof(minHeap_t));
    if (heap == NULL) {
        return NULL;
    }
    heap->arr = (process_t **)malloc(100000 * sizeof(process_t *));
    if (heap->arr == NULL) {
        return NULL;
    heap->size = 0;
    heap->criteria = crit;
    }
    return heap;
}

bool isEmptyHeap(minHeap_t* heap)
{
    return (heap->size==0);
}

void swap(process_t **a, process_t **b)
{
    process_t *temp = *a;
    *a = *b;
    *b = temp;
}

int parent(int index) {
    return (index - 1) / 2;
}

int leftChild(int index) {
    return 2 * index + 1;
}

int rightChild(int index) {
    return 2 * index + 2;
}

int comparePriority(void* A ,void* B)
{
    process_t*a=(process_t*)A;
    process_t*b=(process_t*)B;
    if(a!=NULL&&b!=NULL)
        return a->priority - b->priority;
    else 
        return 0;
}

int compare(minHeap_t* heap,process_t* a, process_t* b)
{
    if(heap->criteria==HPF_t)
        if(a!=NULL & b!=NULL)
            return a->priority - b->priority;
        else
            return 0;
    else 
        if(a!=NULL & b!=NULL)
            return a->RemT - b->RemT;
        else
            return 0;
}

void insert(minHeap_t *heap, process_t *data) {
    int index = heap->size;
    printf("heap size is %d\n",heap->size);
    heap->arr[index] = data;
    heap->size++;

    // Bubble up the newly inserted element
    while (index > 0 && compare(heap,heap->arr[index], heap->arr[parent(index)]) < 0){
        printf("SWAPPING %d\n",heap->size); 
        swap(&heap->arr[index], &heap->arr[parent(index)]);
        index = parent(index);
    }
}

void heapify(minHeap_t *heap, int index) {    
    int smallest = index;
    
    int left = leftChild(index);
    int right = rightChild(index);

    if (left < heap->size && compare(heap,heap->arr[left], heap->arr[smallest]))
        smallest = left;
    if (right < heap->size && compare(heap,heap->arr[right], heap->arr[smallest]) < 0)
        smallest = right;

    
    if (smallest != index) {
        swap(&heap->arr[index], &heap->arr[smallest]);
        heapify(heap, smallest);
    }
}

process_t* getMin(minHeap_t *heap) {
    if (heap->size == 0)
        return NULL;
    else
    {
        #ifdef DEBUG
        printf("The min is here\n");
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

void destroyHeap(minHeap_t* heap)
{
    while(!isEmptyHeap(heap))
        deleteMin(heap);
}
