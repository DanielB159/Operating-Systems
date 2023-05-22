
#include "BoundedQueue.h"

// This function inserts a new string to the queue q.
void enqueueBounded(BoundedQueue *q, char *toInsert) {
    // first, make sure that there is room to add a new item, and that no one is currently reading
    sem_wait(q->empty);
    sem_wait(q->mutex);

    // critical section - adding a new item to the Bounded Queue
    q->arr[q->offsetIn] = toInsert;
    q->offsetIn = (q->offsetIn + 1)%q->size;

    // end of critical section. release semaphores and exit
    sem_post(q->mutex);
    sem_post(q->full);
}

// This function reads a new string from the queue q.
char * dequeueBounded(BoundedQueue *q) {
    // first make sure that something was added and that no one is currently reading
    sem_wait(q->full);
    sem_wait(q->mutex);

    // critical section - removing an item from the Bounded Queue
    char *val = q->arr[q->offsetOut];
    q->offsetOut = (q->offsetOut + 1)%q->size;

    //end of critical section. release semaphores and return val
    sem_post(q->mutex);
    sem_post(q->empty);

    return val;
}

/*
This function creates a new Bounded queue with the given size.
Returns BoundedQueue* on success, nullptr on faliure.
*/
BoundedQueue* createBoundedQueue(int size, sem_t *mutex, sem_t *full, sem_t *empty) {
    // ititializing a queue struct
    BoundedQueue *q = (BoundedQueue *)malloc(sizeof(BoundedQueue));
    if (!q) {
        return NULL;
    }

    q->size = size;
    q->arr = (char **)malloc(size*sizeof(char *));
    if (!q->arr) {
        return NULL;
    }
    q->offsetIn = 0;
    q->offsetOut = 0;
    q->full = full;
    q->empty = empty;
    q->mutex = mutex;

    return q;
}
