#include <stdio.h>
#include <raylib.h>
#include <stdlib.h>
#include "2048.h"

#define CELL_SIZE 100
#define CELL_BLOCK_PERC 90
#define ROUNDED_CORNERS 0.2f
#define FPS 60
#define FADE_SPEED 2.0f

enum GameState {
  INPUT,
  MERGE,
  ANIMATION,
  OVER,
};

static const int W_SIZE = SIZE * CELL_SIZE; 
static const float BLOCK_SIZE = (CELL_BLOCK_PERC * CELL_SIZE) / 100;
static const float PADDING = ((CELL_SIZE - BLOCK_SIZE) * SIZE) / (SIZE + 1);
static const float SLIDE_SPEED = W_SIZE * 4;

static const Color BG_COL = {187, 173, 160, 255};
static const Color EMPTY_TILE_COL = {204, 192, 180, 255};

static const char *TITLE = "2048";

static const char *FONT = "font.ttf";
static Font game_font;

static enum GameState gamestate = INPUT;
static enum Direction dir;
static bool is_animating;
static bool new_block;

static inline Color get_tile_color(int num) {
  switch (num) {
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
    default: return (Color){60, 58, 50, 255};
  }
}

static inline Color get_text_color(int num) {
  return (num <= 4)? (Color){119, 110, 101, 255} : RAYWHITE;
}

static enum GameState get_input(void) {

  if (IsKeyPressed(KEY_LEFT)) {
    dir = LEFT;
    return MERGE;
    
  } else if (IsKeyPressed(KEY_RIGHT)) {
    dir = RIGHT;
    return MERGE;
    
  } else if (IsKeyPressed(KEY_UP)) {
    dir = UP;
    return MERGE;
    
  } else if (IsKeyPressed(KEY_DOWN)) {
    dir = DOWN;
    return MERGE;
    
  }

  return INPUT;
}

static void draw_empty_tiles(void) {
  for (int i = 0; i < SIZE; i++) {
    for (int j = 0; j < SIZE; j++) {
      float x = j * BLOCK_SIZE + PADDING * (j + 1);
      float y = i * BLOCK_SIZE + PADDING * (i + 1);
      Rectangle rect = {x, y, BLOCK_SIZE, BLOCK_SIZE};
      DrawRectangleRounded(rect, ROUNDED_CORNERS, 12, EMPTY_TILE_COL);
    }
  }
}

static void slide(struct Block *b, float target_x, float target_y, float dt) {
  if (b->x < target_x) {
    b->x += SLIDE_SPEED * dt;
    if (b->x > target_x) b->x = target_x;
  } else if (b->x > target_x) {
    b->x -= SLIDE_SPEED * dt;
    if (b->x < target_x) b->x = target_x;
  }

  if (b->y < target_y) {
    b->y += SLIDE_SPEED * dt;
    if (b->y > target_y) b->y = target_y;
  } else if (b->y > target_y) {
    b->y -= SLIDE_SPEED * dt;
    if (b->y < target_y) b->y = target_y;
  }
}

static void rendernum(int num, float x, float y) {
  const char *num_str = TextFormat("%d", num);

  int digits = (num < 10) ? 1 :
               (num < 100) ? 2 :
               (num < 1000) ? 3 :
               (num < 10000) ? 4 : 5;

  float base_font_size = BLOCK_SIZE / 2.2f;
  float font_size = base_font_size - (digits - 2) * (BLOCK_SIZE * 0.08f);

  Vector2 text_size = MeasureTextEx(game_font, num_str, font_size, 0);
  Vector2 pos = {
      x + (BLOCK_SIZE - text_size.x) / 2.0f,
      y + (BLOCK_SIZE - text_size.y) / 2.0f
  };

  Color text_color = get_text_color(num);
  DrawTextEx(game_font, num_str, pos, font_size, 0, text_color);
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
        b->init = true;
        b->alpha = 0;
      }

      Color block_color = get_tile_color(b->num);

      // Fade in.
      if (b->alpha != 1) {
        b->alpha += FADE_SPEED * dt;
        if (b->alpha >= 1) {
          b->alpha = 1;
        }
        block_color.a *= b->alpha;
      }

      // Fade out
      if (b->merged) {
        b->merged_alpha -= FADE_SPEED * dt;
        if (b->merged_alpha <= 0) {
          b->merged = false;
          b->merged_alpha = 0;
        }
        Color merged_color = get_tile_color(b->merged_num);
        merged_color.a *= b->merged_alpha;
        Rectangle rect = {b->merged_x, b->merged_y, BLOCK_SIZE, BLOCK_SIZE};
        DrawRectangleRounded(rect, ROUNDED_CORNERS, 20, merged_color);
      }

      float target_x = j * BLOCK_SIZE + PADDING * (j + 1);
      float target_y = i * BLOCK_SIZE + PADDING * (i + 1);

      slide(b, target_x, target_y, dt);

      if (b->x != target_x || b->y != target_y) {
        is_animating = true;
      }

      Rectangle rect = {b->x, b->y, BLOCK_SIZE, BLOCK_SIZE};

      DrawRectangleRounded(rect, ROUNDED_CORNERS, 20, block_color);
      rendernum(b->num, b->x, b->y);
    }
  }

}

static void manage_gamestate(void) {
 
  if (gamestate == INPUT) {
    gamestate = get_input();

  } else if (gamestate == MERGE) {
    new_block = merge(dir);
    gamestate = ANIMATION;

  } else if (gamestate == ANIMATION) {
    if (!is_animating) {
      gamestate = INPUT;
      if (new_block) {
        random24();
      } else if (isover()) {
        gamestate = OVER;
      }
    }
  }

}
 
static void render(float dt) {
  BeginDrawing();
  ClearBackground(BG_COL);
  draw_empty_tiles();
  draw_tiles(dt);
  EndDrawing();
}

static void init_renderer(void) {
  InitWindow(W_SIZE, W_SIZE, TITLE);
  game_font = LoadFontEx(FONT, 64, NULL, 0);
  if (!IsFontValid(game_font)) {
    fprintf(stderr, "Error: Font \'%s\' not found / corrupted!\n", FONT);
    exit(0);
  }
  SetTargetFPS(FPS);
}

static void gameloop(void) {
  while (!WindowShouldClose()) {
    float dt = GetFrameTime(); 
    manage_gamestate();
    render(dt);
  }
}

static void end_renderer(void) {
  CloseWindow();
}

int main(void) {
  init_renderer();
  init_2048();
  gameloop();
  end_renderer();
  return 0;
}
