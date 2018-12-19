#include <stdint.h>
#include <stdio.h>

#define BOARD_SIZE 5

#define index(x, y) ((BOARD_SIZE * (x)) + (y) + 1)

int32_t board[BOARD_SIZE][BOARD_SIZE];
int32_t order[(BOARD_SIZE * BOARD_SIZE)];
int32_t visited;

void tour(int32_t x, int32_t y)
{
  int32_t i;
  static int32_t offsets[8][2] = {{ -2, -1 }, { -2, 1 }, { 2, -1 }, { 2, 1 },
                                  { -1, -2 }, { -1, 2 }, { 1, -2 }, { 1, 2 }};

  order[visited++] = index(x, y);
  board[x][y] = visited;

  if (visited == (BOARD_SIZE * BOARD_SIZE)) {
    for (i = 0; i < (BOARD_SIZE * BOARD_SIZE) - 1; i++) {
      printf("%d,", order[i]);
    }
    printf("%d\n", order[24]);
  }

  for (i = 0; i < 8; i++) {
    if (x + offsets[i][1] >= 0 && y + offsets[i][0] >= 0 &&
        x + offsets[i][1] < BOARD_SIZE && y + offsets[i][0] < BOARD_SIZE &&
        !board[x + offsets[i][1]][y + offsets[i][0]]) {
      tour(x + offsets[i][1], y + offsets[i][0]);
    }
  }

  board[x][y] = 0;
  visited--;
}

int main(int argc, char *argv[])
{
  uint32_t i, j;

  for (i = 0; i < BOARD_SIZE; i++) {
    for (j = 0; j < BOARD_SIZE; j++) {
      tour(i, j);
    }
  }

  return 0;
}
