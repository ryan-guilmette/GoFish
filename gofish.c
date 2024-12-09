#include <stdio.h>
#include <time.h>
#include <string.h>
#include "gofish.h"

int main(int args, char* argv[]) {
    srand(time(NULL));

    do {
      // starts ther round
      game_start();
      do {
          // Plays a round
          if(game_loop())
              break; // If there is a winner it jumps to game_end
          current = next_player;
        } while(1);
    // exits the program if finished
    } while(game_end());
      fprintf(stdout, "Exiting\n");

    return 0;
}

void game_start() {
    reset_player(&user);
    reset_player(&computer);

    // prompts to the user that the deck is shuffling
    fprintf(stdout, "Shuffling deck...\n");
    shuffle();
    deal_player_cards(&user);
    deal_player_cards(&computer);

    // Makes sure that only one book can be dealt
    if(user.book[0] != 0)
        fprintf(stdout, "Player 1 books %s", pR(user.book[0]));
    if(computer.book[0] != 0)
        fprintf(stdout, "Player 2 books %s", pR(computer.book[0]));

    current = &user;
    next_player = &computer;
}

int game_loop() {
    while (1) {
        // Print the game state
        fprintf(stdout, "\nPlayer 1's Hand - ");
        print_hand(&user);
        fprintf(stdout, "Player 1's Book - ");
        print_book(&user);
        fprintf(stdout, "Player 2's Book - ");
        print_book(&computer);

        // Get the current player's input
        struct player* current_player = current;
        struct player* other_player = (current_player == &user) ? &computer : &user;

        // Check if the current player's hand is empty and draw a card if necessary
        if (current_player->hand_size == 0) {
            struct card* new_card = next_card();
            if (new_card != NULL) {
                add_card(current_player, new_card);
                fprintf(stdout, "%s's hand was empty. ", (current_player == &user) ? "Player 1" : "Player 2");
                if (current_player == &user) {
                    fprintf(stdout, "Player 1 draws %s%c\n", new_card->rank, new_card->suit);
                } else {
                    fprintf(stdout, "Player 2 draws a card\n");
                }
            } else {
                fprintf(stdout, "The deck is empty. %s cannot draw a card.\n", (current_player == &user) ? "Player 1" : "Player 2");
                int count = 0;
                while (current_player->book[count] != 0 && count < 7) {
                    count++;
                }
                if (count == 7) {
                    return 1;
                }
                count = 0;
                while (other_player->book[count] != 0 && count < 7) {
                    count++;
                }
                if (count == 7) {
                    return 1;
                }
                break;  // End the game if no cards can be drawn
            }
            if (game_over(current)) {
                return 1;
            }
        }

        char rank;
        if (current_player == &user) {
            rank = user_play(current_player);
        } else {
            rank = computer_play(current_player);
            fprintf(stdout, "Player 2's turn, enter a Rank: %s\n", pR(rank));
        }

        // Check if the other player has the card
        int cards_transferred = transfer_cards(other_player, current_player, rank);
        if (cards_transferred > 0) {
            fprintf(stdout, "    - ");
            if (current_player == &user) {
                fprintf(stdout, "Player 2 has ");
            } else {
                fprintf(stdout, "Player 1 has ");
            }
            
            struct hand* temp = current_player->card_list;
            int first = 1;
            while (temp != NULL && cards_transferred > 0) {
                if ((rank == '1' && strcmp(temp->top.rank, "10") == 0) || 
                    (rank != '1' && temp->top.rank[0] == rank)) {
                    if (!first) {
                        fprintf(stdout, ", ");
                    }
                    fprintf(stdout, "%s%c", temp->top.rank, temp->top.suit);
                    first = 0;
                    cards_transferred--;
                }
                temp = temp->next;
            }
            fprintf(stdout, "\n");

            char book_rank;
            while ((book_rank = check_add_book(current_player)) != 0) {
                fprintf(stdout, "    - %s books %s\n", (current_player == &user) ? "Player 1" : "Player 2", pR(book_rank));
            }
            fprintf(stdout, "    - %s gets another turn\n", (current_player == &user) ? "Player 1" : "Player 2");
            continue;  // Skip switching players
        } else {
            fprintf(stdout, "    - %s has no %s's\n", (current_player == &user) ? "Player 2" : "Player 1", pR(rank));
            fprintf(stdout, "    - Go Fish, ");
            
            // Draw a card from the deck
            struct card* new_card = next_card();
            if (new_card != NULL) {
                add_card(current_player, new_card);
                if (current_player == &user) {
                    fprintf(stdout, "Player 1 draws %s%c\n", new_card->rank , new_card->suit);
                } else {
                    fprintf(stdout, "Player 2 draws a card\n");
                }
            } else {
                fprintf(stdout, "The deck is empty. %s cannot draw a card.\n", (current_player == &user) ? "Player 1" : "Player 2");
                int count = 0;
                while (current_player->book [count] != 0 && count < 7) {
                    count++;
                }
                if (count == 7) {
                    return 1;
                }
                count = 0;
                while (other_player->book[count] != 0 && count < 7) {
                    count++;
                }
                if (count == 7) {
                    return 1;
                }
                break;  // End the game if no cards can be drawn
            }
        }

        // Switch players
        current = (current == &user) ? &computer : &user;
    }
    return 0;
}

int game_end() {
    struct player* other_player = (current == &user) ? &computer : &user;
    
    // Count books of loser 
    int count = 0;
    while(other_player->book[count] != 0 && count < 6) { 
        count++;
    }
    // User had 7 books (user wins)
    if(current == &user) {
        fprintf(stdout, "Player 1 Wins! 7-%d\n", count);
    } else {
    // Computer has 7 books (computer wins)
        fprintf(stdout, "Player 2 Wins! 7-%d\n", count);
    }

    char yn[3] = "";
    int playAgain = 0;
    do {
        if(playAgain) {
            // error message
            fprintf(stdout, "Error - must enter \"Y\" or \"N\"");
        }
        // asks user if they wan
        fprintf(stdout, "\nDo you want to play again [Y/N]: ");
        scanf("%2s", yn);
        playAgain = 1;

        while(getchar() != '\n');

        if(yn[1] != '\0') {
            continue;
        }

        // Takes user inpiut for yes or no
        if(yn[0] == 'Y') {
            return 1;
        } else if(yn[0] == 'N') {
            return 0;
        } else {
            fprintf(stdout, "Invalid input. Please enter 'Y' or 'N'.\n");
        }
    } while(1);
}

const char* pR(char r){
    if(r == '1') {
        static char ten[] = "10";
        return ten;
    }
    static char rS[2];
    rS[0] = r;
    rS[1] = '\0';
    return rS;
}

// Prints hand of the user
void print_hand(struct player* target) {
    if(target->hand_size == 0) {
        fprintf(stdout, "\n");
        return;
    }

    struct hand* h = target->card_list;
    fprintf(stdout, "%s%c", pR(h->top.rank[0]), h->top.suit);

    int i;
    for(i = 1; i < target->hand_size; i++) {
        h = h->next;
        fprintf(stdout, " %s%c", pR(h->top.rank[0]), h->top.suit);
    }

    fprintf(stdout, "\n");
}

// Prints the books when completed 
void print_book(struct player* target) {
    if(target == NULL || target->book == NULL || target->book[0] == '\0' || target->book[0] == 0) {
        fprintf(stdout, "\n");
        return;
    }

    fprintf(stdout, "%s", pR(target->book[0]));

    int i = 1;
    while(i < 7 && target->book[i] != '\0' && target->book[i] != 0) {
        fprintf(stdout, " %s", pR(target->book[i++]));
    }

    fprintf(stdout, "\n");
}
