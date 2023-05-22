#ifndef SCREEN_MANAGER_H
#define SCREEN_MANAGER_H

#include <string.h>
#include "BoundedQueue.h"

typedef struct {
    BoundedQueue *q;
    int numTimes;
} ScreenManagerInput;

// this function tells the screenmamager to read from the queue and write to the screen
void * printToScreen(void *inp);

#endif