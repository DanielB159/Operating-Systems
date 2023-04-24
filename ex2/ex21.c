//Daniel Boianju 315113159
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>



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


    // closing the opened files
    close(fd1);
    close(fd2);
}