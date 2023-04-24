//Daniel Boianju 315113159
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>

typedef struct HashItem{
    char key;
    int numTimes;
    struct HashItem *next;
} HashItem;

// this function checks weather the files described by fd1 and fd2 are identical
int areIdentical(int fd1, int fd2) {
    char file1read[1], file2read[1];
    //read the first byre of the file.
    int readOut1 = read(fd1, file1read, 1), readOut2 = read(fd2, file2read, 1);
    // while at least one of the files is still being read
    while (readOut1 || readOut2) {
        // check if the reading has failed for one of the files
        if (readOut1 == -1 || readOut2 == -1) {
            if (write(1, "Error in: access\n", 18) == -1) {
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
        exit(1);
    }
    // now, check that the paths indeed exist
    if (!(access(argv[1], F_OK) == 0) || !(access(argv[2], F_OK) == 0)) {
        if (write(1, "Error in: access\n", 18) == -1) {
            exit(-1);
        }
        exit(-1);
    }
    // if got here, both files exist, and thus can be opened
    int fd1 = open(argv[1], O_RDONLY), fd2 = open(argv[2], O_RDONLY);
    if (fd1 == -1 || fd2 == -1) {
        if (write(1, "Error in: open\n", 16) == -1) {
            exit(-1);
        }
        exit(-1);
    }

    // first, check if the files are identical
    if (areIdentical(fd1, fd2)) {
        return 1;
    }
    // DANIEL IMPLEMENT RE READING USING SEEK OR SOMETGHING

    // opening the files for a second reading
    fd1 = open(argv[1], O_RDONLY), fd2 = open(argv[2], O_RDONLY);
    if (fd1 == -1 || fd2 == -1) {
        if (write(1, "Error in: open\n", 16) == -1) {
            exit(-1);
        }
        exit(-1);
    }

    char file1read[1], file2read[1];
    //read the first byre of the file.
    while (read(fd1, file1read, 1) && read(fd2, file2read, 1)) {
        printf("%c", file1read[0]);
        printf("%c", file2read[0]);
    }


    // closing the opened files
    close(fd1);
    close(fd2);   
}