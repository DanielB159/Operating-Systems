#include <pthread.h>
#include "BoundedQueue.h"
#include "UnboundedQueue.h"
#include "Producer.h"
#include "Dispatcher.h"
#include "CoEditor.h"
#include "ScreenManager.h"

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

sem_t *mutex_coed_arr;
sem_t *full_coed_arr;

sem_t mutex_screen_manager;
sem_t full_screen_manager;
sem_t empty_screen_manager;

int main() {
    int numOfProds = 5;
    int sizeOfEachQueue = 30;
    int numOfArticlesInEachQueue = 5;
    int i;
    // initizlize Bounded and Unbounded Queues
    BoundedQueue *producerQueueArr[numOfProds];
    UnboundedQueue *coEditorQueueArr[NUM_CO_EDITORS];
    BoundedQueue *screenManagerQueue;

    // allocate room for threads
    producers = (pthread_t *)malloc(numOfProds*sizeof(pthread_t));
    producerAttrs = (pthread_attr_t *)malloc(numOfProds*sizeof(pthread_attr_t));

    //allocate room for semaphores
    mutex_arr = (sem_t *)malloc(numOfProds*sizeof(sem_t));
    full_arr = (sem_t *)malloc(numOfProds*sizeof(sem_t));
    empty_arr = (sem_t *)malloc(numOfProds*sizeof(sem_t));

    mutex_coed_arr = (sem_t *)malloc(NUM_CO_EDITORS*sizeof(sem_t));
    full_coed_arr = (sem_t *)malloc(NUM_CO_EDITORS*sizeof(sem_t));
    
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
        // initialize the semaphores with the appropriate sizes
        sem_init(&mutex_coed_arr[i], 0, 1);
        sem_init(&full_coed_arr[i], 0, 0);
    }
    // initialize screen manager semaphores
    pthread_attr_init(&managerAttr);
    sem_init(&mutex_screen_manager, 0, 1);
    sem_init(&full_screen_manager, 0, 0);
    sem_init(&empty_screen_manager, 0, sizeOfEachQueue);

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
    for (i = 0; i < NUM_CO_EDITORS; i++) {
        coEditorQueueArr[i] = createUnboundedQueue(&mutex_coed_arr[i], &full_coed_arr[i]);
    }
    dispInput->numOfProds = numOfProds;
    dispInput->queueArr = producerQueueArr;
    dispInput->editorQueue = coEditorQueueArr;
    
    // setting up input for the co-editors
    CoEditorInput *coEditorInputs[3];
    screenManagerQueue = createBoundedQueue(sizeOfEachQueue, &mutex_screen_manager, &full_screen_manager, &empty_screen_manager);
    for (i = 0; i < NUM_CO_EDITORS; i++) {
        coEditorInputs[i] = (CoEditorInput *)malloc(sizeof(CoEditorInput));
        if (coEditorInputs[i] == NULL) {
            exit(-1);
        }
        coEditorInputs[i]->dispQueue = coEditorQueueArr[i];
        coEditorInputs[i]->screenQueue = screenManagerQueue;
    }

    // setting up input for the screenManager
    ScreenManagerInput *screenMngInput = (ScreenManagerInput *)malloc(sizeof(ScreenManagerInput));
    screenMngInput->q = screenManagerQueue;
    screenMngInput->numTimes = numOfProds*numOfArticlesInEachQueue;

    // set the producers to produce their articles 
    for (i = 0; i < numOfProds; i++) {
        pthread_create(&producers[i], &producerAttrs[i], produceArticles, (void *)inputs[i]);
    }
    // set the dispatcher to round robin over the producers
    pthread_create(&dispatcher, &dispatcherAttr, dispatchProducers, (void *)dispInput);

    // set the co-editors to recieve input from the dispatcher and edit it
    for (i = 0; i < NUM_CO_EDITORS; i++) {
        pthread_create(&coEditors[i], &editorsAttrs[i], readEditSubmit, (void *)coEditorInputs[i]);
    }

    // // set up the screen manager to read input and print to screen
    pthread_create(&screenManager, &managerAttr, printToScreen, (void *)screenMngInput);

    // waiting to join the threads
    for (i = 0; i < numOfProds; i++) {
        pthread_join(producers[i], NULL);
    }
    pthread_join(dispatcher, NULL);
    for (i = 0; i < NUM_CO_EDITORS; i++) {
        pthread_join(coEditors[i], NULL);
    }
    pthread_join(screenManager, NULL);

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