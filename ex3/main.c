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




//to delete//////////////
pthread_t producer1;
pthread_t producer2;
pthread_attr_t producer1Attrs;
pthread_attr_t producer2Attrs;

sem_t first_mutex;
sem_t second_mutex;
sem_t first_full;
sem_t second_full;
sem_t first_empty;
sem_t second_empty;

////////////////////////



int main() {
    int numOfProds = 2;
    int sizeOfEachQueue = 10;
    int numOfArticlesInEachQueue = 5;
    int i;
    // this will be the array of the BoundedQueues
    BoundedQueue *producerQueueArr[numOfProds];


    // pthread_attr_init(&producer1Attrs);
    // pthread_attr_init(&producer2Attrs);
    // producerQueueArr[0] = createBoundedQueue(sizeOfEachQueue, &first_mutex, &first_full, &first_empty);
    // ProducerInputs inp = {0, numOfArticlesInEachQueue, producerQueueArr[0]};
    // pthread_create(&producer1, &producer1Attrs, produceArticles, (void *)&inp);

    // producerQueueArr[1] = createBoundedQueue(sizeOfEachQueue, &second_mutex, &second_full, &second_empty);
    // ProducerInputs inp1 = {1, numOfArticlesInEachQueue, producerQueueArr[1]};
    // pthread_create(&producer2, &producer2Attrs, produceArticles, (void *)&inp1);

    // pthread_join(producer1, NULL);
    // pthread_join(producer2, NULL);


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

    ProducerInputs inp;
    // set the producers to produce their articles 
    for (i = 0; i < numOfProds; i++) {
        printf("i is now: %d\n", i);
        producerQueueArr[i] = createBoundedQueue(sizeOfEachQueue, &mutex_arr[i], &full_arr[i], &empty_arr[i]);
        inp.numArticles = numOfArticlesInEachQueue;
        inp.numProducer = i;
        inp.q = producerQueueArr[i];
        // ProducerInputs inp = {i, numOfArticlesInEachQueue, producerQueueArr[i]};
        printf("inp is: %d, %d, %p\n", inp.numProducer, inp.numArticles, inp.q);
        pthread_create(&producers[i], &producerAttrs[i], produceArticles, (void *)&inp);
    }

    // for (i = 0; i < numOfProds; i++) {
    //     for (int j = 0; j < numOfArticlesInEachQueue; j++) {
    //         char *x = dequeueBounded(producerQueueArr[i]);
    //         printf("%s\n", x);
    //     }
    // }

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