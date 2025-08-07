#ifndef GAME_2048_H
#define GAME_2048_H

#include <stdbool.h>

// Grid size (4x4).
#define SIZE 4

// 2048 block struct.
struct Block {
  bool init; // Is the block pos initialized?
  int num; // Block number (2, 4, 8 ...).
  float x, y, alpha; // pos, color alpha of block on screen.
  bool merged; // Did this block merge with another?
  int merged_num; // Block number of merged block.
  float merged_x, merged_y ,merged_alpha; // pos, color alpha of the merged block on screen.
};

// Direction for merging.
enum Direction {
  LEFT,
  RIGHT,
  UP,
  DOWN
};

void init_2048(void); // Initializes the game board.
void spawn_tile(void); // Spawns 2 or 4 randomly on the board.
bool merge(enum Direction dir); // Merges vertically (left, right, up, down).
void reset_2048(void); // Reset the game board.
struct Block* get_block(int row, int col); // Return pointer to a block.

#endif
