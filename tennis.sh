#bin/bash/!
# This program runs a tennis game between two players

# setting up game variables
FIRST_PLAYER_PTS=50
SECOND_PLAYER_PTS=50
FIRST_VAR=0
SECOND_VAR=0
GAME_ON=true
CURRENT_STATE=0

# this function prints the first half of the field and the points
function print_first_half {
    echo " Player 1: ${FIRST_PLAYER_PTS}         Player 2: ${SECOND_PLAYER_PTS} "
    echo " --------------------------------- "
    echo " |       |       #       |       | "
    echo " |       |       #       |       | "
}

# this function prints the second half of the field
function print_second_half {
    echo " |       |       #       |       | "
    echo " |       |       #       |       | "
    echo " --------------------------------- "
}

# these functions all correspond to printing the possible states of the game

function print_zero {
    echo " |       |       O       |       | "
}

function print_one {
    echo " |       |       #   O   |       | "
}

function print_two {
    echo " |       |       #       |   O   | "
}

function print_three {
    echo " |       |       #       |       |O"
}

function print_minus_one {
    echo " |       |   O   #       |       | "
}

function print_minus_two {
    echo " |   O   |       #       |       | "
}

function print_minus_three {
    echo "O|       |       #       |       | "
}

# this function prints the current state of the game
function print_curr_state {
    print_first_half
    case $CURRENT_STATE in
    0)
        print_zero
        ;;
    1)
        print_one
        ;;
    2)
        print_two
        ;;
    3)
        print_three
        ;;
    -1)
        print_minus_one
        ;;
    -2)
        print_minus_two
        ;;
    -3)
        print_minus_three
        ;;
    esac
    print_second_half
}

# this function reads a for the first player and asserts its validety
function read_valid_first_player {
    FIRST_PLAYER_INPUT=false
    while [ "$FIRST_PLAYER_INPUT" = false ]
    do
        read -s FIRST_VAR
        # first, make sure that the read number is a positive integer
        if [[ $FIRST_VAR =~ '^[0-9]+$' ]]; then
            echo "NOT A VALID MOVE !"
            continue
        fi
        # if so, then check if it is in the correct range
        if [ $FIRST_VAR -gt $FIRST_PLAYER_PTS ]; then
            echo "NOT A VALID MOVE !"
            continue
        fi
        # if got here, the read input is correct
        FIRST_PLAYER_INPUT=true
    done
}

# this function reads a for the second player and asserts its validety
function read_valid_second_player {
    SECOND_PLAYER_INPUT=false
    while [ "$SECOND_PLAYER_INPUT" = false ]
    do
        read -s SECOND_VAR
        # first, make sure that the read number is a positive integer
        if [[ $SECOND_VAR =~ '^[0-9]+$' ]]; then
            echo "NOT A VALID MOVE !"
            continue
        fi
        # if so, then check if it is in the correct range
        if [ $SECOND_VAR -gt $SECOND_PLAYER_PTS ]; then
            echo "NOT A VALID MOVE !"
            continue
        fi
        # if got here, the read input is correct
        SECOND_PLAYER_INPUT=true
    done
}

while [ "$GAME_ON" = true ]
do
    print_curr_state
    echo "PLAYER 1 PICK A NUMBER: "
    read_valid_first_player
    echo "PLAYER 2 PICK A NUMBER: "
    read_valid_second_player
    echo $FIRST_VAR
    GAME_ON=false
done




# function ask_for_points {
  
# }
