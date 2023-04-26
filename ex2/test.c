//Daniel Boianju 315113159
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <signal.h>

#define STD_OUTPUT 1
#define MAX_LINE_LENGTH_CONFIGURATION 150
#define MAX_C_FILE_PATH_LENGTH 150
#define LENGTH_OF_FIND_COMMAND 38
#define C_FILE_NAMES 100
#define NUM_ARGS_GCC 5
#define MAX_IO_LENGTH 1024

// this function kills the process
void timeout_handler() {
    kill(getpid(), SIGKILL);
}


/*
This function runs the executable with the given argument line. It does so by opening
a pipe for writing input and reading output, and saving a file with the output.
It returns 1 if the output was read and saved accordingly to a file.
It returns 0 on pipe() error, -1 on opening a new file error, -2 on write error, -3 on fork error
-4 on read error.
*/
int runFileSaveOutput(char *pathToExecutable, char *inputLine) {
    // fd will be the input & output channels to the child process. 
    int fd[2];
    printf("\nnow will run %s with input %s\n\n\n", pathToExecutable, inputLine);
    char output[MAX_IO_LENGTH];
    // open a new pipe for communication
    if (pipe(fd) < 0) {
        return 0;
    }
    int status;
    int pid;
    if ((pid = fork()) < 0) {
        close(fd[0]);
        close(fd[1]);
        return -3;
    } else if (pid == 0) { // Child process
        // redirect stdin to the read end of the pipe
        dup2(fd[0], STDIN_FILENO);
        // redirect stdout to the write end of the pipe
        
        dup2(fd[1], STDOUT_FILENO);
        puts("here");
        char* arguments[2] = {pathToExecutable, NULL};
        // Set the timeout to 5seconds
        // signal(SIGALRM, timeout_handler);
        // alarm(5);
        // Execute the executable file
        execvp(pathToExecutable, arguments);
        exit(-1);
    } else { // Parent process
        // Write the input to the end of the file
        if (write(fd[1], inputLine, strlen(inputLine)) == -1) {
            close(fd[0]);
            close(fd[1]);
            return -2;
        }
        // Wait for the child process to terminate
        waitpid(pid, &status, 0);

        // check if the child process was terminated naturally or by signal
        if (WIFEXITED(status)) { // Child process terminated normally
            // Read the output from the read end of the pipe
            if (read(fd[0], output, MAX_IO_LENGTH) == -1) {
                close(fd[0]);
                close(fd[1]);
                return -4;
            }
            printf("the output is: \n%s", output);
            close(fd[0]);
            close(fd[1]);
            return 1;
        } else if (WIFSIGNALED(status)) {
            // Child process was terminated by a signal
            close(fd[0]);
            close(fd[1]);
            return 2;
        }
    }
}

int main() {
    printf("%d",runFileSaveOutput("students/Similar_output_gets_75/a.out", "5 5"));
}