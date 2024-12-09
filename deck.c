#include "deck.h"
#include <string.h>

/*
 * Variable: deck_instance
 * -----------------------
 *  
 * Go Fish uses a single deck
 */
struct deck deck_instance;

// Shuffles the deck 
int shuffle(){
    int i, j;

    // Checks if the end of the deck has been reached
    if (deck_instance.top_card == 0){

        // Initiliaze the array for the deck of cards
        char suits[4] = {'S','C','H','D'};
        char ranks[13][3] = {"A","2","3","4","5","6","7","8","9","10","J","Q","K"};

        for (i=0; i<4; i++){ 
            for (j=0; j<13; j++){
                struct card new_card; // Creates the new card
                new_card.suit = suits[i]; // Assigns a new suit
                 strcpy(new_card.rank, ranks[j]); // Assigns a new rank

                // Add the card to the deck
                deck_instance.list[deck_instance.top_card++] = new_card;
            }
        }
    } 
    
    for (i=0; i<deck_instance.top_card; i++){
        j = rand() % deck_instance.top_card;

        // Swaps cards to shuffle the deck
        struct card temp = deck_instance.list[i];
        deck_instance.list[i] = deck_instance.list[j];
        deck_instance.list[j] = temp; 
    }
    // Resets the top of the deck
    deck_instance.top_card = 52;
    
    return 0;
}

// Deals the cards 
int deal_player_cards(struct player* target){
    struct card* new_card; 
    
    for (int i = 0; i < 7; i++) { 
        // Check if there are cards left in the deck
        if (deck_size() <= 0) {
            return -1;  // No cards left, return error
        }
        new_card = next_card();  // Get the next card from the deck
        if (new_card == NULL) {
            return -1;  // If no card is drawn, return error
        }
        add_card(target, new_card);  // Add the drawn card to the player's hand
    }

    return 0; 
}

struct card* next_card() {
    if (deck_size() <= 0) {
        return NULL;  // Return NULL if deck is empty
    }
    deck_instance.top_card--;  // Reduce the count of cards in the deck
    return &deck_instance.list[deck_instance.top_card];
}

size_t deck_size() {
    // Check if cards are left in the deck
    if (deck_instance.top_card > 0) {
        return deck_instance.top_card;  // Return the number of cards remaining
    } else {
        return 0;
    }
}



    


    

