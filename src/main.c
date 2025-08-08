#include "raylib.h"
#include "stdio.h"

#define X_SIZE 3
#define Y_SIZE 3
#define INVALID_MOVE_INDICATOR_TIME 0.3

typedef struct {
  int x;
  int y;
} Pos;

typedef struct {
  int x;
  int y;
  float false_move;
} CharState;

typedef int MAP[X_SIZE][Y_SIZE];

int can_move(Pos target_pos, const MAP *map) {
  if (target_pos.y >= Y_SIZE) {
    return 0;
  }
  if (target_pos.y < 0) {
    return 0;
  }
  if (target_pos.x < 0) {
    return 0;
  }
  if (target_pos.x >= X_SIZE) {
    return 0;
  }

  if ((*map)[target_pos.x][target_pos.y] == 1) {
    return 0;
  }

  return 1;
}

int main() {
  const int screen_width = 800;
  const int screen_height = 600;

  InitWindow(screen_width, screen_height,
             "raylib [core] example - basic window");
  SetTargetFPS(60);

  // X, _, _
  // _, X, _
  // X, _, _
  MAP map = {
      {1, 0, 1},
      {0, 1, 0},
      {0, 0, 0},
  };

  const int rect_width = screen_width / X_SIZE;
  const int rect_height = screen_height / Y_SIZE;

  const int char_width = rect_width * 0.8;
  const int char_height = rect_height * 0.8;

  CharState character = {1, 0, 0.0};
  const int char_x_offset = (rect_width - char_width) / 2;
  const int char_y_offset = (rect_height - char_height) / 2;

  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(RAYWHITE);
    // Render map
    for (int x = 0; x < X_SIZE; x++) {
      for (int y = 0; y < Y_SIZE; y++) {
        DrawRectangle(x * rect_width, y * rect_height, rect_width, rect_height,
                      map[x][y] == 1 ? BLACK : RAYWHITE);
      }
    }

    // Draw character

    Color c_colour = YELLOW;
    if (character.false_move > 0) {
      c_colour = RED;
      const float since_last_frame = GetFrameTime();
      character.false_move = character.false_move < since_last_frame
                                 ? 0
                                 : character.false_move - since_last_frame;
    }

    DrawRectangle((character.x * rect_width) + char_x_offset,
                  (character.y * rect_height) + char_y_offset, char_width,
                  char_height, c_colour);

    if (IsKeyPressed(KEY_DOWN)) {
      const Pos new_pos = {character.x, character.y + 1};
      if (can_move(new_pos, &map)) {
        character.y += 1;
      } else {
        character.false_move = INVALID_MOVE_INDICATOR_TIME;
      }
    }
    if (IsKeyPressed(KEY_UP)) {
      const Pos new_pos = {character.x, character.y - 1};
      if (can_move(new_pos, &map)) {
        character.y -= 1;
      } else {
        character.false_move = INVALID_MOVE_INDICATOR_TIME;
      }
    }
    if (IsKeyPressed(KEY_LEFT)) {
      const Pos new_pos = {character.x - 1, character.y};
      if (can_move(new_pos, &map)) {
        character.x -= 1;
      } else {
        character.false_move = INVALID_MOVE_INDICATOR_TIME;
      }
    }
    if (IsKeyPressed(KEY_RIGHT)) {
      const Pos new_pos = {character.x + 1, character.y};
      if (can_move(new_pos, &map)) {
        character.x += 1;
      } else {
        character.false_move = INVALID_MOVE_INDICATOR_TIME;
      }
    }

    EndDrawing();
  }

  CloseWindow();
  return 0;
}
