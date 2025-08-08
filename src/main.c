#include "raylib.h"
#include "stdio.h"

#define X_SIZE 3
#define Y_SIZE 3

typedef struct {
  int x;
  int y;
} CharState;

int main() {
  const int screenWidth = 800;
  const int screenHeight = 600;

  InitWindow(screenWidth, screenHeight, "raylib [core] example - basic window");
  SetTargetFPS(60);

  // X, _, _
  // _, X, _
  // X, _, _
  int map[X_SIZE][Y_SIZE] = {
      {1, 0, 1},
      {0, 1, 0},
      {0, 0, 0},
  };

  const int rectWidth = screenWidth / X_SIZE;
  const int rectHeight = screenHeight / Y_SIZE;

  const int charWidth = rectWidth * 0.8;
  const int charHeight = rectHeight * 0.8;

  CharState character = {1, 0};

  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(RAYWHITE);
    // Render map
    for (int x = 0; x < X_SIZE; x++) {
      for (int y = 0; y < Y_SIZE; y++) {
        DrawRectangle(x * rectWidth, y * rectHeight, rectWidth, rectHeight,
                      map[x][y] == 1 ? BLACK : RAYWHITE);
      }
    }

    // Draw character
    DrawRectangle(character.x * rectWidth, character.y * rectHeight, charWidth,
                  charHeight, YELLOW);

    if (IsKeyPressed(KEY_DOWN)) {
      if (character.y < Y_SIZE -1 ) {
        character.y += 1;
      }
    }
    if (IsKeyPressed(KEY_UP)) {
      if (character.y > 0) {
        character.y -= 1;
      }
    }
    if (IsKeyPressed(KEY_LEFT)) {
      if (character.x > 0) {
        character.x -= 1;
      }
    }
    if (IsKeyPressed(KEY_RIGHT)) {
      if (character.x < X_SIZE - 1) {
        character.x += 1;
      }
    }

    EndDrawing();
  }

  CloseWindow();
  return 0;
}
