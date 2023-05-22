#include "Producer.h"

// #include <pthread.h>
// #include <unistd.h>

// define the options for articles 
const char* options[] = {"SPORTS", "NEWS", "WEATHER"};

// this function creates numArticles articles and inputs them in the BoundedQueue q
void * produceArticles(void *inp) {
    // parse the inputs fiven to the function
    ProducerInputs *inputs = (ProducerInputs *)inp;
    int numProducer = inputs->numProducer;
    int numArticles = inputs->numArticles;
    BoundedQueue *q = inputs->q;

    // initalize counters for articles and random seed
    int numNews = 0, numSports = 0, numWeather = 0;
    srand(time(NULL));
    // make numArticles articles and add enququq each one
    for (int i = 0; i < numArticles; i++) {
        int randIndex = rand() % 3;
        int *numToIncrement;
        // select the correct number to increment
        switch(randIndex) {
            case 0:
                numToIncrement = &numSports;
                break;
            case 1:
                numToIncrement = &numNews;
                break;
            case 2:
                numToIncrement = &numWeather;
                break;
            default:
        }

        char *article = (char *)malloc(sizeof(char)*MAX_PRODUCER_STRINGLEN);
        // if malloc failed, exit
        if (article == NULL) {
            exit(1);
        }
        // define the new article and enqueue it
        sprintf(article, "Producer %d %s %d", numProducer, options[randIndex], *numToIncrement);
        // printf("producer %d now inserting: %s into queue...\n", numProducer, article);
        enqueueBounded(q, article);

        // incrementing the correct counter
        *numToIncrement = *numToIncrement + 1;
    }
    // indicate that done making aricles
    enqueueBounded(q, "DONE");
    return NULL;

}