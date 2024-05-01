#include "processUnits.h"

///////////////////////////////////////////
///////////////Process Functions///////////
///////////////////////////////////////////
process_t* createProcess(int processInfo[])
{
    process_t* P=(process_t*)malloc(sizeof(process_t));
    P->ID = processInfo[0];
    P->AT = processInfo[1];
    P->RT = processInfo[2];
    P->RemT = processInfo[2];
    P->priority = processInfo[3];
    P->state = WAITING;
    P->WT=0;
    return P;
}
///////////////////////////////////////////
/////////////////PCB List//////////////////
///////////////////////////////////////////

List_t* createList()
{
    List_t* list=(List_t*)malloc(sizeof(List_t));
    list->head=NULL;
    list->tail=NULL;
    list->size=0;
    return list;
}

// PCB_t* insertSlot(List_t* list,pid_t id,process_t* p)
// {
//     PCB_t* pcbNode=(PCB_t*)malloc(sizeof(PCB_t));
//     pcbNode->pid=id;
//     pcbNode->process=p;
//     if(list->head==NULL)
//     {
//         list->head=pcbNode;
//         list->tail=pcbNode;
//     }
//     else
//     {
//         list->tail->next=pcbNode;
//         list->tail=pcbNode;
//     }
//     list->size++;
//     return pcbNode;
// }

// PCB_t* getByPid(List_t* list, pid_t key)
// {
//     PCB_t* it = list->head;

//     while(it)
//     {
//         if(it->process->ID==key)
//             return it;
//         it=it->next;
//     }
//     return NULL;
// }

// PCB_t* getByProcess(List_t* list, process_t* key)
// {
//     PCB_t* it = list->head;
    
//     while(it)
//     {
//         if(it->process==key)
//             return it;
//         it=it->next;
//     }
//     return NULL;
// }

void freeSlotData(PCB_t* slot)
{ 
    if(slot!=NULL)
    {
        if(slot->process!=NULL)
            free(slot->process);

        free(slot);
    }
}

// bool deleteSlot(List_t* list ,process_t* p)
// {
//     PCB_t* it = list->head;
//     PCB_t* prev = NULL;
//     while(it)
//     {
//         if(it->process==p)
//         {
//             if(prev==NULL)
//             {
//                 list->head=it->next;
//             }
//             else
//             {
//                 prev->next=it->next;
//             }
//             freeSlotData(it);
//             (list->size)--;
//             return true;
//         }
//         prev=it;
//         it=it->next;
//     }
//     return false;
// }

// void destroyList(List_t* list)
// {
//     PCB_t* it = list->head;
//     PCB_t* temp;
//     while(it)
//     {
//         temp=it;
//         it=it->next;
//         freeSlotData(temp);
//     }
//     free(list);
// }
