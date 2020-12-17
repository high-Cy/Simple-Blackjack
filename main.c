/* A simple blackjack program - */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>

/* Defines-------------------------------------------------------------------*/

#define SUIT_NUM    4
#define RANK_NUM    13
#define DECK_NUM    52
#define CARD_SIZE   2

/* Typedef----------------------------------------------------------*/

// struct with ranks and their corresponding value
typedef struct {
    int rank_val[RANK_NUM];     // value of each rank
    char *rank_str[RANK_NUM];    // name of rank
} ranks_t;

typedef struct {
    char *suit_str[SUIT_NUM];    // short-form name of suits
    enum {DIAMONDS, CLUBS, HEARTS, SPADES} suit_name; // for printing
} suits_t;

/* Function Prototypes-------------------------------------------------------*/

void malloc_deck(char**);
void free_deck(char**);
void form_deck(char**, char**, char**);
void shuffle_deck(char**);
void swap_card(char*, char*);
void print_deck(char**);

int
main(int argc, char *argv[]) {
    ranks_t ranks = {
        {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 10},
        {"A","2","3","4","5","6","7","8","9","0","J","Q","K"}
    };
    suits_t suits = {
        {"D", "C", "H", "S"}
    };
    int i, j;
    char **deck = malloc(DECK_NUM * sizeof(char*));

    malloc_deck(deck);

    form_deck(ranks.rank_str, suits.suit_str, deck);
    
    shuffle_deck(deck);
    print_deck(deck);

    /*for (i=0; i<DECK_NUM; i++) {
        for (j=0; j<CARD_SIZE; j++) {
            printf("%c", deck[i][j]);
        }
        putchar(",");
    }*/

    free_deck(deck);
    deck = NULL;
    return 0;
}

/* Allocate space for each card */
void
malloc_deck(char **deck) {
    int i;
    for (i=0; i<DECK_NUM; i++) {
        deck[i] = malloc((CARD_SIZE+1)*sizeof(char));
        assert(deck[i]);
    }
}

/* Free space of each card and deck itself */
void
free_deck(char **deck) {
    int i;
    for (i=0; i<DECK_NUM; i++) {
        free(deck[i]);
    }
    free(deck);
}

/* Creates deck with 52 cards */
void
form_deck(char *ranks[], char *suits[], char **deck) {
    int i, j, k=0;
    char tmp_card[CARD_SIZE+1]={'\0'};
    for (i=0; i<RANK_NUM; i++) {
        for (j=0; j<SUIT_NUM; j++) {
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
    
    for (i=DECK_NUM-1; i>0; i--) {
        j = rand() % DECK_NUM;
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
    for (i=0; i<DECK_NUM; i++) {
        puts(deck[i]);
    }
}