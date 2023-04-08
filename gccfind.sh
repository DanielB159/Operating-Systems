# This program accepts a folder path and compiles all of the files in it
# first, changing the directory to be argument1 and cleaning all compiled files
if [ $# -eq 2 ]; then
    cd $1
    rm *.out
    #next, compiling all c files with which have argument2 in them.
    find -name "*.c" -type f -print0 | xargs -0 grep -l --null "$2" | xargs -0 sh -c 'for file do gcc -o "${file%.c}".out -w "$file"; done' sh 
    # the find command finds all the c files in the given directory. It then sends output to the grep
    # the grep command finds all given files that have argument 2 in them. It then send them to the next command
    # the the next command is a for loop executed in a different shell (sh -c), it compiles all the files and outputs compiled files with the respective names
fi
if [ $# -eq 3 ]; then
    case "$3" in
        -r)
            flag_r=true
            ;;
        *)
            echo "invalid flag!"
            exit 1
            ;;
        esac
    # while getopts "r" opt; do 
    #     echo "got here 1.5"  
    #     case $opt in
    #         r)
    #             flag_r=true
    #             ;;
    #         \?)
    #             echo "invalid flag!"
    #             exit 1
    #             ;;
    #     esac
    # done
    if [ "$flag_r" = true ]; then
        echo "got here2"
        $($0 $1 $2)
        
        for dir in $(ls -d $1); do
            echo "$dir"
            echo "$dir"
            echo "$dir"
            echo "$dir"
            echo "$dir"
            if [ "$dir" -eq "." ]; then
                echo "aAAAAAAAAAAAAAAAAAAAAAAAAAAH"
                exit 1
            fi
            $($0 $1$dir $2 "-r")
        done
    fi
fi
if [ $# != 2 ] && [ $# != 3 ]; then
    echo "Not enough parameters"
fi