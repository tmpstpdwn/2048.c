/* [[ INCLUDES ]] */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "2048.h"

/* [[ GLOBAL VARS ]] */

// 2048 table.
static struct Block table[SIZE][SIZE] = {0};

// Row pointer arrays.
static struct Block *rowh[SIZE][SIZE] = {0};
static struct Block *rowv[SIZE][SIZE] = {0};

/* [[ FN DFN ]] */

// Set non-zero table pos to 2.
// return false if table is full.
bool random2(void) {
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

  if (count == 0) {
    return false;
  }

  int rand_index = rand() % count;
  int x = empty_cells[rand_index][0];
  int y = empty_cells[rand_index][1];
  table[x][y].num = 2;

  return true;
}

// Initialize table and row pointers.
void init_2048(void) {
  srand(time(NULL));

  random2();
  random2();

  for (int i = 0; i < SIZE; i++) {
    for (int j = 0; j < SIZE; j++) {
      rowh[i][j] = &table[i][j];
      rowv[j][i] = &table[i][j];
    }
  }
}

// Slide a single row.
static void rslide(struct Block *row[], const int dir) {
  int temp[SIZE] = {0};
  int temp_x[SIZE] = {0};
  int temp_y[SIZE] = {0};

  int i = (dir == 1) ? SIZE - 1 : 0;
  int j = i;

  while ((dir == 1) ? i >= 0 : i < SIZE) {
    if (row[i]->num != 0) {
      temp[j] = row[i]->num;
      temp_x[j] = row[i]->x;
      temp_y[j] = row[i]->y;
      j -= dir;
    }
    i -= dir;
  }

  for (int k = 0; k < SIZE; k++) {
    row[k]->num = temp[k];
    row[k]->x = temp_x[k];
    row[k]->y = temp_y[k];
  }
}

// Slide all vertical rows.
static void vslide(int dir) {
  for (int i = 0; i < SIZE; ++i)
    rslide(rowv[i], dir);
}

// Slide all horizontal rows.
static void hslide(int dir) {
  for (int i = 0; i < SIZE; ++i)
    rslide(rowh[i], dir);
}

// Merge a single row.
static void rmerge(struct Block *row[], const int dir) {
  int i = dir == 1 ? 0 : SIZE - 1;
  int end = dir == 1 ? SIZE - 1 : 0;
  int merged_recently = false; 

  while (i != end) {
    int next = i + dir;
    if (row[i]->num && row[i]->num == row[next]->num && !merged_recently) {
      row[next]->num = 2 * row[i]->num;
      row[next]->x = row[i]->x;
      row[next]->y = row[i]->y;
      row[i]->num = 0;
      merged_recently = true;
    } else {
      merged_recently = false;
    }
    i += dir;
  }

} 

// Merge all vertical rows.
static void vmerge(int dir) {
  for (int i = 0; i < SIZE; ++i)
    rmerge(rowv[i], dir);
}

// Merge all horizontal rows.
static void hmerge(int dir) {
  for (int i = 0; i < SIZE; ++i)
    rmerge(rowh[i], dir);
}

// Slide whole table based on dir.
void slide(enum Direction dir) {
  switch (dir) {
    case LEFT:
      hslide(-1);
      break;
    case RIGHT:
      hslide(1);
      break;
    case UP:
      vslide(-1);
      break;
    case DOWN:
      vslide(1);
      break;
  }
}

// Slide whole table based on dir.
void merge(enum Direction dir) {
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
}

// Return pointer to a block.
struct Block* getblock(int row, int col) {
  if (row < 0 || row >= SIZE || col < 0 || col >= SIZE) return NULL;
  return &table[row][col];
}

// Print the table.
void print_2048(void) {
  for (int i = 0; i < SIZE; i++) {
    for (int j = 0; j < SIZE; j++) {
      printf("%d ", (table[i][j]).num);
    }
    printf("\n");
  }
  printf("\n");
}

/* [[END]] */
