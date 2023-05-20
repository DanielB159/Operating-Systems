#ifndef UNBOUNDED_QUEUE_H
#define UNBOUNDED_QUEUE_H

// This struct depicts a bounded queue.
typedef struct {
    int offsetIn;
    int offsetOut;
    char** arr;
} UnboundedQueue;


// This function inserts a new string to the queue q.
void enqueueUnbounded(UnboundedQueue *q, char *toInsert);

// This function reads a new string from the queue q.
char * dequeueUnbounded(UnboundedQueue *q);

/*
This function creates a new Unbounded queue with the given size.
Returns UnboundedQueue* on success, nullptr on faliure.
*/
UnboundedQueue* createUnboundedQueue();

#endif