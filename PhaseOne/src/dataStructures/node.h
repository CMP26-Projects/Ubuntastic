#pragma once
#include <stdlib.h>
typedef struct Node
{
    void* data;
    struct Node* next;
}node_t;

node_t* createNode(void* data,node_t* next);