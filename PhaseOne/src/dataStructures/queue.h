#pragma once
#include "node.h"

typedef struct {
    node_t *front;
    node_t *rear;
    int size;
} queue_t;


queue_t *createQueue();

int isEmptyQueue(queue_t  *queue);

void enqueue(queue_t  *queue, void *data);

void *dequeue(queue_t  *queue);

void *front(queue_t  *queue);

int getSize(queue_t *queue);

void destroyQueue(queue_t *queue);

void printQueue(queue_t* queue, void (*print)(void *));
