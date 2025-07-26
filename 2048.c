/* [[ INCLUDES ]] */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "2048.h"

/* [[ GLOBAL VARS ]] */

// 2048 table.
static int table[SIZE][SIZE] = {0};

// Row pointer arrays.
static int *rowh[SIZE][SIZE], *rowv[SIZE][SIZE];

/* [[ FN DFN ]] */

// Set non-zero table pos to 2.
// return false if table is full.
bool random_two(void) {
  int empty_cells[SIZE * SIZE][2];
  int count = 0;

  for (int i = 0; i < SIZE; i++) {
    for (int j = 0; j < SIZE; j++) {
      if (table[i][j] == 0) {
        empty_cells[count][0] = i;
        empty_cells[count][1] = j;
        count++;
      }
    }
  }

  if (count == 0) {
    return false;
  }

  int rand_index = rand() % count;
  int x = empty_cells[rand_index][0];
  int y = empty_cells[rand_index][1];
  table[x][y] = 2;

  return true;
}

// Initialize table and row pointers.
void init() {
  srand(time(NULL));

  random_two();
  random_two();

  for (int i = 0; i < SIZE; ++i) {
    for (int j = 0; j < SIZE; ++j) {
      rowh[i][j] = &table[i][j];
      rowv[j][i] = &table[i][j];
    }
  }
}

// Slide a single row.
static void slide(int *row[], const int dir) {
  int temp[SIZE] = {0};
  int i = (dir == 1) ? SIZE - 1 : 0;
  int j = i;

  while ((dir == 1) ? i >= 0 : i < SIZE) {
    if (*row[i] != 0) {
      temp[j] = *row[i];
      j -= dir;
    }
    i -= dir;
  }

  for (int k = 0; k < SIZE; k++) {
    *row[k] = temp[k];
  }
}

// Slide and merge a single row.
static void rmerge(int *row[], const int dir) {
  slide(row, dir);

  int i = dir == 1 ? 0 : SIZE - 1;
  int end = dir == 1 ? SIZE - 1 : 0;
  int just_done = 0; 

  while (i != end) {
    if (*row[i] && *row[i] == *row[i+dir] && !just_done) {
      *row[i+dir] = 2 * *row[i];
      *row[i] = 0;
      just_done = 1;
    } else {
      just_done = 0;
    }
    i += dir;
  }

  slide(row, dir);
} 

// Slide and merge all vertical rows.
void vmerge(int dir) {
  for (int i = 0; i < SIZE; ++i)
    rmerge(rowv[i], dir);
}

// Slide and merge all horizontal rows.
void hmerge(int dir) {
  for (int i = 0; i < SIZE; ++i)
    rmerge(rowh[i], dir);
}

/* [[END]] */
