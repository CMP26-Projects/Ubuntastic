#ifndef _QUEUE_H
#define _QUEUE_H
#include <stdlib.h>
typedef struct Node
{
    void *data;
    struct Node *next;
} node_t;

typedef struct
{
    node_t *front;
    node_t *rear;
    int size;
} queue_t;

node_t *createNode(void *, node_t *);
queue_t *createQueue();

int isEmptyQueue(queue_t *queue);

void enqueue(queue_t *queue, void *data);

void dequeue(queue_t *queue);

void *front(queue_t *queue);
int getSize(queue_t *queue);

void destroyQueue(queue_t *queue);

#endif