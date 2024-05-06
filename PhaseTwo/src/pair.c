#include "pair.h"

pair_t *initializePair(int fisrt, int second)
{
    pair_t *pair = (pair_t *)malloc(sizeof(pair_t));
    pair->start = fisrt;
    pair->end = second;

    return pair;
}

void deletePair(pair_t *pair)
{
    if (pair != NULL)
        free(pair);
    return;
}
