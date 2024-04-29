#include "./src/dataStructures/queue.h"

// Function to initialize the queue
queue_t *createQueue() {
    queue_t  *queue = (queue_t *)malloc(sizeof(queue_t));
    if (queue == NULL) {
        fprintf(stderr, "Memory allocation failed!\n");
        exit(EXIT_FAILURE);
    }
    queue->front = queue->rear = NULL;
    queue->size = 0;
    return queue;
}

// Function to check if the queue is empty
int isEmpty(queue_t  *queue) {
    return queue->front == NULL;
}

// Function to enqueue an element into the queue
void enqueue(queue_t  *queue, void *data) {
    node_t *newNode = createNode(data);
    if (isEmpty(queue)) {
        queue->front = queue->rear = newNode;
    } else {
        queue->rear->next = newNode;
        queue->rear = newNode;
    }
    queue->size++;
}

// Function to dequeue an element from the queue
void *dequeue(queue_t  *queue) {
    if (isEmpty(queue)) {
        fprintf(stderr, "Queue is empty!\n");
        exit(EXIT_FAILURE);
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
    if (isEmpty(queue)) {
        fprintf(stderr, "Queue is empty!\n");
        exit(EXIT_FAILURE);
    }
    return queue->front->data;
}

// Function to get the size of the queue
int getSize(queue_t *queue) {
    return queue->size;
}

// Function to free the memory allocated to the queue
void destroyQueue(queue_t *queue) {
    while (!isEmpty(queue)) {
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
    printf("\n");   
}