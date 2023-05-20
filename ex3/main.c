#include <pthread.h>
#include "BoundedQueue.h"
// #include "UnboundedQueue.h"
#include "Producer.h"

#define NUM_CO_EDITORS 3

pthread_t *producers;
pthread_attr_t *producerAttrs;
pthread_t dispatcher;
pthread_attr_t dispatcherAttr;
pthread_t coEditors[NUM_CO_EDITORS];
pthread_attr_t editorsAttrs[NUM_CO_EDITORS];
pthread_t screenManager;
pthread_attr_t managerAttr;

sem_t *mutex_arr;
sem_t *full_arr;
sem_t *empty_arr;





int main() {
    int numOfProds = 2;
    int sizeOfEachQueue = 10;
    int numOfArticlesInEachQueue = 5;
    int i;
    // this will be the array of the BoundedQueues
    BoundedQueue *producerQueueArr[numOfProds];

    // allocate room for threads
    producers = (pthread_t *)malloc(numOfProds*sizeof(pthread_t));
    producerAttrs = (pthread_attr_t *)malloc(numOfProds*sizeof(pthread_attr_t));

    //allocate room for semaphores
    mutex_arr = (sem_t *)malloc(numOfProds*sizeof(sem_t));
    full_arr = (sem_t *)malloc(numOfProds*sizeof(sem_t));
    empty_arr = (sem_t *)malloc(numOfProds*sizeof(sem_t));

    // initialize all of the thread attributes
    for (i = 0; i < numOfProds; i++) {
        pthread_attr_init(&producerAttrs[i]);
    }
    pthread_attr_init(&dispatcherAttr);
    for (i = 0; i < NUM_CO_EDITORS; i++) {
        pthread_attr_init(&editorsAttrs[i]);
    }
    pthread_attr_init(&managerAttr);

    for (i = 0; i < numOfProds; i++) {
        printf("is is now: %d\n", i);
        producerQueueArr[i] = createBoundedQueue(sizeOfEachQueue, &mutex_arr[i], &full_arr[i], &empty_arr[i]);
        ProducerInputs inp = {i, numOfArticlesInEachQueue, producerQueueArr[i]};
        pthread_create(&producers[i], &producerAttrs[i], produceArticles, (void *)&inp);
    }

    for (i = 0; i < numOfProds; i++) {
        for (int j = 0; j < numOfArticlesInEachQueue; j++) {
            char *x = dequeueBounded(producerQueueArr[i]);
            printf("%s\n", x);
        }
    }

    for (i = 0; i < numOfProds; i++) {
        pthread_join(producers[i], NULL);
    }


    free(producers);
    free(producerAttrs);
    free(mutex_arr);
    free(full_arr);
    free(empty_arr);
    return 0;
}