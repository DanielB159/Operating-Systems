#include "Dispatcher.h"

void * dispatchProducers(void *inp) {
    // parse inputs
    DispatcherInput *inputs = (DispatcherInput *)inp;
    BoundedQueue **queueArr = inputs->queueArr;
    int numOfProds = inputs->numOfProds;
    // set up variables for round robin on the producer queues 
    int doneEverything = 0, numOfDons = 0;;
    char doneProducer[numOfProds];
    int i;
    for (i = 0; i < numOfProds; i++) {
        doneProducer[i] = 0;
    }
    i = 0;
    printf("in disp: the number of producers is %d and the pointer to the queue is %p\n", numOfProds, queueArr);
    // round robin on the producers
    while (!doneEverything) {
        // if finished cycle on the producers
        if (i == numOfProds) {
            // if all producers are done, exit
            if (numOfDons == numOfProds) {
                doneEverything = 1;
            } else {
                // if some producers need to finish, go from beginning
                i = 0;
                numOfDons = 0;
            }
        }
        char *article;
        // if this producer isn't done
        if (!doneProducer[i]) {
            article =  dequeueBounded(queueArr[i]);
            if (!strcmp(article, "DONE")) {
                doneProducer[i] = 1;
            }
            printf("%s\n", article);
        } else {
            numOfDons++;
        }
        i++;
    }
    return NULL;
}