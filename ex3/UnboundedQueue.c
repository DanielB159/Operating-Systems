#include <malloc.h>
#include <stdio.h>
#include <semaphore.h>
#include <UnboundedQueue.h>

sem_t mutex;
sem_t full;



// This function inserts a new string to the queue q.
void enqueueUnbounded(UnboundedQueue *q, char *toInsert) {
    // first, make sure that no one is currently reading
    sem_wait(&mutex);

    // critical section - adding a new item to the Unbounded Queue
    q->arr[q->offsetIn] = toInsert;
    q->offsetIn++;
    q->arr = (char **)realloc(q->arr, (q->offsetIn + 1)*sizeof(char *));

    // end of critical section. release semaphores and exit
    sem_post(&mutex);
    sem_post(&full);
}

// This function reads a new string from the queue q.
char * dequeueUnbounded(UnboundedQueue *q) {
    // first make sure that something was added and that no one is currently reading
    sem_wait(&full);
    sem_wait(&mutex);

    // critical section - removing an item from the Unbounded Queue
    char *val = q->arr[q->offsetOut];
    q->offsetOut = q->offsetOut + 1;

    //end of critical section. release semaphores and return val
    sem_post(&mutex);

    return val;
}

/*
This function creates a new Unbounded queue with the given size.
Returns UnboundedQueue* on success, nullptr on faliure.
*/
UnboundedQueue* createUnboundedQueue() {
    UnboundedQueue *q = (UnboundedQueue *)malloc(sizeof(UnboundedQueue));
    if (!q) {
        return NULL;
    }
    sem_init(&mutex, 0, 1);
    sem_init(&full, 0, 0);
    q->arr = (char **)malloc(sizeof(char *)*2);
    if (!q->arr) {
        return NULL;
    }
    q->offsetIn = 0;
    q->offsetOut = 0;
    return q;
}

// int main() {
//     printf("lololololoolll");
//     UnboundedQueue *q = createUnboundedQueue();
//     char *first = "first", *second = "second", *third = "third", *fourth = "fourth", *fifth = "fifth";
//     enqueueUnbounded(q, first);
//     enqueueUnbounded(q, second);
//     enqueueUnbounded(q, third);
//     enqueueUnbounded(q, third);
//     char *x = dequeueUnbounded(q), *y = dequeueUnbounded(q), *z = dequeueUnbounded(q), *t = dequeueUnbounded(q);
//     printf("first: %s, second %s, third: %s, third: %s", x, y, z, t);
//     enqueueUnbounded(q, fourth);
//     enqueueUnbounded(q, fifth);
//     char *w = dequeueUnbounded(q), *u = dequeueUnbounded(q);

//     enqueueUnbounded(q, first);
//     enqueueUnbounded(q, second);
//     w = dequeueUnbounded(q), u = dequeueUnbounded(q);
//     printf("\nfirst: %s, second %s", w, u);

// }