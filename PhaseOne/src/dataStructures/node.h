#include "./src/SystemComponents/headers.h"
typedef struct Node
{
    void* data;
    struct Node* next;
}node_t;

node_t* createNode(void* data,node_t* next)
{
    node_t* node = (node_t*)malloc(sizeof(node_t));
    node->data=data;
    node->next=next;
    return node;
}