#include "raylib.h"
#include "raymath.h"
#include <stdbool.h>
#include <stdio.h>
#include <time.h>

#define COL 500
#define ROW 500
#define CELLSIZE 25
#define screenWidth 450
#define screenHeight 450

typedef enum { DEAD, ALIVE } State;

State gameGrid[COL][ROW] = {0};
State newGrid[COL][ROW] = {0};
State Gol[2][9] = {{DEAD, DEAD, DEAD, ALIVE, DEAD, DEAD, DEAD, DEAD, DEAD},
                   {DEAD, DEAD, ALIVE, ALIVE, DEAD, DEAD, DEAD, DEAD, DEAD}};

State Seeds[2][9] = {
    {DEAD, DEAD, ALIVE, DEAD, DEAD, DEAD, DEAD, DEAD, DEAD},
    {DEAD, DEAD, DEAD, DEAD, DEAD, DEAD, DEAD, DEAD, DEAD},
};

State Som[2][9] = {
    {DEAD, DEAD, ALIVE, ALIVE, DEAD, DEAD, DEAD, DEAD, DEAD},
    {DEAD, DEAD, DEAD, ALIVE, DEAD, DEAD, DEAD, DEAD, DEAD},
};

State Maze[2][9] = {
    {DEAD, DEAD, DEAD, ALIVE, DEAD, DEAD, DEAD, DEAD, DEAD},
    {DEAD, ALIVE, ALIVE, ALIVE, ALIVE, ALIVE, DEAD, DEAD, DEAD}};
State CoolEvil[2][9] = {
    {DEAD, ALIVE, ALIVE, ALIVE, ALIVE, ALIVE, DEAD, DEAD, DEAD},
    {DEAD, DEAD, DEAD, ALIVE, DEAD, DEAD, DEAD, DEAD, DEAD}};

void *gamemode[] = {&Gol, &Seeds};
void gen() {

  for (size_t i = 0; i < ROW; i++) {
    for (size_t j = 0; j < COL; j++) {
      newGrid[i][j] = gameGrid[i][j];
    }
  }

  for (size_t i = 0; i < ROW; i++) {
    for (size_t j = 0; j < COL; j++) {
      int alive_count = 0;

      for (int k = -1; k <= 1; k++) {
        for (int l = -1; l <= 1; l++) {
          if (k == 0 && l == 0)
            continue;
          int row = (i + k + ROW) % ROW;
          int col = (j + l + COL) % COL;
          if (gameGrid[row][col] == ALIVE) {
            alive_count++;
          }
        }
      }

      newGrid[i][j] = CoolEvil[gameGrid[i][j]][alive_count];
    }
  }
  for (size_t i = 0; i < ROW; i++) {
    for (size_t j = 0; j < COL; j++) {
      gameGrid[i][j] = newGrid[i][j];
    }
  }
}
void init_grid(bool rand) {
  SetRandomSeed(time(NULL));
  for (size_t i = 0; i < ROW; i++) {
    for (size_t j = 0; j < COL; j++) {

      gameGrid[i][j] = rand ? (State)(GetRandomValue(0, 1)) : 0;
    }
  }
}
int Floor(double x) {
  if (x >= 0.0) {
    return (int)x;
  } else {
    int intPart = (int)(x);
    return (x == intPart) ? (int)x : intPart - 1;
  }
}
int main() {

  State Pen = ALIVE;

  InitWindow(screenWidth, screenHeight, "Game of Life");
  init_grid(false);

  gameGrid[250][250] = ALIVE;
  double penTime = 0;
  Camera2D camera = {0};
  camera.zoom = 1.0f;

  SetTargetFPS(60);

  while (!WindowShouldClose()) {
    // Update

    // Translate based on mouse right click
    if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
      Vector2 delta = GetMouseDelta();
      delta = Vector2Scale(delta, -1.0f / camera.zoom);

      if (camera.target.y + delta.y + 450 < COL * CELLSIZE * 0.5 &&
          camera.target.y + delta.y > -COL * CELLSIZE * 0.5) {
        camera.target.y += delta.y;
      }
      if (camera.target.x + delta.x + 450 < ROW * CELLSIZE * 0.5 &&
          camera.target.x + delta.x > -ROW * CELLSIZE * 0.5) {
        camera.target.x += delta.x;
      }

      printf("x = %f\n", camera.target.x);
      printf("y = %f\n", camera.target.y);
    }
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {

      Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), camera);

      int x = Floor(mouseWorldPos.x / CELLSIZE);
      int y = Floor(mouseWorldPos.y / CELLSIZE);

      if (x >= -COL / 2 && x < COL / 2 && y >= -ROW / 2 && y < ROW / 2) {

        gameGrid[x + COL / 2][y + ROW / 2] = Pen;

      } else {
        printf("OUT OF BOUNDS\n");
      }
    }
    // Zoom based on mouse wheel
    float wheel = GetMouseWheelMove();
    if (wheel != 0) {
      // Get the world point that is under the mouse
      Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), camera);

      // Set the offset to where the mouse is
      camera.offset = GetMousePosition();

      // Set the target to match, so that the camera maps the world space
      // point under the cursor to the screen space point under the cursor at
      // any zoom
      camera.target = mouseWorldPos;

      // Zoom increment
      const float zoomIncrement = 0.125f;

      camera.zoom += (wheel * zoomIncrement);
      if (camera.zoom < zoomIncrement)
        camera.zoom = zoomIncrement;
    }

    if (IsKeyPressed(KEY_ENTER)) {
      penTime = GetTime();
      Pen = !Pen;
    }
    if (IsKeyPressed(KEY_SPACE) || IsKeyPressedRepeat(KEY_SPACE)) {
      gen();
    }
    if (IsKeyPressed(KEY_R)) {

      init_grid(true);
    }

    if (IsKeyPressed(KEY_C)) {

      init_grid(false);
    }

    // Draw
    BeginDrawing();
    ClearBackground(RAYWHITE);

    BeginMode2D(camera);
    for (int i = -COL / 2; i < COL / 2; i++) {

      for (int j = -ROW / 2; j < ROW / 2; j++) {

        switch (gameGrid[i + COL / 2][j + ROW / 2]) {
        case DEAD:
          DrawRectangle(i * CELLSIZE, j * CELLSIZE, CELLSIZE * 1, CELLSIZE * 1,
                        WHITE);
          break;
        case ALIVE:
          DrawRectangle(i * CELLSIZE, j * CELLSIZE, CELLSIZE * 1, CELLSIZE * 1,
                        BLACK);

          break;
        }
      }
    }

    Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), camera);

    float x = Floor(mouseWorldPos.x / CELLSIZE);
    float y = Floor(mouseWorldPos.y / CELLSIZE);

    if (x >= -COL / 2.f && x < COL / 2.f && y >= -ROW / 2.f && y < ROW / 2.f) {
      DrawRectangle(x * CELLSIZE, y * CELLSIZE, CELLSIZE * 1, CELLSIZE * 1,
                    GRAY);
    }

    EndMode2D();
    // DrawText("Mouse right button drag to move\n\nMouse left button toggle
    // cell state\n\nMouse wheel to zoom", 10, 10, 20,PURPLE);
    //
    if (GetTime() - penTime <= 0.3) {
      if ((bool)Pen) {

        DrawText("Switched pen to Alive", 10, 10, 20, PURPLE);
      } else
        DrawText("Switched pen to Dead", 10, 10, 20, PURPLE);
    }
    EndDrawing();
  }

  CloseWindow();
  return 0;
}
