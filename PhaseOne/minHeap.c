#include "headers.h"


minHeap_t initializeHeap(int crit)
{
    minHeap_t heap;
    heap.size = 0;
    heap.criteria = crit;
    return heap;
}

bool lessThan(process_t a, process_t b, bool criteria)
{
    if (criteria == 0)
        return (a.RemT < b.RemT); //For SRTN
    else
        return (a.priority < b.priority); //For Non Pre-emitive HPF
}

bool isEmptyHeap(minHeap_t* heap)
{
    return heap->size == 0;
}

void push(minHeap_t* heap,process_t P)
{
    heap->arr = (heap->size == 0) ? malloc(sizeof(process_t)) : realloc(heap->arr, (heap->size + 1) * sizeof(process_t));
    process_t newProcess = P;
    int i = (heap->size)++;
    while (i > 0 && lessThan(newProcess, heap->arr[(i - 1) / 2], heap->criteria))
    {
        heap->arr[i] = heap->arr[(i - 1) / 2];
        i = (i - 1) / 2;
    }
    heap->arr[i] = newProcess;
}

void heapify(minHeap_t* heap, int i)
{
    int smallest = i;
    int left = 2 * i + 1;
    int right = 2 * i + 2;
    if (left < heap->size && lessThan(heap->arr[left], heap->arr[smallest], heap->criteria))
    {
        smallest = left;
    }
    if (right < heap->size && lessThan(heap->arr[right], heap->arr[smallest], heap->criteria))
    {
        smallest = right;
    }
    if (smallest != i)
    {
        process_t temp = heap->arr[i];
        heap->arr[i] = heap->arr[smallest];
        heap->arr[smallest] = temp;
        heapify(heap, smallest);
    }
}

process_t* getMin(minHeap_t* heap)
{
    return &heap->arr[0];
}

void pop(minHeap_t* heap)
{
    if (heap->size == 0)
        return;
    process_t temp = heap->arr[0];
    heap->arr[0] = heap->arr[heap->size - 1];
    heap->size--;
    heapify(heap, 0);
}

void printHeap(minHeap_t* heap)
{
    for (int i = 0; i < heap->size; i++)
    {
        printf("ID: %d, AT: %d, RT: %d, Priority: %d, RemT: %d, state: %d\n", heap->arr[i].ID, heap->arr[i].AT, heap->arr[i].RT, heap->arr[i].Priority, heap->arr[i].RemT, heap->arr[i].state);
    }
}

void destroyHeap(minHeap_t* heap)
{
    free(heap->arr);
    heap->size = 0;
}