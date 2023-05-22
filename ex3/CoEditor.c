#include "CoEditor.h"

// this function tells the co-editor to read from the dispatcher queue, edit, and write to the screen-manager queue
void * readEditSubmit(void *inp) {
    CoEditorInput *input = (CoEditorInput *)inp;
    UnboundedQueue *dispQueue = input->dispQueue;
    BoundedQueue *screenQueue = input->screenQueue;
    char *article = "";
    do {
        // if not the first iteration
        if (strcmp(article, "")){
            // read article from the dispatcher queue
            article = dequeueUnbounded(dispQueue);
            // "edit" the article
            sleep(0.1);
            // write the article to the screenManager Queue
            enqueueBounded(screenQueue, article);
        }
    } while (strcmp(article, "DONE"));
    // notify the screen manager that done reading from the dispatcher
    enqueueBounded(screenQueue, "DONE");
}