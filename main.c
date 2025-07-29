#include <stdio.h>
#include "raylib.h"
#include "2048.h"

#define W_SIZE 400
#define BLOCK_SIZE 95
#define PADDING 4

enum GameState {
  INPUT,
  MOVEMENT,
};

enum Movement {
  SLIDE1,
  MERGE,
  SLIDE2,
};

enum Direction dir = LEFT;
enum GameState gamestate = INPUT;
enum Movement movement = SLIDE1;

void takeinput(void) {
  if (IsKeyPressed(KEY_LEFT)) {
    dir = LEFT;
    gamestate = MOVEMENT;
  } else if (IsKeyPressed(KEY_RIGHT)) {
    dir = RIGHT;
    gamestate = MOVEMENT;
  } else if (IsKeyPressed(KEY_UP)) {
    dir = UP;
    gamestate = MOVEMENT;
  } else if (IsKeyPressed(KEY_DOWN)) {
    dir = DOWN;
    gamestate = MOVEMENT;
  }
}

void move(void) {
  if (movement == SLIDE1) {
    slide(dir);
    movement = MERGE;
  } else if (movement == MERGE) {
    merge(dir);
    movement = SLIDE2;
  } else if (movement == SLIDE2) {
    slide(dir);
    movement = SLIDE1;
    gamestate = INPUT;
    random2();
  }
}
 
int main(void) {
  const int screenWidth = W_SIZE;
  const int screenHeight = W_SIZE;

  InitWindow(screenWidth, screenHeight, "2048");
  SetTargetFPS(60);

  init_2048();
  char num_str[12];
  int font_size = 50;

  while (!WindowShouldClose()) {

    if (gamestate == INPUT) {
      takeinput();
    } else if (gamestate == MOVEMENT) {
      move();
    }
    
    BeginDrawing();
    ClearBackground(RAYWHITE);

    for (int i = 0; i < SIZE; i++) {
      for (int j = 0; j < SIZE; j++) {
        struct Block *temp = getblock(i, j);
        if (!temp->num)
          continue;
        int x = j * BLOCK_SIZE + PADDING * (j + 1);
        int y = i * BLOCK_SIZE + PADDING * (i + 1); 
        DrawRectangle(x, y, BLOCK_SIZE, BLOCK_SIZE, BLUE);
        sprintf(num_str, "%d", temp->num);
        int text_width = MeasureText(num_str, font_size);
        int text_height = font_size;
        int text_x = x + (BLOCK_SIZE - text_width) / 2;
        int text_y = y + (BLOCK_SIZE - text_height) / 2;
        DrawText(num_str, text_x, text_y, font_size, BLACK);
      }
    }

    EndDrawing();
  }

  CloseWindow();
  return 0;
}
