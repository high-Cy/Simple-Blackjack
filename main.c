/* A simple blackjack program ---------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include <ctype.h>

/* Defines-------------------------------------------------------------------*/

#define N_SUIT      4
#define N_RANK      13
#define N_DECK      52
#define N_CARD      2       // number of char in a card
#define N_SHUFFLE   10      // number of times to shuffle deck
#define MAX_HAND    11      // max number of cards possible in hand
#define LEN_SNAME   8       // max length of suit name (diamonds)
#define LEN_COMM    6       // max length of command (stand)
#define LEN_BOOL    4       // max length of yes or no (yes)

#define TRUE        1
#define FALSE       0

#define START_BAL   1000
#define START_HAND  2
#define START_ACE   11      // value of starting ace

#define HIT_VAL     16      // dealer to hit when HIT_VAL or lower
#define BJ          21      // value of blackjack

/* typedefs------------------------------------------------------------------*/

// struct with ranks and their corresponding value
typedef struct {
    int rank_val[N_RANK];           // value of each rank
    char *rank_str[N_RANK+1];       // name of rank
} ranks_t;

typedef struct {
    char *suit_str[N_SUIT+1];       // short-form name of suits
    char *suit_name[LEN_SNAME+1];   // name of suit
} suits_t;

typedef struct {
    int balance;
    int to_bet;
} credits_t;

typedef struct {
    char user[MAX_HAND][N_CARD+1];
    char dealer[MAX_HAND][N_CARD+1];
} hand_t;

/* Function Prototypes-------------------------------------------------------*/

void print_prompt(void);
void malloc_deck(char**);
void free_deck(char**);
void form_deck(char**, char**, char**);
void shuffle_deck(char**);
void swap_card(char*, char*);
void print_deck(char**);
void get_bet(credits_t*);
char* get_card(char**, int);
void deal_cards(char**, hand_t*, int*);
void print_hand(char[MAX_HAND][N_CARD+1], suits_t, int, int);
void print_user_hand(hand_t*, suits_t, ranks_t, int, int);
int get_hand_value(char[MAX_HAND][N_CARD+1], ranks_t, int);
int check_ace(char[MAX_HAND][N_CARD+1]);
void get_command(char*);
void card_draw(char**, int*, hand_t*, ranks_t, suits_t, int*, int*, int);
void dealer_turn(char** ,int*, hand_t*, ranks_t, int*, char*);
void user_turn(char**, int*, hand_t*, ranks_t, suits_t, int*, char*,  int);
void hit(char**, int*, char[MAX_HAND][N_CARD+1], int*);
void game_status(credits_t*, hand_t*, ranks_t, suits_t, int*, int*, int);
int get_continue(void);

int
main(int argc, char *argv[]) {
    ranks_t ranks = {
        { 1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 10, 10, 10},
        {"A","2","3","4","5","6","7","8","9","0","J","Q","K"}
    };
    suits_t suits = {
        {"D",        "C",     "H",      "S"     },
        {"Diamonds", "Clubs", "Hearts", "Spades"}
    };

    /* allocate space for deck */
    char **deck = malloc(N_DECK * sizeof(char*));
    assert(deck);
    malloc_deck(deck);

    /* combine ranks and suits */
    form_deck(ranks.rank_str, suits.suit_str, deck);
    shuffle_deck(deck);

    printf("Welcome stranger, to this Simple Blackjack Simulator!\n"
            "You'll be given %d credits to start off\n"
            "Game ends when you lose all the credits.\n\n", START_BAL);
    
    /* record amount to bet */
    credits_t credits;
    credits.balance=START_BAL;
    get_bet(&credits);
    while (credits.balance >= 0) {
        printf("You bet %d credit(s).\n", credits.to_bet);

        /* deal cards to user and dealer */
        hand_t hands;
        int iDeck=0;                // card index of deck
        int nUser, nDealer;         // number of cards
        int dealerStart;            // is dealer's starting hand?
        nUser = nDealer = START_HAND;
        deal_cards(deck, &hands, &iDeck);
        
        /* dealer's hand */
        dealerStart = TRUE;
        printf("\nDealer's hand: \n");
        print_hand(hands.dealer, suits, nUser, dealerStart);
        dealerStart = FALSE;    // set to false to print dealer's hand later

        /* user's hand */
        print_user_hand(&hands, suits, ranks, nUser, dealerStart);
        
        /* user & dealer to draw cards */
        card_draw(deck, &iDeck, &hands, ranks, suits, 
                                &nDealer, &nUser, dealerStart);

        /* provide game status */
        game_status(&credits, &hands, ranks, suits, &nUser, &nDealer, 
                        dealerStart);

        /* continue playing? */
        if (!get_continue()) {
            printf("\nThanks for playing!\n\n");
            break;
        } else if (credits.balance<=0){
            printf("Looks like you don't have enough credits.\n");
            printf("Thanks for playing\n\n");
            break;
        } else {
            printf("Your balance is: %d\n\n", credits.balance);
            get_bet(&credits);
        }
    }
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
    char tmp_card[N_CARD+1]="\0";
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

/* Prints all cards to check shuffling */
void
print_deck(char **deck) {
    int i;
    putchar('\n');
    for (i=0; i<N_DECK; i++) {
        puts(deck[i]);
    }
}

/* Gets the bet amount from user and modify balance */
void
get_bet(credits_t *credits) {
    int value, valid;
    char *input = NULL;
    size_t len=0;
    ssize_t input_size=0;

    printf("Place your bets: \n");
    print_prompt();

    do {
        input_size = getline(&input, &len, stdin);

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

char*
get_card(char **deck, int n) {
    return deck[n];
}

/* Deals starting cards to user and dealer */
void
deal_cards(char **deck, hand_t *hands, int *iDeck){
    int i;
    // shuffle n amount of times
    for (i=0; i<N_SHUFFLE; i++) {
        shuffle_deck(deck);
    }

    int iHand=0;
    for (iHand=0; iHand<2; iHand++) {
        strcpy(hands->dealer[iHand], deck[*iDeck]);
        *iDeck += 1;
        strcpy(hands->user[iHand], deck[*iDeck]);
        *iDeck += 1;
    }
}

void
print_hand(char hand[MAX_HAND][N_CARD+1], suits_t suits, int nCards, 
                int dealerStart) {

    for (int i=0; i<nCards; i++) {
        printf("    %c of ", hand[i][0]);
        for (int j=0; j<N_SUIT; j++) {
            if (hand[i][1] == suits.suit_str[j][0]) { 
                /* Note: suits->suit_str[j][0] => [0] to compare char 
                        since suit_str[j] is a string */
                printf("%s\n", suits.suit_name[j]);
            }
        }

        // don't reveal dealer's 2nd card
        if (dealerStart) {
            printf("    Hidden\n");
            break;
        }
    }

}

void
print_user_hand(hand_t *hands, suits_t suits, ranks_t ranks, 
                    int nUser, int dealerStart) {

    printf("\nYour hand: \n");
    print_hand(hands->user, suits, nUser, dealerStart);
    printf("Value: %d", get_hand_value(hands->user, ranks, nUser));
}

/* Count and return the hand's value */
int
get_hand_value(char hand[MAX_HAND][N_CARD+1], ranks_t ranks, int nCards) {
    int value=0, ace=FALSE;
    // check starting hand with ace
    if (nCards == START_HAND) {
        ace = check_ace(hand);
    }
    
    for (int i=0; i<nCards; i++) {
        for (int j=0; j<N_RANK; j++) {
            if (ace) {
                value += START_ACE;
                ace = FALSE; // prevent bust in case of double starting ace
                break;
            } else if (hand[i][0]==ranks.rank_str[j][0]) {
                value += ranks.rank_val[j];
                break;
            }
        }
    }

    return value;
}

/* Check if there's an Ace in hand */
int 
check_ace(char hand[MAX_HAND][N_CARD+1]) {
    for (int i=0; i<START_HAND; i++) {
        if (hand[i][0] == 'A') {
            return TRUE;
        }
    }
    return FALSE;
}

/* Asks user to hit or stand */
void
get_command(char* command) {
    int valid;
    char *input=NULL;
    size_t len=0;
    size_t input_size=0;

    printf("\n\nType 'hit' to take another card, \nor 'stand' to hold your "
            "cards till the end.\n");
    print_prompt();

    memset(command, 0, strlen(command));
    do {
        input_size = getline(&input, &len, stdin);

        valid = sscanf(input, "%s", command);

        if (valid!=1) {
            printf("Invalid input. Try again.\n");
            print_prompt();

        } else if (strcmp(command, "hit")!=0 && strcmp(command, "stand")!=0) {
            printf("Type hit or stand (in lower case).\n"); 
            print_prompt();
        }
        
    } while (valid!=1 || (strcmp(command, "hit")!=0 
                        && strcmp(command, "stand")!=0));

}

/* Dealer & user take turns drawing cards if needed 
   Returns TRUE if user gone bust, FALSE otherwise */
void
card_draw(char **deck, int *iDeck, hand_t *hands, ranks_t ranks, 
                suits_t suits, int *nDealer, 
                int *nUser, int dealerStart) {
    char dealer_command[LEN_COMM] = "\0";
    char user_command[LEN_COMM] = "\0";

    while (strcmp(user_command, "stand")!=0 
                || strcmp(dealer_command, "stand")!=0) {

        if (strcmp(dealer_command, "stand")!=0) {
            dealer_turn(deck, iDeck, hands, ranks, nDealer, dealer_command);

        }
        if (strcmp(user_command, "stand")!=0) {
            get_command(user_command);

            user_turn(deck, iDeck, hands, ranks, suits, nUser, user_command, 
                            dealerStart);
            print_user_hand(hands, suits, ranks, *nUser, dealerStart);

            if (get_hand_value(hands->user, ranks, *nUser) > BJ) {
                break;
            }
        }
    }
}

/* Determine to draw a card or not */
void 
dealer_turn(char **deck, int *iDeck, hand_t *hands, ranks_t ranks, 
                int *nDealer, char *dealer_command) {

    // set dealer to draw card when hand is lower than wanted value
    int value = get_hand_value(hands->dealer, ranks, *nDealer);
    if (value <= HIT_VAL) {
        hit(deck, iDeck, hands->dealer, nDealer);
        strcpy(dealer_command, "hit\0");
    } else {
        strcpy(dealer_command, "stand\0");
    }
}

/* User to determine to draw card or not */
void
user_turn(char **deck, int *iDeck, hand_t *hands, ranks_t ranks, 
                suits_t suits, int *nUser, char* user_command, 
                int dealerStart) {

    //printf("%d", strcmp(user_command, "hit"));
    if (strcmp(user_command, "hit")==0) {
        hit(deck, iDeck, hands->user, nUser);
    } 
}

/* Add a card from the deck to hand */
void
hit(char** deck, int *iDeck, char hand[MAX_HAND][N_CARD+1], int *nCards) {
    strcpy(hand[*nCards], deck[*iDeck]);
    *nCards += 1;
    *iDeck += 1;
}

/* Displays outcome */
void
game_status(credits_t *credits, hand_t *hands, ranks_t ranks, suits_t suits, 
                int *nUser, int *nDealer, int dealerStart) {
    int user_val = get_hand_value(hands->user, ranks, *nUser);
    int dealer_val = get_hand_value(hands->dealer, ranks, *nDealer);

    if (user_val > BJ) {
        printf("    BUST!\n");
    }

    printf("\n\nDealer's hand: \n");
    print_hand(hands->dealer, suits, *nDealer, dealerStart);
    printf("Value: %d\n\n", dealer_val);

    if (user_val > BJ || (user_val < dealer_val && dealer_val < BJ)) {
        printf("You lost. :(\n");
    } else if (user_val == dealer_val) {
        printf("Draw.\n");
        credits->balance += credits->to_bet;
    } else {
        printf("YOU WON! :)\n");
        credits->balance += 2*credits->to_bet;

    }
}

/* Asks user if they want to continue playing */
int
get_continue(void) {
    int cont, valid;
    char *input = NULL;
    size_t len=0;
    ssize_t input_size=0;

    printf("Do you want to continue? Type 1 for yes, 0 for no: \n");
    print_prompt();

    do {
        input_size = getline(&input, &len, stdin);

        valid = sscanf(input, "%d", &cont);

        if (valid!=1) {
            printf("Input was not a number. Try again.\n");
            print_prompt();

        } else if (cont !=0 && cont!=1){
            printf("Type 1 for yes, 0 for no\n"); 
            print_prompt();
        } 

    } while (valid!=1 || (cont !=0 && cont!=1));

    return cont;
}
