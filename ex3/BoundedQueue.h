#ifndef BOUNDED_QUEUE_H
#define BOUNDED_QUEUE_H

#include <semaphore.h>
#include <malloc.h>


// This struct depicts a bounded queue.
typedef struct {
    sem_t *mutex;
    sem_t *full;
    sem_t *empty;
    int size;
    int offsetIn;
    int offsetOut;
    char** arr;
} BoundedQueue;

// This function inserts a new string to the queue q.
void enqueueBounded(BoundedQueue *q, char *toInsert);

// This function reads a new string from the queue q.
char * dequeueBounded(BoundedQueue *q);

/*
This function creates a new Bounded queue with the given size.
Returns UnboundedQueue* on success, nullptr on faliure.
*/
BoundedQueue* createBoundedQueue(int size, sem_t *mutex, sem_t *full, sem_t *empty);

#endif