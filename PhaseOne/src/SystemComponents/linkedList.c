// #include "linkedList.h"
// list_t* createLinkedList(void (*freeData)(void *),int (*compare)(void *, void *))
// {
//     list_t* list = (list_t*)malloc(sizeof(list_t));
//     if (list == NULL) {
//         return NULL;
//     }
//     list->freeData=freeData;
//     list->head = NULL;
//     list->size=0;
//     return list;
// }

// void insertAtBeginning(list_t* list, void *data) 
// {
//     node_t* newNode = createNode(data,NULL);
//     newNode->next = list->head;
//     list->head = newNode;
//     (list->size)++;
// }


// void insertAtEnd(list_t* list, void *data) 
// {
//     node_t* newNode = createNode(data,NULL);
//     if (list->head == NULL) {
//         list->head = newNode;
//     } else {
//         node_t* current = list->head;
//         while (current->next != NULL) {
//             current = current->next;
//         }
//         current->next = newNode;
//     }
//     (list->size)++;
// }

// node_t* getNodeByValue(list_t* list, void* value) 
// {
//     node_t* current = list->head;
//     while (current != NULL) {
//         if(list->compare(current->data,value))
//             return current;
//         current = current->next;
//     }
//     return NULL; // Value not found
// }

// void deleteUsingVal(list_t* list, void *value) 
// {
//     node_t* current = list->head;
//     node_t* prev = NULL;

//     while (current != NULL && list->compare(current->data, value) != 0) {
//         prev = current;
//         current = current->next;
//     }

//     if (current == NULL) {
//         return;
//     }

//     if (prev == NULL) {
//         list->head = current->next;
//     } else {
//         prev->next = current->next;
//     }
//     (list->size)--;
//     list->freeData(current->data);
//     free(current);
// }


// void printList(list_t* list, void (*print)(void *)) 
// {
//     node_t* current = list->head;
//     while (current != NULL) 
//     {
//         print(current->data);
//         current = current->next;
//     }
// }

// void destroyList(list_t* list) 
// {
//     node_t* current = list->head;
//     while (current != NULL) {
//         node_t* temp = current;
//         current = current->next;
//         list->freeData(temp->data);
//         free(temp);
//     }
//     list->size=0;
//     free(list);
// }