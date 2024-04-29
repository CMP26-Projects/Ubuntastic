#include "queue.h"

// Function to initialize the queue
queue_t *createQueue() {
    queue_t  *queue = (queue_t *)malloc(sizeof(queue_t));
    if (queue == NULL) {
        return NULL;
    }
    queue->front = queue->rear = NULL;
    queue->size = 0;
    return queue;
}

// Function to check if the queue is empty
int isEmptyQueue(queue_t  *queue) {
    #ifdef DEBUG
    if(queue==NULL)
    {
        printf("Queue is NULL\n");
        return 1;
    }
    #endif
    return queue->front == NULL;
}

// Function to enqueue an element into the queue
void enqueue(queue_t  *queue, void *data) {
    node_t *newNode = createNode(data,NULL);
    if (isEmptyQueue(queue)) {
        queue->front = queue->rear = newNode;
    } else {
        queue->rear->next = newNode;
        queue->rear = newNode;
    }
    queue->size++;
}

// Function to dequeue an element from the queue
void* dequeue(queue_t  *queue) {
    if (isEmptyQueue(queue)) {
        return (void*)NULL;
    }
    node_t *temp = queue->front;
    void *data = temp->data;
    queue->front = queue->front->next;
    free(temp);
    queue->size--;
    return data;
}

// Function to get the front element of the queue without removing it
void *front(queue_t  *queue) {
    if (isEmptyQueue(queue)) {
        return NULL;
    }
    return queue->front->data;
}

// Function to get the size of the queue
int getSize(queue_t *queue) {
    return queue->size;
}

// Function to free the memory allocated to the queue
void destroyQueue(queue_t *queue) {
    while (!isEmptyQueue(queue)) {
        dequeue(queue);
    }
    free(queue);
}

void printQueue(queue_t* queue, void (*print)(void *))
{
  node_t *current = queue->front;
    while (current != NULL) {
        print(current->data);
        current = current->next;
    }
}