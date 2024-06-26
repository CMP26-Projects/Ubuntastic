#include "minHeap.h"
// Function to create a new heap
minHeap_t *createHeap(int crit)
{
    minHeap_t *heap = (minHeap_t *)malloc(sizeof(minHeap_t));
    if (heap == NULL)
    {
        return NULL;
    }
    heap->arr = (process_t **)malloc(100000 * sizeof(process_t *));
    if (heap->arr == NULL)
    {
        return NULL;
    }
    heap->size = 0;
    heap->criteria = crit;
    return heap;
}

bool isEmptyHeap(minHeap_t *heap)
{
    return (heap->size == 0);
}

//=========================================

void swap(process_t **a, process_t **b)
{
    process_t *temp = *a;
    *a = *b;
    *b = temp;
}
int parent(int i, int size)
{
    return (i + 1) / 2 > 0 ? (i + 1) / 2 : -1;
}

int leftChild(int i, int size)
{
    return (i * 2 + 1) < size ? (i * 2 + 1) : -1;
}

int rightChild(int i, int size)
{
    return (i * 2 + 2) < size ? (i * 2 + 2) : -1;
}

void heapify(minHeap_t *heap, int i, int size)
{
    process_t **arr = heap->arr;
    int l, r, min;
    l = leftChild(i, size);
    r = rightChild(i, size);

    if (heap->criteria == HPF_t)
    {
        if (l != -1 && arr[i]->priority > arr[l]->priority)
            min = l;
        else
            min = i;
        if (r != -1 && arr[min]->priority > arr[r]->priority)
        {
            min = r;
        }

        if (min != i)
        {
            swap(&arr[i], &arr[min]);
            heapify(heap, min, size);
        }
    }
    else
    {
        if (l != -1 && arr[i]->RemT > arr[l]->RemT)
            min = l;
        else
            min = i;
        if (r != -1 && arr[min]->RemT > arr[r]->RemT)
        {
            min = r;
        }

        if (min != i)
        {
            swap(&arr[i], &arr[min]);
            heapify(heap, min, size);
        }
    }
}

void Build_heap(minHeap_t *heap, int size)
{
    process_t **arr = heap->arr;
    for (int i = size + 1 / 2; i >= 0; i--)
    {
        heapify(heap, i, size);
    }
}

void HeapSort(minHeap_t *heap, int size)
{
    process_t **arr = heap->arr;

    int n = size;

    Build_heap(heap, size);
    for (int i = n - 1; i >= 1; i--)
    {
        swap(&arr[i], &arr[0]);
        n--;
        heapify(heap, 0, n);
    }
}
process_t *getMin(minHeap_t *heap)
{
    if (heap->size == 0)
        return NULL;
    else
    {
        return heap->arr[0];
    }
}
void destroyHeap(minHeap_t *heap)
{
    while (!isEmptyHeap(heap))
        deleteMin(heap);
}
void deleteRoot(minHeap_t *heap)
{

    int n = heap->size;
    // Get the last element
    process_t *lastElement = heap->arr[n - 1];

    // Replace root with last element
    heap->arr[0] = lastElement;

    // Decrease size of heap by 1
    // // free(lastElement);
    /// lastElement = NULL;

    n = n - 1;
    heap->size = n;

    // heapify the root node
    heapify(heap, 0, n);
}
void deleteMin(minHeap_t *heap)
{
    deleteRoot(heap);
}
void insert(minHeap_t *heap, process_t *data)
{
    int index = heap->size;
    heap->arr[index] = data;
    heap->size++;
    int i = index;
    Build_heap(heap, heap->size);
}