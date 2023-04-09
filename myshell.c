#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "unistd.h"
#include "sys/types.h"
#include "sys/wait.h"

#define COMMAND_BUFFER_SIZE 101
#define COMMAND_PATH_SIZE 4096

char* findLibrary(char *command) {
    // this buffer will hold the path to the command 
    char *path = getenv("PATH");
    // because strtok changes the string, we will use it on a copy of the PATH variable.
    char pathCopy[strlen(path) + 1];
    strcpy(pathCopy, path);
    char *dir;
    // stepping over the PATH variable with ":" as the delimiter
    dir = strtok(pathCopy, ":");
    while (dir != NULL) {
        // Construct the full path to the command
        char *command_path = (char *)malloc(COMMAND_PATH_SIZE*sizeof(char));
        snprintf(command_path, COMMAND_PATH_SIZE*sizeof(char), "%s/%s", dir, command);
        // Check if the command exists in this directory using access()
        if (access(command_path, F_OK) == 0) {
            return command_path;
        }

        // Move on to the next directory
        dir = strtok(NULL, ":");
    }
    // if the path to the command was not found, return NULL
    return NULL;
}


// This function executes the shell loop of getting input from the user and executing it indefinitely
void shellLoop() {
    char userString[COMMAND_BUFFER_SIZE], userStringCpy[COMMAND_BUFFER_SIZE];
    char command[COMMAND_BUFFER_SIZE];
    while (1) {
        char *token1, *token2;
        const char delim[2] = " ";
        printf("$ ");
        fflush(stdout);
        // getting input from the user with no regards to spaces
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
        // finding the path to the command
        char *pathToDir = findLibrary(command);
        
        // forking and executing the command in the child
        int stat;
        int pid = fork();
        switch (pid) {
            case -1:
                perror("fork failed");
                break;
            case 0:
                execv(pathToDir, arguments);
                break;
            default:
                wait(&stat);
        }

        // freeing the allocated memory
        int k;
        for (k = 0; k < numTokens - 1; k++) {
            free(arguments[k]);
        }
        free(arguments);
        if (pathToDir) {
            free(pathToDir);
        }
    }
}




int main(int argc, char *argv[]) {
    // Check if number of arguments is valid
    if (argc >= 2) {
        int i;
        // this for loop adds all of the new enviroment variables to the path
        for (i = 1; i < argc; i++) {
            // first, get the current PATH
            char *old_path = getenv("PATH");
            // now, find the size of the current "PATH" variable
            int pathSize = strlen(old_path);
            // next, find the size of the new enviroment variable
            int size = strlen(argv[i]);
            // initialize a new path variable
            char new_path[size + pathSize + 2];
            // next, append the new directory to the PATH value
            snprintf(new_path, sizeof(new_path), "%s:%s", old_path, argv[i]);
            // Set the modified PATH value back into the environment without overwriting it
            setenv("PATH", new_path, 1);
        }
    }
    shellLoop();
    
    return 0;
}
