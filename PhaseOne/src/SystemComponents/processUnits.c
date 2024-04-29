#include "processUnits.h"

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

void printProcess(void* data)
{
    process_t* temp=(process_t*)data;
    if(temp!=NULL)
    printf("ID: %d, AT: %d, RT: %d, Priority: %d, RemT: %d, ", temp->ID, temp->AT, temp->RT, temp->priority, temp->RemT);
    switch (temp->state)
    {
    case FINISHED:
        printf("State: FINISHED\n");
        break;
    
    case WAITING:
        printf("State: WAITING\n");
        break;
    
    case RESUMED:
        printf("State: RESUMED\n");
        break;
    
    case STOPPED:
        printf("State: STOPPED\n");
        break;
    
    case ARRIVED:
        printf("State: ARRIVED\n");
        break;
            
    default:
        printf("State: STARTED\n");
        break;
    }
}



int comparePriority(void* A ,void* B)
{
    process_t*a=(process_t*)A;
    process_t*b=(process_t*)B;
    return a->priority - b->priority;
}

int compareRemTime(void* A ,void* B)
{
    process_t*a=(process_t*)A;
    process_t*b=(process_t*)B;
    return a->RemT - b->RemT;
}


pcb_slot* createSlot(int id,process_t* p)
{
    pcb_slot* slot=(pcb_slot*)malloc(sizeof(pcb_slot));
    slot->pid=id;
    slot->process=p;
}

int compareSlot(void* data,void* key)
{
    pcb_slot* slot=(pcb_slot*)data;
    if (TYPE_CHECK(key, pid_t *) && (pid_t *)key == &slot->pid) 
        return true; // Node with matching PID found
    else if (TYPE_CHECK(key, process_t *) && (process_t *)key == slot->process)
        return true; // Node with matching process pointer found
    else
        return false;
}

void freeSlot(void* data)
{
    pcb_slot* slot=(pcb_slot*)data;
    if(slot!=NULL&&slot->process!=NULL)
    {
        free(slot->process);
        free(slot);
    }
}