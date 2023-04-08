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

# this function prints the current board of the game
function print_curr_board {
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
        if ! [[ $FIRST_VAR =~ ^[0-9]+$ ]]; then
            echo "NOT A VALID MOVE !"
            continue
        fi
        # if so, then check if it is in the correct range
        if [ $FIRST_VAR -gt $FIRST_PLAYER_PTS ] || [ $FIRST_VAR -lt 0 ]; then
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
        if ! [[ $SECOND_VAR =~ ^[0-9]+$ ]]; then
            echo "NOT A VALID MOVE !"
            continue
        fi
        # if so, then check if it is in the correct range
        if [ $SECOND_VAR -gt $SECOND_PLAYER_PTS ] || [ $SECOND_VAR -lt 0 ]; then
            echo "NOT A VALID MOVE !"
            continue
        fi
        # if got here, the read input is correct
        SECOND_PLAYER_INPUT=true
    done
}

# this function updates the game state according to the choice of the players
function update_game_state {
    # if the first player won this point
    if [ $FIRST_VAR -gt $SECOND_VAR ]; then
        if [ $CURRENT_STATE -lt 0 ]; then
            CURRENT_STATE=1
        else
            CURRENT_STATE=$(($CURRENT_STATE + 1))
        fi
    fi
    # if the second player won this point
    if [ $FIRST_VAR -lt $SECOND_VAR ]; then
        if [ $CURRENT_STATE -gt 0 ]; then
            CURRENT_STATE=-1
        else
            CURRENT_STATE=$(($CURRENT_STATE - 1))
        fi
    fi
    # if there is a draw, the current state stays the same
}

# this function checks weather the game has ended
function check_if_ended {
    # firstly, check if one of the edges of the board was hit
    if [ $CURRENT_STATE -eq 3 ]; then
        GAME_ON=false
        echo "PLAYER 1 WINS !"
    elif [ $CURRENT_STATE -eq -3 ]; then
        GAME_ON=false
        echo "PLAYER 2 WINS !"
    # next, check if one of the players lost all points
    elif [ $SECOND_PLAYER_PTS -eq 0 ] && [ $FIRST_PLAYER_PTS -gt 0 ]; then
        GAME_ON=false
        echo "PLAYER 1 WINS !"
    elif [ $FIRST_PLAYER_PTS -eq 0 ] && [ $SECOND_PLAYER_PTS -gt 0 ]; then
        GAME_ON=false
        echo "PLAYER 2 WINS !"
    # finally, check if both players have zero points
    elif [ $FIRST_PLAYER_PTS -eq 0 ] && [ $SECOND_PLAYER_PTS -eq 0 ]; then
        GAME_ON=false
        if [ $CURRENT_STATE -gt 0 ]; then
            echo "PLAYER 1 WINS !"
        elif [ $CURRENT_STATE -lt 0 ]; then
            echo "PLAYER 2 WINS !"
        else
            echo "IT'S A DRAW !"
        fi
    fi
}


# this is the main script that uses all of the above functions
print_curr_board
while [ "$GAME_ON" = true ]
do
    echo "PLAYER 1 PICK A NUMBER: "
    read_valid_first_player
    echo "PLAYER 2 PICK A NUMBER: "
    read_valid_second_player
    # decrease each player with the points they chose
    FIRST_PLAYER_PTS=$(($FIRST_PLAYER_PTS - $FIRST_VAR))
    SECOND_PLAYER_PTS=$(($SECOND_PLAYER_PTS - $SECOND_VAR))
    # update the current state of the game
    update_game_state
    print_curr_board
    # print the plays of the players
    echo -e "       Player 1 played: ${FIRST_VAR}\n       Player 2 played: ${SECOND_VAR}\n\n"
    # check to see if game ended
    check_if_ended
done