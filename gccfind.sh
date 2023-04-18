#!/bin/bash
#Daniel Boianju 315113159
# This program accepts a folder path and compiles all of the files in it

# first, check if there are 2 arguments. If so, work only in the given directory without sub-directories
if [ $# -eq 2 ]; then
    # first, removing all of the compiled files only in this directory
    rm $1/*.out
    # next, compiling all c files with which have argument2 in them. Limit only to this folder
    find $1  -maxdepth 1 -name "*.c" -type f -print0 | xargs -0 grep -l -i --null "\b$2[^a-z]*\b" | xargs -0 sh -c 'for file do gcc -o "${file%.c}".out -w "$file"; done' sh 
    # the find command finds all the c files in the given directory. It then sends output to the grep
    # the grep command finds all given files that have argument 2 in them. It then send them to the next command
    # the the next command is a for loop executed in a different shell (sh -c), it compiles all the files and outputs compiled files with the respective names
fi
# if there are three arguments, check if the third is a valid flag
if [ $# -eq 3 ]; then
    case "$3" in
        -r)
            # if it is a valid flag, set the flag_r to be true
            flag_r=true
            ;;
        *)
            # otherwise, exit
            exit 1
            ;;
        esac
    # got here only if the flag is valid
    # delete all compiled files also in subdirectories 
    find $1 -name "*.out" -type f -delete;
    # compile all files also in sub-directories
    find $1 -name "*.c" -type f -print0 | xargs -0 grep -i -l --null "\b$2[^a-z]*\b" | xargs -0 sh -c 'for file do gcc -o "${file%.c}".out -w "$file"; done' sh 
fi
# if there are less than two arguments or more than three arguments, notify the user and end 
if [ $# != 2 ] && [ $# != 3 ]; then
    echo "Not enough parameters"
fi
