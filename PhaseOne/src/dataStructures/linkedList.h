
#include "./src/SystemComponents/headers.h"
#include "./src/dataStructures/node.h"
typedef struct
{
    node_t* head;
    int size;
    void (*freeData)(void *);
    int (*compare)(void *, void *);
}list_t;

list_t* createLinkedList(void (*freeData)(void *),int (*compare)(void *, void *));

void insertAtBeginning(list_t* list, void *data);

void insertAtEnd(list_t* list, void *data) ;

node_t* getNodeByValue(list_t* list, void* value);

void deleteUsingVal(list_t* list, void *value);

void printList(list_t* list, void (*print)(void *));

void destroyList(list_t* list);