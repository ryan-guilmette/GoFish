#include <string.h>
#include "player.h"

/*
 * Instance Variables: user, computer   
 * ----------------------------------
 *  
 *  We only support 2 users: a human and a computer
 */
struct player user;
struct player computer;

int add_card(struct player* target, struct card* new_card) {
    // malloc for memory
    struct hand* new_node = (struct hand*)malloc(sizeof(struct hand));
    
    // Check if malloc was successful
    if (new_node == NULL) {
        return -1;  // Error: memory allocation failed
    }

    // Initialize the new node with the card
    new_node->top = *new_card;
    new_node->next = NULL;

    // If the hand is empty, set this as the first card
    if (target->hand_size == 0) {
        target->card_list = new_node;
    } else {
        // Otherwise, traverse to the last node and append the new card
        struct hand* current = target->card_list;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = new_node;
    }

    // Add to players hand size
    target->hand_size++;

    // Check for book
    check_add_book(target);

    return 0;
}

int remove_card(struct player* target, struct card* old_card) {
    // If our hand size is empty, return -1 
    if (target->hand_size <= 0)
        return -1;

    struct hand* current = target->card_list;
    struct hand* before = NULL;

    // Find card to remove
    while (current != NULL) {
        if (current->top.suit == old_card->suit && current->top.rank[0] == old_card->rank[0]) {
            // Card found
            break;
        }
        before = current;
        current = current->next;
    }

    // Card not found
    if (current == NULL) {
        return -1;
    }

    // Card ss invalid
    current->top.suit = 0;   // Invalidate suit
    current->top.rank[0] = 0; // Invalidate rank

    // Removing card from the beginning of the list
    if (before == NULL) {
        target->card_list = current->next; // Update top of the list
    } else {
        // Removing card from the middle or end of the list
        before->next = current->next;
    }

    target->hand_size--; // Decrease hand size

    return 0; // Success
}

char check_add_book(struct player* target) {
    if (target->card_list == NULL) {
        return 0; // No cards to check for a book
    }

    // Sets the targets for the ranks
    struct hand* current = target->card_list;
    char ranks[13] = {'A', '2', '3', '4', '5', '6', '7', '8', '9', '1', 'J', 'Q', 'K'};
    
    // Loops through the 13 ranks until target is reached
    for (int i = 0; i < 13; i++) {
        int count = 0;
        current = target->card_list;
        
        // Includes 10 which is two integers
        while (current != NULL) {
            if (current->top.rank[0] == ranks[i] || 
                (ranks[i] == '1' && strcmp(current->top.rank, "10") == 0)) {
                count++;
            }
            current = current->next;
        }

        // Loops to find 4 of the same rank, which makes it a book
        if (count == 4) {
            // Initiates a book
            for (int j = 0; j < 7; j++) {
                if (target->book[j] == 0) {
                    target->book[j] = ranks[i];
                    if (j == 6){
                        return 1; // Found a book
                    }
                    break;
                }
            }

            // Remove all cards of this rank
            current = target->card_list;
            while (current != NULL) {
                if (current->top.rank[0] == ranks[i] || 
                    (ranks[i] == '1' && strcmp(current->top.rank, "10") == 0)) {
                    remove_card(target, &current->top);
                    current = target->card_list; // Reset to start of list
                } else {
                    current = current->next;
                }
            }

            return ranks[i];
        }
    }

    return 0; // No book found
}

int search(struct player* target, char rank) {
    struct hand* current_hand = target->card_list; // Start with the first card

    // Traverse the linked list of cards
    while (current_hand != NULL) {
        // Compare the card's rank with the searched rank
        if (current_hand->top.rank[0] == rank) {
            return 1; // Rank found
        }
        current_hand = current_hand->next; // Move to the next card
    }

    return 0; // Rank not found
}

int transfer_cards(struct player* src, struct player* dest, char rank) {
    int transferred = 0;
    struct hand* current = src->card_list;
    struct hand* prev = NULL;

    while (current != NULL) {
        if ((rank == '1' && strcmp(current->top.rank, "10") == 0) || 
            (rank != '1' && current->top.rank[0] == rank)) {
            // Card found, transfer it
            if (prev == NULL) {
                src->card_list = current->next;
            } else {
                prev->next = current->next;
            }
            struct hand* temp = current;
            current = current->next;
            
            // Add to destination
            temp->next = dest->card_list;
            dest->card_list = temp;
            
            src->hand_size--;
            dest->hand_size++;
            transferred++;
        } else {
            prev = current;
            current = current->next;
        }
    }
    return transferred;
}

int game_over(struct player* target) {
    int book_count = 0;
    for (int i = 0; i < 7; i++) {
        if (target->book[i] != 0) {
            book_count++;
        }
    }
    // ends the game if a players reaches 7 books
    return book_count == 7;
}

int reset_player(struct player* target) {
    // Remove all cards from the player's hand
    while (target->card_list != NULL) {
        remove_card(target, &target->card_list->top);
    }

    // Check if the player's hand is empty
    if (target->hand_size != 0 || target->card_list != NULL) {
        return -1;  // Error: hand is not empty
    }

    // Reset book elements to 0
    int i;
    for (i = 0; i < 7; i++) {
        target->book[i] = 0;  // Set each book slot to 0
    }

    // Check if the book size is correct
    if (i != 7) {
        return -1;  // Error: book size incorrect
    }

    return 0;  // Success: player reset
}

char computer_play(struct player* target) {
    int randIndex = rand() % target->hand_size;
    struct hand* current_hand = target->card_list;

    for(int i = 0; i < randIndex; i++) {
        current_hand = current_hand->next;
    }

    return current_hand->top.rank[0];
}

char user_play(struct player* target) {
    char input[3];
    char rank;
    while (1) {
        fprintf(stdout, "Player 1's turn, enter a Rank: ");
        scanf("%2s", input);
        while (getchar() != '\n');  // Clear input buffer
        
        // Convert input to uppercase
        for (int i = 0; input[i]; i++) {
            if (input[i] >= 'a' && input[i] <= 'z') {
                input[i] = input[i] - 'a' + 'A';
            }
        }
        
        // Handle '10' as a special case
        if (strcmp(input, "10") == 0) {
            rank = '1';  // Use '1' to represent 10
        } else if (strlen(input) == 1) {
            rank = input[0];
        } else {
            fprintf(stdout, "Error - invalid rank\n");
            continue;
        }
        
        // Check if the input is valid
        if ((rank >= '2' && rank <= '9') || rank == '1' || rank == 'J' || rank == 'Q' || rank == 'K' || rank == 'A') {
            if (search(target, rank)) {
                return rank;
            } else {
                fprintf(stdout, "Error - must have at least one card from rank to play\n");
            }
        } else {
            fprintf(stdout, "Error - invalid rank\n");
        }
    }
}








