/* [[ INCLUDES, DEFINES AND ENUMS ]] */

#include <stdio.h>
#include <raylib.h>
#include <stdlib.h>
#include "2048.h"

#define FPS 60
#define SCREEN_WINDOW_PERC 0.4f
#define CELL_BLOCK_PERC 0.9f
#define ROUNDED_CORNERS 0.1f
#define SLIDE_SPEED_FACTOR 3.5f
#define FADE_SPEED_FACTOR 0.004f

enum GameState {
  INPUT,
  MERGE,
  SLIDE,
  RESET,
};

/* [[ GLOBAL VARS ]] */

// UI stuff.
static int W_SIZE; 
static float BLOCK_SIZE;
static float PADDING;
static float SLIDE_SPEED;
static float FADE_SPEED;

// Repeatedly used colors for BG and empty tiles.
static const Color BG_COL = {187, 173, 160, 255};
static const Color EMPTY_TILE_COL = {204, 192, 180, 255};

// Title.
static const char *TITLE = "2048";

// Font.
static const char *FONT = "assets/font.ttf";
static Font font;

// Icon.
static const char *ICON = "assets/2048.png";
static Image icon;

// Game contextual variables.
static enum GameState gamestate = INPUT;
static enum Direction dir;
static bool is_sliding;
static bool new_block;

/* [[ FN DFN ]] */

// Return tile color based on the number.
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

// Return text color based on the number.
static inline Color get_text_color(int num) {
  return (num <= 4)? (Color){119, 110, 101, 255} : RAYWHITE;
}

// Draw background empty tiles.
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

// Render number properly, centering on block at x, y.
static void rendernum(int num, float x, float y) {
  const char *num_str = TextFormat("%d", num);

  int digits = (num < 10) ? 1 :
               (num < 100) ? 2 :
               (num < 1000) ? 3 :
               (num < 10000) ? 4 : 5;

  float base_font_size = BLOCK_SIZE / 2.2f;
  float font_size = base_font_size - (digits - 2) * (BLOCK_SIZE * 0.08f);

  Vector2 text_size = MeasureTextEx(font, num_str, font_size, 0);
  Vector2 pos = {
      x + (BLOCK_SIZE - text_size.x) / 2.0f,
      y + (BLOCK_SIZE - text_size.y) / 2.0f
  };

  Color text_color = get_text_color(num);
  DrawTextEx(font, num_str, pos, font_size, 0, text_color);
}

// Animate and draw non empty tiles.
static void draw_tiles(float dt) {
  is_sliding = false;

  for (int i = 0; i < SIZE; i++) {
    for (int j = 0; j < SIZE; j++) {

      struct Block *b = get_block(i, j);

      if (!b->num) {
        continue;
      }

      if (!b->init) {
        b->x = j * BLOCK_SIZE + PADDING * (j + 1);
        b->y = i * BLOCK_SIZE + PADDING * (i + 1);
        b->init = true;
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

      // slide.
      float target_x = j * BLOCK_SIZE + PADDING * (j + 1);
      float target_y = i * BLOCK_SIZE + PADDING * (i + 1);

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

      if (b->x != target_x || b->y != target_y) {
        is_sliding = true;
      }

      Rectangle rect = {b->x, b->y, BLOCK_SIZE, BLOCK_SIZE};
      DrawRectangleRounded(rect, ROUNDED_CORNERS, 20, block_color);
      rendernum(b->num, b->x, b->y);
    }
  }
}

// Manage gamestate (INPUT, MERGE, SLIDE, RESET).
static void manage_gamestate(void) {
  if (gamestate == INPUT) {
    gamestate = INPUT;

    if (IsKeyPressed(KEY_LEFT)) {
      dir = LEFT;
      gamestate = MERGE;
    
    } else if (IsKeyPressed(KEY_RIGHT)) {
      dir = RIGHT;
      gamestate = MERGE;
    
    } else if (IsKeyPressed(KEY_UP)) {
      dir = UP;
      gamestate = MERGE;
    
    } else if (IsKeyPressed(KEY_DOWN)) {
      dir = DOWN;
      gamestate = MERGE;
    
    } else if (IsKeyPressed(KEY_SPACE)) {
      gamestate = RESET;
    }

  } else if (gamestate == MERGE) {
    new_block = merge(dir);
    gamestate = SLIDE;

  } else if (gamestate == SLIDE) {
    if (!is_sliding && new_block) {
      spawn_tile();
      gamestate = INPUT;
    }

  } else if (gamestate == RESET) {
    reset_2048();
    gamestate = INPUT;
  }
}

// Render the game.
static void render(float dt) {
  BeginDrawing();
  ClearBackground(BG_COL);
  draw_empty_tiles();
  draw_tiles(dt);
  EndDrawing();
}

// Initialize the renderer.
static void init_renderer(void) {
  InitWindow(0, 0, TITLE);

  int monitor = GetCurrentMonitor();
  int screen_width = GetMonitorWidth(monitor);
  int screen_height = GetMonitorHeight(monitor);

  int min_dim = (screen_width < screen_height) ? screen_width : screen_height;
  W_SIZE = (int)(min_dim * SCREEN_WINDOW_PERC);

  int x = (screen_width - W_SIZE)/2;
  int y = (screen_height - W_SIZE)/2;

  SetWindowSize(W_SIZE, W_SIZE);
  SetWindowPosition(x, y);

  float CELL_SIZE = (float)W_SIZE / SIZE;
  BLOCK_SIZE = CELL_BLOCK_PERC * CELL_SIZE;
  PADDING = ((CELL_SIZE - BLOCK_SIZE) * SIZE) / (SIZE + 1);
  SLIDE_SPEED = W_SIZE * SLIDE_SPEED_FACTOR;
  FADE_SPEED = W_SIZE * FADE_SPEED_FACTOR;

  icon = LoadImage(ICON);
  font = LoadFontEx(FONT, 128, NULL, 0);

  if (!IsImageValid(icon) || !IsFontValid(font)) {
    fprintf(stderr, "Error: Window icon '%s' or font '%s' not found / corrupted!\n", ICON, FONT);
    exit(1);
  }

  SetWindowIcon(icon);
  SetTextureFilter(font.texture, TEXTURE_FILTER_BILINEAR);
  SetTargetFPS(FPS);
}

// Gameloop manages gamestate and renders until window is destroyed.
static void gameloop(void) {
  while (!WindowShouldClose()) {
    float dt = GetFrameTime(); 
    manage_gamestate();
    render(dt);
  }
}

// End the renderer.
static void end_renderer(void) {
  UnloadFont(font);
  UnloadImage(icon);
  CloseWindow();
}

// Entry point.
int main(void) {
  init_renderer();
  init_2048();
  gameloop();
  end_renderer();
  return 0;
}

/* [[END]] */
