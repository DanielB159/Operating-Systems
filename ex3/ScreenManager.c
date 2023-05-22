#include "ScreenManager.h"

// this function tells the screenmamager to read from the queue and write to the screen
void * printToScreen(void *inp) {
    ScreenManagerInput *input = (ScreenManagerInput *)inp;
    BoundedQueue *q = input->q;
    int numTimes = input->numTimes;
    // read articles and print to screen
    char *article = "";
    int i;
    for(i = 0; i < numTimes; i++) {
        // if not the first iteration
        if (strcmp(article, "")) {
            // print article to the screen
            printf("%s\n", article);
        }
        article = dequeueBounded(q);
    }
    return NULL;
}