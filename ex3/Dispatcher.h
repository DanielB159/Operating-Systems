#ifndef DISPATCHER_H
#define DISPATCHER_H

#include <string.h>
#include "BoundedQueue.h"
#include "Producer.h"
#include "UnboundedQueue.h"

typedef struct {
    BoundedQueue **queueArr;
    int numOfProds;
    UnboundedQueue **editorQueue;
} DispatcherInput;

void * dispatchProducers(void *inp);

#endif