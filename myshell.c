#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#define COMMAND_BUFFER_SIZE 101

// This function executes the shell loop of getting input from the user and executing it indefinitely
void shellLoop(char* dir) {
    char userSring[COMMAND_BUFFER_SIZE];
    char command[COMMAND_BUFFER_SIZE];
    while (1) {
        char *token1, *token2;
        const char delim[2] = " ";
        printf("$ ");
        fflush(stdout);
        // getting input from the user with no regards to spaces
        // DANIEL BEFORE SUBMITION DELETE FGETS!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        // fgets(userSring, COMMAND_BUFFER_SIZE, stdin);
        //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        scanf(" %[^\n]%*c", userSring);
        token1 = strtok(userSring, delim);
        int numTokens = 0;
        //this while loop checks first how many arguments there are with the command
        while (token1 != NULL) {
            if (numTokens != 0) {
                numTokens++;
            } else {
                // this is the command, it to the command string
                strcpy(command, token1);
                numTokens++;
            }
            token1 = strtok(NULL, userSring);
        }
        // initializing the array of arguments to pass to execv
        char** arguments = (char**)malloc(numTokens*sizeof(char*));
        // tokenizing the userString again but with saving all of the arguments
        int i;
        token2 = strtok(userSring, delim);
        for (i = 0; token2 != NULL; i++) {
            if (i != 0) {
                // moving the argument to the array of arguments 
                arguments[i-1] = (char*)malloc(sizeof(token2) + 1);
                strcpy(arguments[i-1], token2);
            }
            token2 = strtok(NULL, userSring);
        }
        for (int j = 0; j < numTokens; j++) {
            printf("%d: %s\n", j, arguments[j]);
        }
    }
}




int main(int argc, char *argv[]) {
    // Check if number of arguments is valid
    if (argc != 2) {
        exit(1);
    }

    shellLoop(argv[1]);
    
    return 0;
}
