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

#define STD_OUTPUT 1
#define MAX_LINE_LENGTH_CONFIGURATION 150
#define MAX_C_FILE_PATH_LENGTH 150
#define LENGTH_OF_FIND_COMMAND 38
#define C_FILE_NAMES 100

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
    // chack if it is a directory or not
    return S_ISDIR(status.st_mode);
}

/*
This function returns 1 if the given directory has a .c file. 0 otherwise.
returns -1 on command faliure, -2 on pipe opening faliure.
*/
int hasCFile(char *directory) {
    int size = strlen(directory) + LENGTH_OF_FIND_COMMAND;
    // command will be the find command that we run, cFiles will be the buffer for the command output
    char command[size], cFiles[C_FILE_NAMES];
    snprintf(command, size, "find %s -maxdepth 1 -name \"*.c\" -type f", directory);
    // open a pipe for reading the output
    FILE *pipe = popen(command, "r");
    fgets(cFiles, sizeof(cFiles), pipe);
    printf("current directory: %s, the files found are:", directory);
    puts(cFiles);
    pclose(pipe);
    return 1;
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
    return 1;
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
        if (write(STD_OUTPUT, "Error in: read\n", 16) == -1) {
            close(confiFd);
            exit(-1);
        }
        close(confiFd);
        exit(-1);
    }
    // check if all lines indeed correspond to valid paths
    if (access(studentFolderLine, R_OK)) {
        if (write(STD_OUTPUT, "Not a valid directory\n", 23) == -1) {
            close(confiFd);
            exit(-1);
        }
        close(confiFd);
        exit(-1);
    }
    if (access(inputFileLine, R_OK)) {
        if (write(STD_OUTPUT, "Input file not exist\n", 22) == -1) {
            close(confiFd);
            exit(-1);
        }
        close(confiFd);
        exit(-1);
    }
    if (access(outputFileLine, R_OK)) {
        if (write(STD_OUTPUT, "Output file not exist\n", 23) == -1) {
            close(confiFd);
            exit(-1);
        }
        close(confiFd);
        exit(-1);
    }
    // now opening a new file for results of the students
    int resultsFd = open("results.csv", O_CREAT, S_IRWXU);
    // check if opening failed
    if (resultsFd == -1) {
        if (write(STD_OUTPUT, "Error in: open\n", 16) == -1) {
            close(confiFd);
            exit(-1);
        }
        close(confiFd);
        exit(-1);
    }

    /*
    Next, go over all folders in students directory, and compile the c files inside them.
    also signal if no such file exists.
    */
    DIR *studentFolder = opendir(studentFolderLine); 
    struct dirent *dirEntity;
    while ((dirEntity = readdir(studentFolder)) != NULL) {
        // if the entity is not a special saved folder
        if (strcmp(dirEntity->d_name, ".") && strcmp(dirEntity->d_name, "..")) {
            // concatenate the path to the student directory to the entity name
            int size = strlen(studentFolderLine) + strlen(dirEntity->d_name) + 2;
            char path[size];
            // define the path to the dirEnt
            snprintf(path, size, "%s/%s", studentFolderLine, dirEntity->d_name);
            hasCFile(path);
        }
        
    }


    // close all files and exit
    closedir(studentFolder);
    close(confiFd);
    close(resultsFd);
    return 0;
}