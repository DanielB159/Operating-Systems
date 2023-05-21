#include <pthread.h>
#include "BoundedQueue.h"
// #include "UnboundedQueue.h"
#include "Producer.h"
#include "Dispatcher.h"

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
    int numOfProds = 5;
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
        // initilize the semaphores with the appropriate sizes
        sem_init(&mutex_arr[i], 0, 1);
        sem_init(&full_arr[i], 0, 0);
        sem_init(&empty_arr[i], 0, sizeOfEachQueue);
    }
    pthread_attr_init(&dispatcherAttr);
    for (i = 0; i < NUM_CO_EDITORS; i++) {
        pthread_attr_init(&editorsAttrs[i]);
    }
    pthread_attr_init(&managerAttr);

    // setting up the inputs for each producer
    ProducerInputs *inputs[numOfProds];
    
    for (i = 0; i < numOfProds; i++) {
        producerQueueArr[i] = createBoundedQueue(sizeOfEachQueue, &mutex_arr[i], &full_arr[i], &empty_arr[i]);
        inputs[i] = (ProducerInputs *)malloc(sizeof(ProducerInputs));
        if (inputs[i] == NULL) {
            exit(1);
        }
        // setting up the input
        inputs[i]->numArticles = numOfArticlesInEachQueue;
        inputs[i]->numProducer = i;
        inputs[i]->q = producerQueueArr[i];
    }

    // setting up the inputs for the dispatcher
    DispatcherInput *dispInput = (DispatcherInput *)malloc(sizeof(DispatcherInput));
    if (dispInput == NULL) {
        exit(-1);
    }
    dispInput->numOfProds = numOfProds;
    dispInput->queueArr = producerQueueArr;

    // set the producers to produce their articles 
    for (i = 0; i < numOfProds; i++) {
        // ProducerInputs inp = {i, numOfArticlesInEachQueue, producerQueueArr[i]};
        pthread_create(&producers[i], &producerAttrs[i], produceArticles, (void *)inputs[i]);
    }
    // set the dispatcher to round robin over the producers
    pthread_create(&dispatcher, &dispatcherAttr, dispatchProducers, (void *)dispInput);

    // waiting to join the threads
    for (i = 0; i < numOfProds; i++) {
        pthread_join(producers[i], NULL);
    }
    pthread_join(dispatcher, NULL);

    // freeing all allocated memory
    for (i = 0; i < numOfProds; i++) {
        free(inputs[i]);
        producerQueueArr[i]->arr;
    }

    free(producers);
    free(producerAttrs);
    free(mutex_arr);
    free(full_arr);
    free(empty_arr);
    return 0;
}