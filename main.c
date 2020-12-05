#include <stdio.h>
#include <stdlib.h>

/* Global Variables----------------------------------------------------------*/

// struct with ranks and their corresponding value
struct ranks_dict {
    char key[13]; 
    int value[13]; 
} ranks = {    
        {'A','2','3','4','5','6','7','8','9','0','J','Q','K'},
        {1,2,3,4,5,6,7,8,9,10,10,10,10}
    };

// the suits
char suits[4] = {'D', 'C', 'H', 'S'};

int
main(int argc, char *argv[]) {
    /* code */
    return 0;
}

