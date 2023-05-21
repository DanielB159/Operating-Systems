#ifndef DISPATCHER_H
#define DISPATCHER_H

#include <string.h>
#include "BoundedQueue.h"
#include "Producer.h"

typedef struct {
    BoundedQueue **queueArr;
    int numOfProds;
} DispatcherInput;

void * dispatchProducers(void *inp);

#endif