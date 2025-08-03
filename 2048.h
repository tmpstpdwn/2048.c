#ifndef GAME_2048_H
#define GAME_2048_H

#include <stdbool.h>

#define SIZE 1

struct Block {
  int num;
  float x;
  float y;
  int init;
};

enum Direction {
  LEFT,
  RIGHT,
  UP,
  DOWN
};

bool isover(void); // Is the game over?
void random2(void); // Spawn 2/4 randomly on the board?
void init_2048(void); // Initializes the game board
bool merge(enum Direction dir); // Merges vertically (left, right, up, down)
struct Block* getblock(int row, int col); // Return pointer to a block.
void print_2048(void); // Print the table

#endif
