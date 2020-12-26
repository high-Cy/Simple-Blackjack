/* A simple blackjack program ---------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include <ctype.h>
#include <math.h>

/* Defines-------------------------------------------------------------------*/

#define CH_NL       '\n'

#define N_SUIT      4
#define N_RANK      13
#define N_DECK      52
#define N_CARD      2

#define TRUE        1
#define FALSE       0

#define START_BAL   1000

/* Typedef----------------------------------------------------------*/

// struct with ranks and their corresponding value
typedef struct {
    int rank_val[N_RANK];     // value of each rank
    char *rank_str[N_RANK];    // name of rank
} ranks_t;

typedef struct {
    char *suit_str[N_SUIT];    // short-form name of suits
    enum {DIAMONDS, CLUBS, HEARTS, SPADES} suit_name; // for printing
} suits_t;

typedef struct {
    int balance;
    int to_bet;
} credits_t;

/* Function Prototypes-------------------------------------------------------*/

void print_prompt(void);
void malloc_deck(char**);
void free_deck(char**);
void form_deck(char**, char**, char**);
void shuffle_deck(char**);
void swap_card(char*, char*);
void print_deck(char**);
void get_bet(credits_t*);
int valid_bet(char*, int);

int
main(int argc, char *argv[]) {
    ranks_t ranks = {
        {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 10},
        {"A","2","3","4","5","6","7","8","9","0","J","Q","K"}
    };
    suits_t suits = {
        {"D", "C", "H", "S"}
    };

    // allocate space for deck
    char **deck = malloc(N_DECK * sizeof(char*));
    assert(deck);
    malloc_deck(deck);

    form_deck(ranks.rank_str, suits.suit_str, deck);
    shuffle_deck(deck);
    print_deck(deck);

    printf("Welcome stranger, to this Simple Blackjack Simulator!\n"
            "You'll be given %d credits to start off\n"
            "Game ends when you lose all the credits.\n\n", START_BAL);

    credits_t credits;
    credits.balance=START_BAL;
    get_bet(&credits);

    free_deck(deck);
    deck = NULL;
    return 0;
}

void
print_prompt(void) {
    printf("-> ");
}

/* Allocate space for each card */
void
malloc_deck(char **deck) {
    int i;
    for (i=0; i<N_DECK; i++) {
        deck[i] = malloc((N_CARD+1)*sizeof(char));
        assert(deck[i]);
    }
}

/* Free space of each card and deck itself */
void
free_deck(char **deck) {
    int i;
    for (i=0; i<N_DECK; i++) {
        free(deck[i]);
    }
    free(deck);
}

/* Creates deck with 52 cards */
void
form_deck(char *ranks[], char *suits[], char **deck) {
    int i, j, k=0;
    char tmp_card[N_CARD+1]={'\0'};
    for (i=0; i<N_RANK; i++) {
        for (j=0; j<N_SUIT; j++) {
            strcat(tmp_card, ranks[i]);
            strcat(tmp_card, suits[j]);
            strcpy(deck[k], tmp_card);
            memset(tmp_card, 0, sizeof(tmp_card));
            k++;
        }
    }
}

/* Modified Fisher-Yates shuffle algorithm to fit array of strings */
void
shuffle_deck(char **deck) {
    int i, j;
    char *tmp_card;
    
    // diferent seed for rand() every time program is ran
    srand(time(NULL));
    
    for (i=N_DECK-1; i>0; i--) {
        j = rand() % N_DECK;
        tmp_card = deck[i];
        deck[i] = deck[j];
        deck[j] = tmp_card;
    }

}

/* Swap 2 pointers */
void
swap_card(char *a, char *b) {
    char tmp = *a;
    *a = *b;
    *b = tmp; 
}

void
print_deck(char **deck) {
    int i;
    for (i=0; i<N_DECK; i++) {
        puts(deck[i]);
    }
}

void
get_bet(credits_t *credits) {
    int value, valid;
    int n = floor(log10(abs(START_BAL))) + 1; // number of digits 
    char input[n];

    printf("Place your bets: \n");
    print_prompt();

    do {
        memset(input, 0, sizeof(input));
        fgets(input, sizeof(input), stdin);

        valid = sscanf(input, "%d", &value);

        if (valid!=1) {
            printf("Input was not a number. Try again.\n");
            print_prompt();

        } else if (value>credits->balance || value<=0) {
            printf("Input was out of bounds. Try again.\n");
            print_prompt();

        } 
        
    } while (valid!=1 || value>credits->balance || value<=0);

    credits->to_bet = value;
    credits->balance -= value;
}