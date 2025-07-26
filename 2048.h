#ifndef GAME_2048_H
#define GAME_2048_H

#include <stdbool.h>

#define SIZE 4

bool random_two(void);   // Places a random 2 on the board
void init(void);         // Initializes the game board
void vmerge(int dir);    // Merges vertically (up or down)
void hmerge(int dir);    // Merges horizontally (left or right)

#endif
