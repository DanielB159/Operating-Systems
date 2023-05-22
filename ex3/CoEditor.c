#include "CoEditor.h"

#include <threads.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/syscall.h>

// this function tells the co-editor to read from the dispatcher queue, edit, and write to the screen-manager queue
void * readEditSubmit(void *inp) {
    CoEditorInput *input = (CoEditorInput *)inp;
    UnboundedQueue *dispQueue = input->dispQueue;
    BoundedQueue *screenQueue = input->screenQueue;
    char *article = "";
    while (strcmp(article, "DONE")) {
        // if no the first iteration
        if (strcmp(article, "")) {
            // "edit" the article
            sleep(0.1);
            // write the article to the screenManager Queue
            enqueueBounded(screenQueue, article);
        }
        // read article from the dispatcher queue
        article = dequeueUnbounded(dispQueue);
    }
    // notify the screen manager that done reading from the dispatcher
    return NULL;
}