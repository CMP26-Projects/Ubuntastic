#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "./src/dataStructures/minHeap.h"

// Function to create a new heap
minHeap_t *createHeap(size_t capacity, int (*compare)(const void *, const void *), void (*print)(const void *)) {
    minHeap_t *heap = (minHeap_t *)malloc(sizeof(minHeap_t));
    if (heap == NULL) {
        fprintf(stderr, "Memory allocation failed!\n");
        exit(EXIT_FAILURE);
    }
    heap->arr = (void **)malloc(capacity * sizeof(void *));
    if (heap->arr == NULL) {
        fprintf(stderr, "Memory allocation failed!\n");
        exit(EXIT_FAILURE);
    }
    heap->capacity = capacity;
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
        fprintf(stderr, "Heap overflow!\n");
        exit(EXIT_FAILURE);
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
        return heap->arr[0];
}

void deleteMin(minHeap_t* heap)
{
     if (heap->size == 0) {
        return;
    }
    if (heap->size == 1) {
        heap->size--;
    }

    heap->arr[0] = heap->arr[heap->size - 1];
    heap->size--;
    heapify(heap, 0);
}

void printHeap(minHeap_t *heap) {
    for (size_t i = 0; i < heap->size; i++) {
        heap->print(heap->arr[i]);
    }
}
