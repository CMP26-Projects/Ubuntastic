#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define null 0

struct processData
{
    int arrivaltime;
    int priority;
    int runningtime;
    int id;
    int memsize;
};

int main(int argc, char *argv[])
{
    FILE *pFile;
    char filePath[25];
    sprintf(filePath, "test-%s.txt", argv[1]);
    pFile = fopen(filePath, "w");
    struct processData pData;
    fprintf(pFile, "#id\tarrival\truntime\tpriority\tmemsize\n");
    pData.arrivaltime = 1;
    srand(time(null));
    for (int i = 1; i <= atoi(argv[1]); i++)
    {
        pData.id = i;
        pData.arrivaltime += rand() % (4);
        pData.runningtime = (rand() % 4) + 1;
        pData.priority = rand() % (11);
        pData.memsize = rand() % 256;
        fprintf(pFile, "%d\t%d\t%d\t%d\t%d\n", pData.id, pData.arrivaltime, pData.runningtime, pData.priority, pData.memsize);
    }
    fclose(pFile);
}
