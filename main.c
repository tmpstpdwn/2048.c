#include <stdio.h>
#include <raylib.h>
#include "2048.h"

#define CELL_SIZE 80
#define CELL_BLOCK_PERC 88
#define FONT_SIZE 50

enum GameState {
  INPUT,
  MERGE,
  ANIMATION,
  OVER,
};

static const int W_SIZE = SIZE * CELL_SIZE; 
static const float BLOCK_SIZE = (CELL_BLOCK_PERC * CELL_SIZE) / 100;
static const float PADDING = ((CELL_SIZE - BLOCK_SIZE) * SIZE) / (SIZE + 1);
static const float ANIM_SPEED = W_SIZE * 4;

static enum GameState gamestate = INPUT;
static enum Direction dir = LEFT;
static bool is_animating = false;
static bool new_block = false;

static Color get_tile_color(int value) {
  switch (value) {
    case 0: return (Color){238, 238, 238, 255};
    case 2: return (Color){238, 228, 218, 255};
    case 4: return (Color){237, 224, 200, 255};
    case 8: return (Color){242, 177, 121, 255};
    case 16: return (Color){245, 149, 99, 255};
    case 32: return (Color){246, 124, 95, 255};
    case 64: return (Color){246, 94, 59, 255};
    case 128: return (Color){237, 207, 114, 255};
    case 256: return (Color){237, 204, 97, 255};
    case 512: return (Color){237, 200, 80, 255};
    case 1024: return (Color){237, 197, 63, 255};
    case 2048: return (Color){237, 194, 46, 255};
    default: return (Color){60, 58, 50, 255}; // For >2048
  }
}

static bool takeinput(void) {
  int new_dir = -1;

  if (IsKeyPressed(KEY_LEFT))  new_dir = LEFT;
  else if (IsKeyPressed(KEY_RIGHT)) new_dir = RIGHT;
  else if (IsKeyPressed(KEY_UP))    new_dir = UP;
  else if (IsKeyPressed(KEY_DOWN))  new_dir = DOWN;

  if (new_dir != -1) {
    dir = new_dir;
    return true;
  }

  return false;
}

static void animate(struct Block *b, float target_x, float target_y, float dt) {
  if (b->x < target_x) {
    b->x += ANIM_SPEED * dt;
    if (b->x > target_x) b->x = target_x;
  } else if (b->x > target_x) {
    b->x -= ANIM_SPEED * dt;
    if (b->x < target_x) b->x = target_x;
  }

  if (b->y < target_y) {
    b->y += ANIM_SPEED * dt;
    if (b->y > target_y) b->y = target_y;
  } else if (b->y > target_y) {
    b->y -= ANIM_SPEED * dt;
    if (b->y < target_y) b->y = target_y;
  }
}

static void rendernum(int num, float x, float y) {
  const char* num_str = TextFormat("%d", num);
  int text_height = FONT_SIZE;
  int digits = (num < 10) ? 1 : (num < 100) ? 2 : (num < 1000) ? 3 : 4;
  int font_size = 50 - (digits - 1) * 5;
  int text_width = MeasureText(num_str, font_size);
  int text_x = x + (BLOCK_SIZE - text_width) / 2;
  int text_y = y + (BLOCK_SIZE - text_height) / 2;
  Color text_color = (num <= 4) ? (Color){119, 110, 101, 255} : RAYWHITE;
  DrawText(num_str, text_x, text_y, font_size, text_color);
}

static void draw_tiles(float dt) {
  is_animating = false;

  for (int i = 0; i < SIZE; i++) {
    for (int j = 0; j < SIZE; j++) {

      struct Block *b = getblock(i, j);

      if (!b->num) {
        continue;
      }

      if (!b->init) {
        b->x = j * BLOCK_SIZE + PADDING * (j + 1);
        b->y = i * BLOCK_SIZE + PADDING * (i + 1);
        b->init = 1;
      }

      float target_x = j * BLOCK_SIZE + PADDING * (j + 1);
      float target_y = i * BLOCK_SIZE + PADDING * (i + 1);

      animate(b, target_x, target_y, dt);

      if (b->x != target_x || b->y != target_y) {
        is_animating = true;
      }

      DrawRectangleRounded((Rectangle){b->x, b->y, BLOCK_SIZE, BLOCK_SIZE}, 0.2f, 12, get_tile_color(b->num));
      rendernum(b->num, b->x, b->y);
    }
  }

}

static void draw_empty_tiles(void) {
  for (int i = 0; i < SIZE; i++) {
    for (int j = 0; j < SIZE; j++) {
      float x = j * BLOCK_SIZE + PADDING * (j + 1);
      float y = i * BLOCK_SIZE + PADDING * (i + 1);
      DrawRectangleRounded((Rectangle){x, y, BLOCK_SIZE, BLOCK_SIZE}, 0.2f, 12, get_tile_color(0));
    }
  }
}
 
static void render(float dt) {
  draw_empty_tiles();
  draw_tiles(dt);
}

int main(void) {
  InitWindow(W_SIZE, W_SIZE, "2048");

  init_2048();

  while (!WindowShouldClose()) {
    float dt = GetFrameTime(); 

    if (gamestate == INPUT) {
      if (takeinput()) {
        gamestate = MERGE;
      }

    } else if (gamestate == MERGE) {
      new_block = merge(dir);
      gamestate = ANIMATION;

    } else if (gamestate == ANIMATION) {
      if (!is_animating) {
        gamestate = INPUT;
        if (new_block) {
          random2();
        } else if (isover()) {
          gamestate = OVER;
        }
      }

    } else {
      printf("GAME OVER\n");
      WaitTime(2);
      return 0;

    }
    
    BeginDrawing();

    ClearBackground(RAYWHITE);

    render(dt);

    EndDrawing();
  }

  CloseWindow();
  return 0;
}
