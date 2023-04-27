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


// close the configuration fd and exit with custom error message.
void closeConfigurationCustom(int confiFd, char* errorMessage) {
    if (write(STD_OUTPUT, errorMessage, strlen(errorMessage)) == -1) {
            close(confiFd);
            exit(-1);
        }
    close(confiFd);
    exit(-1);
}

// close the configuration fd and exit.
void closeConfiguration(int confiFd, char* functionError) {
    int size = strlen(functionError) + 12;
    char message[size];
    // build the error message
    snprintf(message, size, "Error in: %s\n", functionError);
    if (write(STD_OUTPUT, message, size) == -1) {
            close(confiFd);
            exit(-1);
        }
    close(confiFd);
    exit(-1);
}

// close the configuration folder, the results folder and the student folder and write error reason
void closeConfiResStudErr(int confiFd, int resultsFd, DIR *studentFolder, int errorFd, char *functionError) {
    int size = strlen(functionError) + 12;
    char message[size];
    // build the error message
    snprintf(message, size, "Error in: %s\n", functionError);
    if (write(STD_OUTPUT, message, size) == -1) {
        close(confiFd);
        closedir(studentFolder);
        close(resultsFd);
        close(errorFd);
        exit(-1);
    }
    close(confiFd);
    closedir(studentFolder);
    close(resultsFd);
    close(errorFd);
    exit(-1);
}

// close the configuration folder, the results folder and the student folder without writing
void closeConfiResStudWithoutWriting(int confiFd, int resultsFd, DIR *studentFolder, int errorFd) {
    close(confiFd);
    closedir(studentFolder);
    close(resultsFd);
    close(errorFd);
    exit(-1);
}

/*
This function returns 1 if the dirEnt is a directory, 0 otherwise.
it returns -1 on stat faliure.
*/
int isDir(char* dirEntpath) {
    // define status struct
    struct stat status;
    // check the status of the dirEnt
    if (stat(dirEntpath, &status) == -1) {
        return -1;
    }
    // check if it is a directory or not
    return S_ISDIR(status.st_mode);
}

/*
This function returns 1 if the given directory has a .c file. 0 otherwise.
if C file has been found, write it's name to the buffer
returns -1 on opendir faliure.
*/
int findCFile(char *directory, char *buffer) {
    DIR *dir = opendir(directory);
    if (dir == NULL) {
        return -1;
    }
    struct dirent *dirEntity;
    // loop over al entities in directory and check if there is a .c file present
    while ((dirEntity = readdir(dir)) != NULL) {
        int length = strlen(dirEntity->d_name);
        if (length >= 2 && dirEntity->d_name[length - 1] == 'c' && dirEntity->d_name[length - 2] == '.') {
            strcpy(buffer, dirEntity->d_name);
            closedir(dir);
            return 1;
        }
    }
    // if not found a .c file
    closedir(dir);
    return 0;
}


/*
This function reads a line from a file. If read correctly, returns 1.
If there is a reading error, returns -1.
*/ 
int readLineFromFile(int fd, char* buf) {
    char temp[1];
    int status = read(fd, temp, 1), i = 0;
    // while still reading the fie
    do {
        if (status == -1) {
            return -1;
        }
        if (temp[0] != '\n') {
            buf[i] = temp[0];
            i++;
            read(fd, temp, 1);
        } else {
            // if got here, then got to the end of the line
            buf[i] = '\0';
            return 1;
        }
    } while (status);
    // if the line does not have an '\n' character at the end but still finished reading the line
    buf[i] = '\0';
    return 1;
}

/*
This function writes the grade of the student to the results.csv file.
returns 1 on success, - on write faliure
*/
int writeToResults(int resultsFd, char *studentName, char *grade, char *reason) {
    // build the line to write
    int size = strlen(studentName) + strlen(grade) + strlen(reason) + 5;
    char lineToWrite[size];
    snprintf(lineToWrite, size, "%s,%s,%s\n", studentName, grade, reason);
    // write the grade to the results file
    if (write(resultsFd, lineToWrite, size + 1) == -1) {
        return -1;
    }
    return 1;
}

/*
This function compiles the C file that is in the directory (it assumes that there is one)
returns 1 if the file was compiled successfully, returns 0 if there was a compilation error.
returns -1 on malloc error, -2 on fork error, -3 on open error
*/
int compileCFile(char *pathToCFile, char *pathToFileDirectory) { 
    // build arguments for gcc command
    char **arguments = (char **)malloc(NUM_ARGS_GCC*sizeof(char *));
    if (arguments == NULL) {
        return -1;
    }

    // build the arguments for the gcc command
    arguments[0] = (char *)malloc(4*sizeof(char));
    arguments[1] = (char *)malloc(3*sizeof(char));
    arguments[2] = (char *)malloc((strlen(pathToFileDirectory) + 7)*sizeof(char));
    arguments[3] = (char *)malloc((strlen(pathToCFile) + 1)*sizeof(char));
    arguments[4] = NULL;
    if (arguments[0] == NULL || arguments[1] == NULL || arguments[2] == NULL || arguments[3] == NULL) {
        return -1;
    }
    strcpy(arguments[0], "gcc");
    strcpy(arguments[1], "-o");
    snprintf(arguments[2], strlen(pathToFileDirectory) + 7, "%s/a.out", pathToFileDirectory);
    strcpy(arguments[3], pathToCFile);
    int stat;
    // fork and execute the gcc command
    int pid = fork();
    switch(pid) {
        case -1: // fork error
            return -2;
        case 0: //child process
            execvp("gcc", arguments);
            puts("exec of gcc failed");
            exit(-1);
        default: //parent process
            wait(&stat);
    }
    // if the exit status was 0, the compilation was a success. Otherwise, there was a compilation error
    if (!stat) {
        return 1;
    }
    return 0;
}

// this function kills the process
void timeout_handler() {
    kill(getpid(), SIGKILL);
}

/*
This function runs the executable with the given argument line. It does so by opening
a pipe for writing input and reading output, and saving a file with the output.
It returns 1 if the output was read and saved accordingly to a file, 2 on timeout
It returns 0 on pipe() error, -1 on opening a new file error, -2 on write error, -3 on fork error
-4 on read error, -5 on waitpid error;
*/
int runFileSaveOutput(char *pathToExecutable, int inputFilefd) {
    // fd will be the input & output channels to the child process. 
    // int fd[2];
    // outputTemp will be the file that we will redirect the output to
    int outputTempFd = open("outputTemp.txt",  O_RDWR | O_CREAT | O_TRUNC, S_IRWXU);
    if (outputTempFd == -1) {
        return -1;
    }
    printf("\nnow will run %s\n\n\n", pathToExecutable);
    // char output[MAX_IO_LENGTH];
    // open a new pipe for communication
    // if (pipe(fd) < 0) {
    //     return 0;
    // }
    int pid;
    if ((pid = fork()) < 0) {
        close(outputTempFd);
        // close(fd[0]);
        // close(fd[1]);
        return -3;
    } else if (pid == 0) { // Child process
        // redirect stdin to the read input from the input file
        dup2(inputFilefd, STDIN_FILENO);

        // redirect stdout to write output to the output file
        dup2(outputTempFd, STDOUT_FILENO);
        char* arguments[2] = {pathToExecutable, NULL};
        // Set the timeout to 5seconds
        signal(SIGALRM, timeout_handler);
        alarm(5);
        // Execute the executable file
        execvp(pathToExecutable, arguments);
        exit(-1);
    } else { // Parent process

        int status;
        wait(&status);
        if (WIFEXITED(status)) { // Child process terminated normally
            close(outputTempFd);
            return 1;
        } else if (WIFSIGNALED(status)) {
            // Child process was terminated by a signal
            close(outputTempFd);
            return 2;
        }

    //     int statusIfEnded, statusIfWaiting;
    //     // first, check if the child process has exited already
    //     sleep(1);
    //     pid_t result =  waitpid(pid, &statusIfEnded, WNOHANG);
    //     if (result == -1) { // if the waitpid command failed
    //         close(outputTempFd);
    //         close(fd[0]);
    //         close(fd[1]);
    //         return -5;
    //     } else if (result == 0) { // if the child process has not exited yet
    //         puts("the child process has not exited. writing input");
    //         // Write the input to the end of the file
    //         if (write(fd[1], inputLine, strlen(inputLine)) == -1) {
    //             close(outputTempFd);
    //             close(fd[0]);
    //             close(fd[1]);
    //             return -2;
    //         }
    //         // Wait for the child process to terminate
    //         waitpid(pid, &statusIfWaiting, 0);
    //         // check if the child process was terminated naturally or by signal
    //         if (WIFEXITED(statusIfWaiting)) { // Child process terminated normally
    //             // Read the output from the read end of the pipe
    //             if (read(fd[0], output, MAX_IO_LENGTH) == -1) {
    //                 close(outputTempFd);
    //                 close(fd[0]);
    //                 close(fd[1]);
    //                 return -4;
    //             }
    //             printf("the output is: \n%s", output);
    //             if (write(outputTempFd, output, strlen(output)) == -1) {
    //                 return -2;
    //             }
    //             close(outputTempFd);
    //             close(fd[0]);
    //             close(fd[1]);
    //             return 1;
    //         } else if (WIFSIGNALED(statusIfWaiting)) {
    //             // Child process was terminated by a signal
    //             close(outputTempFd);
    //             close(fd[0]);
    //             close(fd[1]);
    //             return 2;
    //         }
    //     } else { // thi child process ended with no input
    //         puts("the child process ended with not input required");
    //         if (WIFEXITED(statusIfEnded)) { // Child process terminated normally
    //             // Read the output from the read end of the pipe
    //             if (read(fd[0], output, MAX_IO_LENGTH) == -1) {
    //                 close(outputTempFd);
    //                 close(fd[0]);
    //                 close(fd[1]);
    //                 return -4;
    //             }
    //             printf("the output is: \n%s", output);
    //             if (write(outputTempFd, output, strlen(output)) == -1) {
    //                 return -2;
    //             }
    //             close(outputTempFd);
    //             close(fd[0]);
    //             close(fd[1]);
    //             return 1;
    //         } else if (WIFSIGNALED(statusIfEnded)) {
    //             // Child process was terminated by a signal
    //             close(outputTempFd);
    //             close(fd[0]);
    //             close(fd[1]);
    //             return 2;
    //         }
    //     }
    
        
        
    }
}

int main(int argc, char* argv[]) {
    // first, check if there is a correct amount of arguments
    if (argc != 2) {
        exit(-1);
    }
    // open the configuration file for reading and check for error in opening
    int confiFd = open(argv[1], O_RDONLY);
    if (confiFd == -1) {
        if (write(STD_OUTPUT, "Error in: open\n", 16) == -1) {
            exit(-1);
        }
        exit(-1);
    }
    char studentFolderLine[MAX_LINE_LENGTH_CONFIGURATION];
    char inputFileLine[MAX_LINE_LENGTH_CONFIGURATION];
    char outputFileLine[MAX_C_FILE_PATH_LENGTH];
    // read all of the lines to the correct buffers
    if ((readLineFromFile(confiFd, studentFolderLine) == -1) || (readLineFromFile(confiFd, inputFileLine) == -1) || 
        (readLineFromFile(confiFd, outputFileLine) == -1)) {
        closeConfigurationCustom(confiFd, "read");
    }
    // check if all lines indeed correspond to valid paths
    if (access(studentFolderLine, R_OK)) {
        closeConfigurationCustom(confiFd, "Not a valid directory\n");
    }
    if (access(inputFileLine, R_OK)) {
        closeConfigurationCustom(confiFd, "Input file not exist\n");
    }
    if (access(outputFileLine, R_OK)) {
        closeConfigurationCustom(confiFd, "Output file not exist\n");
    }

    //first, reading the actual input that is to be inputted
    int inputFilefd = open(inputFileLine, O_RDONLY);
    if (inputFilefd == -1) {
        closeConfiguration(confiFd, "read");
    }
    char actualInput[MAX_IO_LENGTH];
    if (readLineFromFile(inputFilefd, actualInput) == -1) {
        close(inputFilefd);
        closeConfiguration(confiFd, "read");
    }
    close(inputFilefd);

    // open a file to direct error output to errors.txt 
    int errorFd = open("errors.txt", O_WRONLY | O_CREAT | O_APPEND | O_TRUNC, S_IRWXU);
    if (errorFd == -1) {
        closeConfiguration(confiFd, "open");
    }

    dup2(errorFd, STDERR_FILENO); // Redirect stderr to errors.txt

    // now opening a new file for results of the students
    int resultsFd = open("results.csv", O_WRONLY | O_CREAT | O_APPEND | O_TRUNC, S_IRWXU);
    // check if opening failed
    if (resultsFd == -1) {
        closeConfiguration(confiFd, "open");
    }
    DIR *studentFolder = opendir(studentFolderLine); 
    struct dirent *dirEntity;
    printf("the current pid is: %d\n", getpid());
    while ((dirEntity = readdir(studentFolder)) != NULL) {
        // if the entity is not a special saved folder
        if (strcmp(dirEntity->d_name, ".") && strcmp(dirEntity->d_name, "..")) {
            // concatenate the path to the student directory to the entity name
            int size = strlen(studentFolderLine) + strlen(dirEntity->d_name) + 2;
            char path[size];
            // define the path to the dirEnt
            snprintf(path, size, "%s/%s", studentFolderLine, dirEntity->d_name);
            // first, check if the entity is a directory.
            int isDirStatus = isDir(path);
            // if stat failed in isDir
            if (isDirStatus == -1) {
                closeConfiResStudErr(confiFd, resultsFd, studentFolder, errorFd, "stat");
            } else if (isDirStatus) { // if really is a directory
                // check if has a .c file
                char cFileName[MAX_C_FILE_PATH_LENGTH];
                int cFileStatus = findCFile(path, cFileName);
                if (cFileStatus == -1) {
                    closeConfiResStudErr(confiFd, resultsFd, studentFolder, errorFd, "opendir");
                } else if (cFileStatus == 0) {
                    if (writeToResults(resultsFd, dirEntity->d_name, "0", "NO_C_FILE") == -1) {
                        // close all opened resources and exit without writing 
                        closeConfiResStudWithoutWriting(confiFd, resultsFd, studentFolder, errorFd);
                    }
                } else {
                    int sizeC = strlen(path) + strlen(cFileName) + 2;
                    char pathToCFile[sizeC];
                    // define the path to the C file
                    snprintf(pathToCFile, sizeC, "%s/%s", path, cFileName);
                    // compile the file
                    int statusCompilation = compileCFile(pathToCFile, path);
                    // check the status of the compilation
                    switch (statusCompilation) {
                        case -1:
                            closeConfiResStudErr(confiFd, resultsFd, studentFolder, errorFd, "malloc");
                            break;
                        case -2:
                            closeConfiResStudErr(confiFd, resultsFd, studentFolder, errorFd, "fork");
                            break;
                        case -3:
                            closeConfiResStudErr(confiFd, resultsFd, studentFolder, errorFd, "open");
                            break;
                        case 0:
                            // if the compilation did not succeed, write to the results file
                            if (writeToResults(resultsFd, dirEntity->d_name, "10", "COMPILATION_ERROR") == -1) {
                                // close all opened resources and exit without writing 
                                closeConfiResStudWithoutWriting(confiFd, resultsFd, studentFolder, errorFd);
                            }
                            break;
                        default:
                            // define the path to the executable
                            int sizeExe = strlen(path) + 8;
                            char pathToExe[sizeExe];
                            // define the path to the C file
                            snprintf(pathToExe, sizeC, "%s/%s", path, "a.out");
                            // now, run the executable with the given input
                            int runStatus = runFileSaveOutput(pathToExe, inputFilefd);
                            exit(-1);
                            switch (runStatus) {
                                case 0:
                                    closeConfiResStudErr(confiFd, resultsFd, studentFolder, errorFd, "pipe");
                                    break;
                                case -1:
                                    closeConfiResStudErr(confiFd, resultsFd, studentFolder, errorFd, "open");
                                    break;
                                case -2:
                                    closeConfiResStudWithoutWriting(confiFd, resultsFd, studentFolder, errorFd);
                                    break;
                                case -3:
                                    closeConfiResStudErr(confiFd, resultsFd, studentFolder, errorFd, "fork");
                                    break;
                                case -4:
                                    closeConfiResStudErr(confiFd, resultsFd, studentFolder, errorFd, "read");
                                    break;
                                case 2:
                                    if (writeToResults(resultsFd, dirEntity->d_name, "20", "TIMEOUT") == -1) {
                                        // close all opened resources and exit without writing 
                                        closeConfiResStudWithoutWriting(confiFd, resultsFd, studentFolder, errorFd);
                                    }
                                    break;
                                default:
                                    /*
                                    if got here, then the output is saved in a file called "outputTemp.txt".
                                    now just need to compare current output with correct output 
                                    */
                                    char* arguments[4] = {"./comp.out", "outputTemp.txt", outputFileLine, NULL};
                                    int status;
                                    int pid = fork();
                                    switch(pid) {
                                        case -1: //fork failed
                                            closeConfiResStudErr(confiFd, resultsFd, studentFolder, errorFd, "fork");
                                            break;
                                        case 0: // child process
                                            execvp("./comp.out", arguments);
                                            closeConfiResStudErr(confiFd, resultsFd, studentFolder, errorFd, "execvp");
                                        default: //parent process
                                            wait(&status);
                                            int exit_status = WEXITSTATUS(status);
                                            printf("for the file %s the comparison is %d and the process is %d\n", path, exit_status, getpid());
                                            // if (!strcmp(path, "students/Excellent_gets_100")) {
                                            //     system("cat outputTemp.txt");
                                            //     exit(-1);
                                            // }
                                            puts("the cat is:");
                                            system("cat outputTemp.txt");
                                            switch (exit_status){
                                                case -1:
                                                    closeConfiResStudErr(confiFd, resultsFd, studentFolder, errorFd, "comp.out");
                                                    break;
                                                case 1:
                                                    if (writeToResults(resultsFd, dirEntity->d_name, "100", "EXCELLENT") == -1) {
                                                        // close all opened resources and exit without writing 
                                                        closeConfiResStudWithoutWriting(confiFd, resultsFd, studentFolder, errorFd);
                                                    }
                                                    break;
                                                case 2:
                                                    if (writeToResults(resultsFd, dirEntity->d_name, "50", "WRONG") == -1) {
                                                        // close all opened resources and exit without writing 
                                                        closeConfiResStudWithoutWriting(confiFd, resultsFd, studentFolder, errorFd);
                                                    }
                                                    break;
                                                case 3:
                                                    if (writeToResults(resultsFd, dirEntity->d_name, "75", "SIMILAR") == -1) {
                                                        // close all opened resources and exit without writing 
                                                        closeConfiResStudWithoutWriting(confiFd, resultsFd, studentFolder, errorFd);
                                                    }
                                                    break;
                                                default:
                                            }
                                            
                                    }
                                remove("outputTemp.txt");
                            }
                    }
                }
            }
        }
    }
    // close all files and exit
    closedir(studentFolder);
    close(confiFd);
    close(resultsFd);
    close(errorFd);
    return 0;
}