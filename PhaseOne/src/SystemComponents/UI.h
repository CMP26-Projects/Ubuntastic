#pragma once
#include "processUnits.h"

// Uncomment the following line to enable debugging
//#define DEBUG

void displayScheduler(int algo)
{
    system("clear");
    switch (algo)
    {
    case RR_t:
        printf("%s(--------------------RR ALGORITHM--------------------)\n",BLU);
        break;
    case HPF_t:
        printf("%s(--------------------HPF ALGORITHM--------------------)\n",BLU);
        break;
    case SRTN_t:
        printf("%s(--------------------SRTN ALGORITHM--------------------)\n",BLU);
        break;
    default:
        printf("INVALID SCHEDULING ALGORITHM\n");
        exit(-1);
        break;
    }
}

void printLine(char line[],char* clr)
{
    printf("%s%s\n", clr,line);
    printf("%s",NRM); //Reset the color of the terminal 
}

void printError(char errorMsg[])
{
    perror(errorMsg);
}



void getUserInput(int *schedAlgo, int *timeSlice) //Will get updated and support GUI 
{
    Get_Scheduling_Algorithm: 
    system("clear"); //Clear the terminal
    printf("|| Choose a Scheduling Algorithm ||\n");
    printf("For SRTN, Enter 0\nFor HPF, Enter 1\nFor RR, Enter 2\nAlgorithm: ");
    scanf("%d", schedAlgo);
    if (*schedAlgo> 2)
    {
        printf("Invalid Option ->\n");
        sleepMilliseconds(500); 
        goto Get_Scheduling_Algorithm;
    }


    if(*schedAlgo == 2)
    {
        //For RR get the time slice
        Get_RR_timeSlice:
            printf("||Enter the Time Slice||\nTime: ");
            scanf("%d", timeSlice);
            if (*timeSlice < 0)
            {
                printf("\nInvalid Option -> ");
                goto Get_RR_timeSlice;
            }
    }
    system("clear");
}

void insertIntoLog(state_t state,float* pInfo)
{
    FILE* file = fopen("Scheduler.log", "a");
    if (!file)
    {
        printf("Error in opening the file.. ");
        exit(-1);
    }

    char st[30];
    switch (state)
    {
    case STARTED:
        strcpy(st, "⏳ started ⏳");
        break;
    case ARRIVED:
        strcpy(st, "🚀 arrived 🚀");
        break;
    case STOPPED:
        strcpy(st, "⛔️ stopped ⛔️");
        break;
    case FINISHED:
        strcpy(st, "✅ finished ✅");
        break;
    case RESUMED:
        strcpy(st, "▶️ resumed ▶️");
        break;
    }
    fprintf(file, "AT time %d process %.2f %s total %.2f remain %.2f wait %.2f ", getClk(), pInfo[0], st, pInfo[1], pInfo[2], pInfo[3]);
    
    if (state == FINISHED)
    {
        fprintf(file, "TA %.2f WTA %.2f",pInfo[4] ,pInfo[5]);
    }
    fprintf(file, "\n");
    fclose(file);
}


void generatePrefFile(float* statistics)
{
    FILE *file=fopen("Scheduler.pref","w");
    if(!file)
    {
        printf("can't open the file\n");
        exit(-1);
    }


    fprintf(file, "CPU utilization = %.2f %%\n", statistics[0]);
    fprintf(file, "Avg WTA = %.2f\n", statistics[1]);
    fprintf(file, "Avg Waiting = %.2f\n", statistics[2]);
    fprintf(file, "Std WTA = %.2f\n", statistics[3]);
    fclose(file);
}
