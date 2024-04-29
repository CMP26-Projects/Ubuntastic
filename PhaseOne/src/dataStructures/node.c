#include "node.h"
node_t* createNode(void* data,node_t* next)
{
    node_t* node = (node_t*)malloc(sizeof(node_t));
    node->data=data;
    node->next=next;
    return node;
}