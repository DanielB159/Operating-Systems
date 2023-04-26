//Daniel Boianju 315113159
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>

#define STD_OUTPUT 1
#define CASE_DIFFERENCE 32

/*
this function copies the file, without ' ' or '\n' to the arr, assuming it has enough space.
The copying is done case insensitive. 
returns -1 on read error, 1 on copy success.
*/
int copyToArr(int fd, char *buff) {
    int i = 0;
    char temp[1];
    int status = read(fd, temp, 1);
    // continue reading while have not reached the end of file
    do {
        if (status == -1) {
            return -1;
        }
        if (temp[0] != ' ' && temp[0] != '\n') {
            // if the character is Uppercase, convert to lower and add
            if (temp[0] >= 'A' && temp [0] <= 'Z') {
                buff[i] = temp[0] + CASE_DIFFERENCE;
            } else {
                buff[i] = temp[0];
            }
            i++;
        }
        status = read(fd, temp, 1);
    } while (status != 0);
    
    return 1;
}


// this function returns the length of the file without spaces or '\n'. returns -1 if read failed
int checkLength(int fd) {
    int size = 0;
    char temp[1];
    int status = read(fd, temp, 1);
    // continue reading while have not reached the end of file
    do {
        if (status == -1) {
            return -1;
        }
        if (temp[0] != ' ' && temp[0] != '\n') {
            size++;
        }
        status = read(fd, temp, 1);
    } while (status != 0);
    
    // return the size plus one for null terminator
    return size + 1;
}

// this function checks weather the files described by fd1 and fd2 are similar
int areSimilar(int fd1, int fd2) {
    // first, check the length of the files without ' ' or '\n'
    int size1 = checkLength(fd1), size2 = checkLength(fd2);
    if (size1 == -1 || size2 == -1) {
        close(fd1);
        close(fd2);
        if (write(STD_OUTPUT, "Error in: read\n", 16) == -1) {
            exit(-1);
        }
        exit(-1);
    }
    // check if one of the files contains only spaces and the other does not.
    if ((!size1 && size2) || (size1 && !size2)) {
        return 0;
    }
    // check if t=both files contain only spaces
    if (!size1 && !size2) {
        return 1;
    }

    // allocate enough space for the files
    char *firstFile = (char *)malloc(sizeof(char)*size1);
    char *secondFile = (char *)malloc(sizeof(char)*size2);
    if (firstFile == NULL || secondFile == NULL) {
        close(fd1);
        close(fd2);
        if (write(STD_OUTPUT, "Error in: malloc\n", 18) == -1) {
            exit(-1);
        }
        exit(-1);
    }

    // reset the offset in both files
    if (!(!(lseek(fd1, 0, SEEK_SET) == -1) && !(lseek(fd2, 0, SEEK_SET) == -1))) {
        if (write(STD_OUTPUT, "Error in: lseek\n", 17) == -1) {
            close(fd1);
            close(fd2);
            exit(-1);
        }
        exit(-1);
    }

    // copy the files to the buffers
    if ((copyToArr(fd1, firstFile) == -1) || (copyToArr(fd2, secondFile) == -1)) {
        if (write(STD_OUTPUT, "Error in: read\n", 16) == -1) {
            close(fd1);
            close(fd2);
            exit(-1);
        }
        exit(-1);
    }
    
    // configure the last character to be null terminator
    firstFile[size1 - 1] = '\0';
    firstFile[size1 - 1] = '\0';

    // if the files are equal, return 1. otherwise, return 0;
    return !strcmp(firstFile, secondFile);
}


// this function checks weather the files described by fd1 and fd2 are identical
int areIdentical(int fd1, int fd2) {
    char file1read[1], file2read[1];
    //read the first byte of the file.
    int readOut1 = read(fd1, file1read, 1), readOut2 = read(fd2, file2read, 1);
    // while at least one of the files is still being read
    while (readOut1 || readOut2) {
        // check if the reading has failed for one of the files
        if (readOut1 == -1 || readOut2 == -1) {
            if (write(STD_OUTPUT, "Error in: read\n", 16) == -1) {
                close(fd1);
                close(fd2);
                exit(-1);
            }
            exit(-1);
        }
        // next, check if one of the files ended before the other
        if (readOut1 && !readOut2) {
            return 0;
        }
        if (!readOut1 && readOut2) {
            return 0;
        }
        // next, check if the read byte is equal
        if (file1read[0] != file2read[0]) {
            return 0;
        }
        // keep reading
        readOut1 = read(fd1, file1read, 1);
        readOut2 = read(fd2, file2read, 1);
    }
    // if passed all tests, the files are indeed equal
    return 1;
}

int main(int argc, char *argv[]) {
    
    // first, make sure that indeed got 2 arguments in addition to a.out
    if (argc != 3) {
        exit(-1);
    }
    printf("now checking files: %s and %s...\n", argv[1], argv[2]);
    // now, check that the paths indeed exist
    if (!(access(argv[1], F_OK) == 0) || !(access(argv[2], F_OK) == 0)) {
        if (write(STD_OUTPUT, "Error in: access\n", 18) == -1) {
            exit(-1);
        }
        exit(-1);
    }
    // if got here, both files exist, and thus can be opened
    int fd1 = open(argv[1], O_RDONLY), fd2 = open(argv[2], O_RDONLY);
    if (fd1 == -1 || fd2 == -1) {
        if (write(STD_OUTPUT, "Error in: open\n", 16) == -1) {
            exit(-1);
        }
        exit(-1);
    }

    // first, check if the files are identical
    if (areIdentical(fd1, fd2)) {
        puts("returning 1");
        return 1;
    }

    // resetting the offset in the files that have been read
    if (!(!(lseek(fd1, 0, SEEK_SET) == -1) && !(lseek(fd2, 0, SEEK_SET) == -1))) {
        if (write(STD_OUTPUT, "Error in: lseek\n", 16) == -1) {
            close(fd1);
            close(fd2);
            exit(-1);
        }
        exit(-1);
    }

    if (areSimilar(fd1, fd2)) {
        puts("returning 3");
        return 3;
    }

    // closing the opened files
    close(fd1);
    close(fd2);
    puts("returning 2");
    return 2; 
}