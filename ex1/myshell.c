//Daniel Boianju 315113159
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "unistd.h"
#include "sys/types.h"
#include "sys/wait.h"
#include "math.h"

#define COMMAND_BUFFER_SIZE 101
#define COMMAND_PATH_SIZE 4096
#define MAX_NUM_COMMANDS 100
#define MAX_PID_CHAR_LENGTH 10

/*
this function recieves a command string and returns its path, or NULL if there isnt one.
If the malloc fails, the findLibrary function returns the pointer to the command, indicating to perror and exit.
*/
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
        if (command_path == NULL) {
            // if the memory allocation has failed, notify the shell to free memory and perror
            return command;
        }
        snprintf(command_path, COMMAND_PATH_SIZE*sizeof(char), "%s/%s", dir, command);
        // Check if the command exists in this directory using access()
        if (access(command_path, F_OK) == 0) {
            return command_path;
        }
        // the command does not exist. Free the allocated memory
        free(command_path);
        // Move on to the next directory
        dir = strtok(NULL, ":");
    }
    // if the path to the command was not found, return NULL
    return NULL;
}

// this function frees the arguments saved and the history of commands saved
void freeArgumentsAndHistory(char** arguments, char** historyCommands, int numTokens, int curNumCummands) {
    int l;
    for (l = 0; l < numTokens; l++) {
        free(arguments[l]);
    }
    free(arguments);
    for (l = 0; l < curNumCummands; l++) {
        free(historyCommands[l]);
    }
}

// this function frees only the arguments saved
void freeArguments(char** arguments, int numTokens) {
    int h;
    for (h = 0; h < numTokens; h++) {
        free(arguments[h]);
    }
    free(arguments);
}

// This function executes the shell loop of getting input from the user and executing it indefinitely
void shellLoop() {
    char* historyCommands[MAX_NUM_COMMANDS];
    int curNumCummands = 0;
    while (1) {
        char userString[COMMAND_BUFFER_SIZE], userStringCpy1[COMMAND_BUFFER_SIZE], userStringCpy2[COMMAND_BUFFER_SIZE];
        char command[COMMAND_BUFFER_SIZE];
        char *token1, *token2;
        const char delim[2] = " ";
        printf("$ ");
        fflush(stdout);
        // getting input from the user with no regards to spaces
        scanf(" %[^\n]%*c", userString);
        // copying the command for second strtok 
        strcpy(userStringCpy1, userString);
        strcpy(userStringCpy2, userString);
        token1 = strtok(userStringCpy1, delim);
        int numTokens = 0;
        // this while loop checks first how many arguments there are with the command
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
        // if the allocation has failed, free all memory and perror
        if (arguments == NULL) {
            int t;
            for (t = 0; t < curNumCummands; t++) {
                free(historyCommands[t]);
            }
            perror("malloc failed");
            exit(1);
        }
        // tokenizing the userString again but saving all of the arguments
        token2 = strtok(userStringCpy2, delim);
        int i;
        for (i = 0; token2 != NULL; i++) {
            arguments[i] = (char*)malloc(strlen(token2) + 1);
            // if the allocation has failed, free all memory and perror
            if (arguments[i] == NULL) {
                freeArgumentsAndHistory(arguments, historyCommands, numTokens, curNumCummands);
                perror("malloc failed");
                exit(1);
            }
            strcpy(arguments[i], token2);
            token2 = strtok(NULL, delim);
        }
        // setting the final argument to be NULL
        arguments[i] = NULL;
        // if the command is the built in command: "history", execute it.
        if (!strcmp(command, "history")) {
            int commandLength = (MAX_PID_CHAR_LENGTH + strlen(userString) + 2)*sizeof(char);
            historyCommands[curNumCummands] = (char *)malloc(commandLength);
            // check if the malloc failed. If so, free all allocated memory and perror
            if (historyCommands[curNumCummands] == NULL) {
                freeArgumentsAndHistory(arguments, historyCommands, numTokens, curNumCummands);
                perror("malloc failed");
                exit(1);
            }
            snprintf(historyCommands[curNumCummands], commandLength, "%d %s", getpid(), userString);
            curNumCummands++;
            int c;
            for (c = 0; c < curNumCummands; c++) {
                printf("%s\n", historyCommands[c]);
            }
        } else if (!strcmp(command, "cd")) { // if the command is the built in command: "cd", execute it.
            // Check if the command exists in this directory using access()
            if (access(arguments[1], F_OK) == 0) {
                // cd is implemented using chdir 
                chdir(arguments[1]);
            } else {
                perror("cd failed");
            }
            int commandLength = (MAX_PID_CHAR_LENGTH + strlen(userString) + 2)*sizeof(char);
            historyCommands[curNumCummands] = (char *)malloc(commandLength);
            // check if the malloc failed. If so, free all allocated memory and perror
            if (historyCommands[curNumCummands] == NULL) {
                freeArgumentsAndHistory(arguments, historyCommands, numTokens, curNumCummands);
                perror("malloc failed");
                exit(1);
            }
            snprintf(historyCommands[curNumCummands], commandLength, "%d %s", getpid(), userString);
            curNumCummands++;
            // if the command is the built in command: "exit", free all memory and return.
        } else if (!strcmp(command, "exit")) { 
            freeArgumentsAndHistory(arguments, historyCommands, numTokens, curNumCummands);
            exit(1);
        } else { // if the command is not a built in function 
            // finding the path to the command
            char *pathToDir = findLibrary(command);
            // check to see if the malloc in the function falied
            if (pathToDir == command) {
                freeArgumentsAndHistory(arguments, historyCommands, numTokens, curNumCummands);
                perror("malloc failed");
                exit(1);
            }
            else if (pathToDir == NULL) {
                /*
                add the executed command to the history of commands, and then notify that there is no such command.
                the size allocted for the PID is the maximum number of characters for a process id, the length of the 
                command string and another two characters for the ' ' in between and the '\0' in the end
                */
                int commandLength = (MAX_PID_CHAR_LENGTH + strlen(userString) + 2)*sizeof(char);
                historyCommands[curNumCummands] = (char *)malloc(commandLength);
                // check if the malloc failed. If so, free all allocated memory and perror
                if (historyCommands[curNumCummands] == NULL) {
                    freeArgumentsAndHistory(arguments, historyCommands, numTokens, curNumCummands);
                    perror("malloc failed");
                    exit(1);
                }
                snprintf(historyCommands[curNumCummands], commandLength, "%d %s", getpid(), userString);
                curNumCummands++;
                // next, release all current loop allocated memory and perror that no command was found
                // freeing the allocated memory for the arguments
                char errString[strlen(command) + 8];
                strcpy(errString, command);
                strcpy(errString + strlen(errString), " failed");
                perror(errString);
            } else {
                // forking and executing the command in the child
                int stat;
                int pid = fork();
                switch (pid) {
                    // if the fork failed
                    case -1:
                        freeArguments(arguments, numTokens);
                        if (pathToDir) {
                            free(pathToDir);
                        }
                        perror("fork failed");
                        break;
                    // if this is the child, execute the program
                    case 0:
                        // if there really was a path to a command, execute it
                        if (pathToDir != NULL) {
                            execv(pathToDir, arguments);
                        } else { // otherwise, terminate
                            exit(1);
                        }
                        break;
                    // otherwise, this is the parent, wait for the child
                    default:
                        //likewise, if there really was a path to the command, wait for the child process to terminate
                        if (pathToDir != NULL) {
                            wait(&stat);;
                        }
                        /*
                        add the executed command to the history of commands. 
                        the size allocted for the PID is the maximum number of characters for a process id, the length of the 
                        command string and another two characters for the ' ' in between and the '\0' in the end
                        */
                        int commandLength = (MAX_PID_CHAR_LENGTH + strlen(userString) + 2)*sizeof(char);
                        historyCommands[curNumCummands] = (char *)malloc(commandLength);
                        // check if the malloc failed. If so, free all allocated memory and perror
                        if (historyCommands[curNumCummands] == NULL) {
                            freeArgumentsAndHistory(arguments, historyCommands, numTokens, curNumCummands);
                            perror("malloc failed");
                            exit(1);
                        }
                        snprintf(historyCommands[curNumCummands], commandLength, "%d %s", pid, userString);
                        curNumCummands++;
                        // if there was a valid path, free the memory that was allocated
                        if (pathToDir) {
                            free(pathToDir);
                        }
                }
            }
            
        }
        // freeing the allocated memory for the arguments
        freeArguments(arguments, numTokens);
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
    // the shellLoop function is the function that runs the loop of the mini-shell
    shellLoop();
    return 0;
}