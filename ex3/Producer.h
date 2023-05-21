#ifndef PRODUCER_H
#define PRODUCER_H

#include "BoundedQueue.h"
#include <string.h>
#include <time.h>
#include <stdlib.h>

#define MAX_PRODUCER_STRINGLEN 100

// this struct is the input to the function producerArticles
typedef struct {
    int numProducer;
    int numArticles;
    BoundedQueue *q;
} ProducerInputs;

// this function creates numArticles articles and inputs them in the BoundedQueue q 
void * produceArticles(void *inp);

#endif

