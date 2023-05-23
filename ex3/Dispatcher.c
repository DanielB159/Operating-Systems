#include "Dispatcher.h"

char * findType(char *prodString) {
    char *token;
    
    // Get the third token using space (" ") as the delimiter
    token = strtok(prodString, " ");
    token = strtok(NULL, " ");
    token = strtok(NULL, " ");

    // return the token
    return token;
}

void * dispatchProducers(void *inp) {
    // parse inputs
    DispatcherInput *inputs = (DispatcherInput *)inp;
    BoundedQueue **queueArr = inputs->queueArr;
    UnboundedQueue **editorQueue = inputs->editorQueue;
    int numOfProds = inputs->numOfProds;

    // set up variables for round robin on the producer queues 
    int numOfDons = 0;;
    char doneProducer[numOfProds];
    int i;
    for (i = 0; i < numOfProds; i++) {
        doneProducer[i] = 0;
    }
    i = 0;
    // round robin on the producers
    while (1) {
        // if finished cycle on the producers
        if (i == numOfProds) {
            // if all producers are done, exit
            if (numOfDons == numOfProds) {
                break;
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
            } else {
                // create copy of article
                char *copy = malloc((sizeof(char) + 1)*strlen(article));
                if (copy == NULL) {
                    exit(-1);
                }
                strcpy(copy, article);
                char *type = findType(copy);
                if (!strcmp(type, "SPORTS")) {
                    enqueueUnbounded(editorQueue[0], article);
                } else if (!strcmp(type, "NEWS")) {
                    enqueueUnbounded(editorQueue[1], article);
                } else {
                    enqueueUnbounded(editorQueue[2], article);
                }
                free(copy);
            }
        } else {
            numOfDons++;
        }
        i++;
    }
    // if done with all producer queues, indicate to co-editor queues
    enqueueUnbounded(editorQueue[0], "DONE");
    enqueueUnbounded(editorQueue[1], "DONE");
    enqueueUnbounded(editorQueue[2], "DONE");

    return NULL;
}