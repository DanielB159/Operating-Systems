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
            exit(-1);
        }
        exit(-1);
    }
    // check if all lines indeed correspond to valid paths
    if (!access(studentFolderLine, R_OK)) {
        if (write(STD_OUTPUT, "Not a valid directory\n", 23) == -1) {
            exit(-1);
        }
        exit(-1);
    }
    if (!access(inputFileLine, R_OK)) {
        if (write(STD_OUTPUT, "Input file not exist\n", 22) == -1) {
            exit(-1);
        }
        exit(-1);
    }
    if (!access(outputFileLine, R_OK)) {
        if (write(STD_OUTPUT, "Output file not exist\n", 23) == -1) {
            exit(-1);
        }
        exit(-1);
    }
    


    close(confiFd);
}