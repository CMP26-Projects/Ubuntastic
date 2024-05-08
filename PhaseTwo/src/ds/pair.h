
#ifndef _PAIR_T
#define _PAIR_T
#include <stdlib.h>
#include <stdbool.h>
typedef struct
{
    int start;
    int end;
} pair_t;

pair_t *initializePair(int fisrt, int second);
void deletePair(pair_t *pair);
#endif
