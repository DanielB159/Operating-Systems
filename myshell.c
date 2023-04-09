#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "unistd.h"
#include "sys/types.h"
#include "sys/wait.h"

#define COMMAND_BUFFER_SIZE 101
#define COMMAND_PATH_SIZE 1024

char* findLibrary(char *command) {
    // this buffer will hold the path to the command 
    char *path = (char *)malloc(sizeof(char) * COMMAND_PATH_SIZE);
    char *whichString = "which";
    char toRun[6 + COMMAND_BUFFER_SIZE];
    sprintf(toRun, "%s %s", whichString, command);
    // executing the command "which command" to get the directory of it, and opening a pipe to read the output
    FILE* pipe = popen(toRun, "r");
    if (pipe == NULL) {
        exit(1);
    }
    // read the output to the path buffer
    if (fgets(path, sizeof(char) * COMMAND_PATH_SIZE, pipe) == NULL) {
        exit(1);
    }
    /* 
    null terminate the newline character at the end of the "path". 
    strcspn returns the index before the first appearance of "\n"
    */
    path[strcspn(path, "\n")] = '\0'; 
    pclose(pipe);
    return path;
}


// This function executes the shell loop of getting input from the user and executing it indefinitely
void shellLoop(char* dir) {
    char * test = NULL;
    char userString[COMMAND_BUFFER_SIZE], userStringCpy[COMMAND_BUFFER_SIZE];
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
        scanf(" %[^\n]%*c", userString);
        // copying the command for second strtok 
        strcpy(userStringCpy, userString);
        token1 = strtok(userString, delim);
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
            token1 = strtok(NULL, delim);
        }
        // initializing the array of arguments to pass to execv
        char** arguments = (char**)malloc((numTokens + 1) * sizeof(char *));
        // tokenizing the userString again but saving all of the arguments
        token2 = strtok(userStringCpy, delim);
        int i;
        for (i = 0; token2 != NULL; i++) {
            arguments[i] = (char*)malloc(sizeof(token2) + 1);
            strcpy(arguments[i], token2);
            token2 = strtok(NULL, delim);
        }
        // setting the final argument to be NULL
        arguments[i] = NULL;
        // finding the path to the 
        char *pathToDir = findLibrary(command);
        for (int j = 0; j <= numTokens; j++) {
            printf("this is the %d: %s\n", j, arguments[j]);
        }


        // forking and executing the command in the child
        int stat;
        int pid = fork();
        if (!pid) {
            execv(pathToDir, arguments);
        } else {
            wait(&stat);
        }

        // freeing the allocated memory
        for (int j = 0; j < numTokens - 1; j++) {
            free(arguments[j]);
        }
        free(arguments);
        free(pathToDir);
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
