#include "ScreenManager.h"

// this function tells the screenmamager to read from the queue and write to the screen
void * printToScreen(void *inp) {
    ScreenManagerInput *input = (ScreenManagerInput *)inp;
    BoundedQueue *q = input->q;
    int numTimes = input->numTimes;
    // read articles and print to screen
    char *article = dequeueBounded(q);;
    int i;
    for(i = 0; i < numTimes; i++) {
        // print article to the screen
        printf("%s\n", article);
        free(article);
        if (i != numTimes - 1) {
            article = dequeueBounded(q);
        }
    }
    printf("DONE\n");
    return NULL;
}