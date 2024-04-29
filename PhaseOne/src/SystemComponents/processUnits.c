#include "./src/SystemComponents/processUnits.h"

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

void printProcess(process_t* temp)
{
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



int comparePriority(process_t* a, process_t* b)
{
    return a->priority - b->priority;
}

int compareRemTime(process_t* a, process_t* b)
{
    return a->RemT - b->RemT;
}


pcb_slot* createSlot(int id,process_t* p)
{
    pcb_slot* slot=(pcb_slot*)malloc(sizeof(pcb_slot));
    slot->pid=id;
    slot->process=p;
}

bool compareSlot(pcb_slot* slot,void* key)
{
    if (TYPE_CHECK(key, pid_t *) && *(pid_t *)key == &slot->pid) 
        return true; // Node with matching PID found
    else if (TYPE_CHECK(key, process_t *) && (process_t *)key == slot->process)
        return true; // Node with matching process pointer found
    else
        return false;
}

void* freePCB(pcb_slot* slot)
{
    if(slot!=NULL&&slot->process!=NULL)
    {
        free(slot->process);
        free(slot);
    }
}