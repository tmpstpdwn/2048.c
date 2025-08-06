/* [[ INCLUDES ]] */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "2048.h"

/* [[ GLOBAL VARS ]] */

// 2048 table.
static struct Block table[SIZE][SIZE] = {0};

// Row pointer arrays.
static struct Block *rowh[SIZE][SIZE] = {0}; // Horizontal.
static struct Block *rowv[SIZE][SIZE] = {0}; // Vertical.

// Should a new block be added?
static bool new_block = false;

/* [[ FN DFN ]] */

// Reset table.
void reset_2048(void) {
  for (int i = 0; i < SIZE; i++) {
    for (int j = 0; j < SIZE; j++) {
      memset(&table[i][j], 0, sizeof(struct Block));
    }
  }
  random24();
  random24();
}

// Set random non-zero table cell to 2 or 4.
void random24(void) {
  int empty_cells[SIZE * SIZE][2];
  int count = 0;

  for (int i = 0; i < SIZE; i++) {
    for (int j = 0; j < SIZE; j++) {
      if (table[i][j].num == 0) {
        empty_cells[count][0] = i;
        empty_cells[count][1] = j;
        count++;
      }
    }
  }

  if (count == 0) return;

  int rand_index = rand() % count;
  int x = empty_cells[rand_index][0];
  int y = empty_cells[rand_index][1];
  table[x][y].num = (rand() % 10 == 0) ? 4 : 2;
}

// Initialize random seed, row pointers.
void init_2048(void) {
  srand(time(NULL));

  for (int i = 0; i < SIZE; i++) {
    for (int j = 0; j < SIZE; j++) {
      rowh[i][j] = &table[i][j];
      rowv[j][i] = &table[i][j];
    }
  }

  random24();
  random24();
}

// Merge a row in the given direction (1: right, -1: left).
static void rmerge(struct Block *arr[SIZE], int dir) {
  int step = (dir == 1) ? -1 : 1;
  int write = (dir == 1) ? SIZE - 1 : 0;
  int read = write + step;
  int end = (dir == 1) ? -1 : SIZE;

  while (read != end) {
    if (!arr[read]->num) {
      read += step;
      continue;
    }
      
    if (arr[write]->num == arr[read]->num) {
      arr[write]->merged_num = arr[write]->num;
      arr[write]->merged_x = arr[write]->x;
      arr[write]->merged_y = arr[write]->y;
      arr[write]->merged = true;
      arr[write]->merged_alpha = 1;

      arr[write]->num *= 2;
      arr[write]->x = arr[read]->x;
      arr[write]->y = arr[read]->y;
      arr[write]->init = arr[read]->init;
      
      arr[read]->init = false;
      arr[read]->num = 0;

      write += step, read += step;
      new_block = true;

    } else if (!arr[write]->num) {
      arr[write]->num = arr[read]->num;
      arr[write]->x = arr[read]->x;
      arr[write]->y = arr[read]->y;
      arr[write]->init = arr[read]->init;

      arr[read]->init = false;
      arr[read]->num = 0;

      read += step;
      new_block = true;

    } else if (!arr[write+step]->num) {
      write += step;

    } else {
      write += step, read += step;

    }
  }
}

// Merge all vertical rows in the given direction (1: right, -1: left).
static void vmerge(int dir) {
  for (int i = 0; i < SIZE; ++i)
    rmerge(rowv[i], dir);
}

// Merge all horizontal rows in the given direction (1: right, -1: left).
static void hmerge(int dir) {
  for (int i = 0; i < SIZE; ++i)
    rmerge(rowh[i], dir);
}

// Merge the whole table based on dir (LEFT, RIGHT, UP, DOWN).
// Returns whether a new block should be added or not.
bool merge(enum Direction dir) {
  new_block = false;
  switch (dir) {
    case LEFT:
      hmerge(-1);
      break;
    case RIGHT:
      hmerge(1);
      break;
    case UP:
      vmerge(-1);
      break;
    case DOWN:
      vmerge(1);
      break;
  }
  return new_block;
}

// Return pointer to a block.
struct Block* getblock(int row, int col) {
  if (row < 0 || row >= SIZE || col < 0 || col >= SIZE) return NULL;
  return &table[row][col];
}

/* [[END]] */
