#ifndef CO_EDITOR_H
#define CO_EDITOR_H

#include <unistd.h>
#include "UnboundedQueue.h"
#include "BoundedQueue.h"
#include <string.h>

typedef struct {
    UnboundedQueue *dispQueue;
    BoundedQueue *screenQueue;
} CoEditorInput;

// this function tells the co-editor to read from the dispatcher queue, edit, and write to the screen-manager queue
void * readEditSubmit(void *inp);

#endif