#ifndef GAME_2048_H
#define GAME_2048_H

#include <stdbool.h>

#define SIZE 4

struct Block {
  int num;
  int x;
  int y;
};

enum Direction {
  LEFT,
  RIGHT,
  UP,
  DOWN
};

bool random2(void); // Places a random 2 on the board
void init_2048(void); // Initializes the game board
void slide(enum Direction dir); // Merges horizontally (left or right)
void merge(enum Direction dir); // Merges vertically (up or down)
struct Block* getblock(int row, int col); // Return pointer to a block.
void print_2048(void); // Print the table

#endif
