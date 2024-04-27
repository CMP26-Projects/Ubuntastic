#include "headers.h"
bool isEmpty(queue_t *q)
{
    return q->front == NULL;
}

void initializeQueue(queue_t* q)
{
    q->front = q->rear = NULL;
    q->count=0;
}

void enqueue(queue_t* q,process_t x)
{
    node_t* temp = (node_t*)malloc(sizeof(node_t));
    temp->data = x;
    temp->next = NULL;

    if (q->rear == NULL)
    {
        q->front = q->rear = temp;
    }
    else
    {
        q->rear->next = temp;
        q->rear = temp;
    }
    (q->count)++;
}

bool dequeue(queue_t* q, process_t** p)
{
    if (q->front == NULL)
    {
        *p = NULL;
        return false;
    }

    node_t* temp = q->front;
    q->front = q->front->next;
    //IT was the only node in the queue
    if (q->front == NULL)
    {
        q->rear = NULL;
    }
    --(q->count);

    // Allocate memory for p and copy the data from temp->data
    if(*p == NULL)
    *p = (process_t*)malloc(sizeof(process_t));
    **p = temp->data;
    free(temp);

    return true;
}

void printQueue(queue_t* q)
{
    node_t* temp = q->front;
    while (temp != NULL)
    {
        printf("ID: %d, AT: %d, RT: %d, Priority: %d, RemT: %d, state: %d\n", temp->data.ID, temp->data.AT, temp->data.RT, temp->data.priority, temp->data.RemT, temp->data.state);
        temp = temp->next;
    }
}

void destroyQueue(queue_t* q)
{
    process_t* p;
    while (dequeue(q, &p))
    {
        free(p);
    }
}