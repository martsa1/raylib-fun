#include "memory.h"
#include "raylib.h"
#include "stdio.h"
#include <math.h>

#define X_SIZE 3
#define Y_SIZE 4
#define INVALID_MOVE_INDICATOR_TIME 0.3

enum terrain {
  GROUND = 0,
  WALL = 1,
};

typedef struct {
  int x;
  int y;
  int passable;
} Pos;

typedef struct {
  int x;
  int y;
  float false_move;
} CharState;

typedef struct {
  Pos pos;
  float dist;
  int parent_idx;
} Distance;

typedef int MAP[Y_SIZE][X_SIZE];

#define MAX_PATH Y_SIZE *X_SIZE

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

  if ((*map)[target_pos.y][target_pos.x] == 1) {
    return 0;
  }

  return 1;
}

// A python generator would be _perfect_ here...
int find_neighbours(Pos cur_pos, const MAP *map, Pos *neighbours) {
  int count = 0;

  // Diagonals and origin should not be returned.
  // This leaves essentially cardinal directions only.
  Pos cardinals[4] = {
      // North
      {cur_pos.x, cur_pos.y - 1},
      // South
      {cur_pos.x, cur_pos.y + 1},
      // East
      {cur_pos.x - 1, cur_pos.y},
      // West
      {cur_pos.x + 1, cur_pos.y},
  };

  for (int idx = 0; idx < 4; idx++) {
    if (can_move(cardinals[idx], map)) {
      neighbours[count] = cardinals[idx];
      count++;
    }
  }
  return count;
}

int calculate_path(Pos *path, int max_path_size, Pos player_pos, Pos dest,
                   int (*map)[Y_SIZE][X_SIZE]) {
  printf("searching for path from (%d, %d) to (%d, %d)\n", player_pos.x,
         player_pos.y, dest.x, dest.y);
  // Djikstra path finding.
  Distance unvisited[Y_SIZE * X_SIZE];
  Distance visited[Y_SIZE * X_SIZE];
  // Pos visited[Y_SIZE * X_SIZE];
  // int visit_count = 0;
  int found = 0;
  int total_unvisited = 1;
  int total_visited = 0;
  int count = 0;

  // set start pos
  Pos cur_pos = {player_pos.x, player_pos.y};
  unvisited[0] = (Distance){cur_pos, 0, -1};

  while (found == 0) {
    count++;
    if (count > pow(Y_SIZE * X_SIZE, 2)) {
      printf("it was then he realised, he fucked up...\n");
      break;
    };
    // visit the shorted-dist node of 'unvisited'
    // find neighbours of current node
    // add viable neighbours to unvited
    // record path if shorter than existing path to current node in
    // 'visisted'
    int closest_dist = 0;
    int closest_node = 0;
    for (int idx = 0; idx < total_unvisited; idx++) {
      Distance *node = &unvisited[idx];
      if (node->dist < closest_dist) {
        closest_node = idx;
        closest_dist = node->dist;
      }
    }

    if (total_unvisited <= 0) {
      printf("Nothing more to explore...\n");
      return -1;
    } else {
      printf("total_unvisited: %d\n", total_unvisited);
    }
    Distance cur_node = unvisited[closest_node];

    // 'pop' cur_node out of unvisited... (feck)
    unvisited[closest_node] = unvisited[total_unvisited - 1];
    total_unvisited--;

    // Find neighbours of current node...
    Pos neighbours[4];
    int num_neighbours = find_neighbours(cur_pos, map, neighbours);

    // Add each neighbour to visit
    for (int idx = 0; idx < num_neighbours; idx++) {
      Pos *neighbour = &neighbours[idx];
      Distance new_target = {
          {neighbour->y, neighbour->x}, 1 + cur_node.dist, total_visited};
      // Check if neighbour is already due to be visited and overwrite if
      // we have a shorter path...
      int found_new = 0;
      for (int idx = 0; idx < total_unvisited; idx++) {
        if (memcmp(&unvisited[idx].pos, &new_target.pos, sizeof(Pos)) == 0) {
          found_new = 1;
          if (new_target.dist < unvisited[idx].dist) {
            printf("found shorter neighbour: (%d, %d): %f, parent_id: %d\n",
                   new_target.pos.x, new_target.pos.y, new_target.dist,
                   new_target.parent_idx);
            unvisited[idx] = new_target;
          } else {
            printf("found longer neighbour: (%d, %d): %f, parent_id: %d\n",
                   new_target.pos.x, new_target.pos.y, new_target.dist,
                   new_target.parent_idx);
            // Existing node path is already shorter, bail.
            break;
          }
        }
        if (found_new == 0) {
          printf("found new neighbour: (%d, %d): %f, parent_id: %d\n",
                 new_target.pos.x, new_target.pos.y, new_target.dist,
                 new_target.parent_idx);
          unvisited[total_unvisited] = new_target;
          total_unvisited++;
        }
      }
    }

    // mark cur_node as visited...
    visited[total_visited] = cur_node;
    total_visited++;
    if (memcmp(&cur_node.pos, &dest, sizeof(Pos)) == 0) {
      found = 1;
    }
  }

  // Figure out wtf we did...
  printf("total_visited: %d\n", total_visited);
  Distance backwards_path[Y_SIZE * X_SIZE];
  backwards_path[0] = visited[total_visited - 1];
  int back_p_idx = 0;
  printf("total_unvisited: %d, total_visited: %d, back_p_idx: %d\n",
         total_unvisited, total_visited, back_p_idx);
  while (true) { // TODO: Don't leave this wank in here...
    Distance prev = backwards_path[back_p_idx];
    if (prev.parent_idx > -1) {
      back_p_idx++;
      backwards_path[back_p_idx] = visited[prev.parent_idx];
    } else {
      break;
    }
  }

  for (int idx = back_p_idx; idx > -1; idx--) {
    path[total_visited - idx] = backwards_path[idx].pos;
  }
  return total_visited;
}

int main() {
  const int screen_width = 800;
  const int screen_height = 600;

  InitWindow(screen_width, screen_height,
             "raylib [core] example - basic window");
  SetTargetFPS(60);

  // X, _, _
  // _, X, _
  // _, X, _
  // _, _, _
  MAP map = {
      {1, 0, 0},
      {0, 1, 0},
      {0, 1, 0},
      {0, 0, 0},
  };

  const int rect_width = screen_width / X_SIZE;
  const int rect_height = screen_height / Y_SIZE;

  const int char_width = rect_width * 0.8;
  const int char_height = rect_height * 0.8;

  int moved = 1;
  CharState character = {1, 0, 0.0};
  const int char_x_offset = (rect_width - char_width) / 2;
  const int char_y_offset = (rect_height - char_height) / 2;

  const Pos destination = {0, 1};

  Pos path[MAX_PATH];

  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(RAYWHITE);
    // Render map
    for (int x = 0; x < X_SIZE; x++) {
      for (int y = 0; y < Y_SIZE; y++) {
        DrawRectangle(x * rect_width, y * rect_height, rect_width, rect_height,
                      map[y][x] == 1 ? BLACK : RAYWHITE);
      }
    }

    // Draw character
    Color c_colour = YELLOW;
    if (character.false_move > 0) {
      c_colour = RED;
      const float since_last_frame = GetFrameTime();
      character.false_move = character.false_move - since_last_frame;
    }

    DrawRectangle((character.x * rect_width) + char_x_offset,
                  (character.y * rect_height) + char_y_offset, char_width,
                  char_height, c_colour);

    if (IsKeyPressed(KEY_DOWN)) {
      const Pos new_pos = {character.x, character.y + 1};
      if (can_move(new_pos, &map)) {
        character.y += 1;
        moved = 1;
      } else {
        character.false_move = INVALID_MOVE_INDICATOR_TIME;
      }
    }
    if (IsKeyPressed(KEY_UP)) {
      const Pos new_pos = {character.x, character.y - 1};
      if (can_move(new_pos, &map)) {
        character.y -= 1;
        moved = 1;
      } else {
        character.false_move = INVALID_MOVE_INDICATOR_TIME;
      }
    }
    if (IsKeyPressed(KEY_LEFT)) {
      const Pos new_pos = {character.x - 1, character.y};
      if (can_move(new_pos, &map)) {
        character.x -= 1;
        moved = 1;
      } else {
        character.false_move = INVALID_MOVE_INDICATOR_TIME;
      }
    }
    if (IsKeyPressed(KEY_RIGHT)) {
      const Pos new_pos = {character.x + 1, character.y};
      if (can_move(new_pos, &map)) {
        character.x += 1;
        moved = 1;
      } else {
        character.false_move = INVALID_MOVE_INDICATOR_TIME;
      }
    }

    // Render path to dest
    if (moved == 1) {
      int path_len =
          calculate_path(&path[0], MAX_PATH, (Pos){character.x, character.y},
                         destination, &map);

      printf("path:\n");
      for (int idx = 0; idx < path_len; idx++) {
        printf("%d: (%d, %d)\n", idx, path[idx].x, path[idx].y);
      }
      // TODO: render path array here...
    }

    EndDrawing();
    moved = 0;
  }

  CloseWindow();
  return 0;
}
