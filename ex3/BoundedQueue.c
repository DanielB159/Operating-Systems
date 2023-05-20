
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
    // // initilize the semaphores with the appropriate sizes
    // sem_init(mutex, 0, 1);
    // sem_init(full, 0, 0);
    // sem_init(empty, 0, size);
    // set up the parameters for q
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

// int main() {
//     BoundedQueue *q = createBoundedQueue(3, &mutex, &full, &empty);
//     char *first = "first", *second = "second", *third = "third", *fourth = "fourth", *fifth = "fifth";




//     enqueueBounded(q, first);
//     printf("\nout is %d and in is %d\n", q->offsetOut, q->offsetIn);
//     enqueueBounded(q, second);
//     printf("\nout is %d and in is %d\n", q->offsetOut, q->offsetIn);
//     enqueueBounded(q, third);
//     printf("\nout is %d and in is %d\n", q->offsetOut, q->offsetIn);
//     char *x = dequeueBounded(q), *y = dequeueBounded(q), *z = dequeueBounded(q);
//     printf("first: %s, second %s, third: %s", x, y, z);
//     printf("\nout is %d and in is %d\n", q->offsetOut, q->offsetIn);
//     printf("\nout is %d and in is %d\n", q->offsetOut, q->offsetIn);
//     enqueueBounded(q, fourth);
//     printf("\nout is %d and in is %d\n", q->offsetOut, q->offsetIn);
//     enqueueBounded(q, fifth);
//     char *w = dequeueBounded(q), *u = dequeueBounded(q);
//     printf("\nfourth: %s, fifth %s", w, u);

//     enqueueBounded(q, first);
//     printf("\nout is %d and in is %d\n", q->offsetOut, q->offsetIn);
//     enqueueBounded(q, second);
//     printf("\nout is %d and in is %d\n", q->offsetOut, q->offsetIn);
//     w = dequeueBounded(q), u = dequeueBounded(q), w = dequeueBounded(q);
//     printf("\first: %s, second %s", w, u);


//     return 0;
// }