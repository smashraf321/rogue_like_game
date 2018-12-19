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

  order[visited++] = index(x, y);
  board[x][y] = visited;

  if (visited == (BOARD_SIZE * BOARD_SIZE)) {
    for (i = 0; i < (BOARD_SIZE * BOARD_SIZE) - 1; i++) {
      printf("%d,", order[i]);
    }
    printf("%d\n", order[24]);
  }

  if (x - 2 >= 0 && y - 1 >= 0 && !board[x - 2][y - 1]) {
    tour(x - 2, y - 1);
  }
  if (x - 2 >= 0 && y + 1 <  BOARD_SIZE && !board[x - 2][y + 1]) {
    tour(x - 2, y + 1);
  }
  if (x - 1 >= 0 && y - 2 >= 0 && !board[x - 1][y - 2]) {
    tour(x - 1, y - 2);
  }
  if (x - 1 >= 0 && y + 2 <  BOARD_SIZE && !board[x - 1][y + 2]) {
    tour(x - 1, y + 2);
  }
  if (x + 1 <  BOARD_SIZE && y - 2 >= 0 && !board[x + 1][y - 2]) {
    tour(x + 1, y - 2);
  }
  if (x + 1 <  BOARD_SIZE && y + 2 <  BOARD_SIZE && !board[x + 1][y + 2]) {
    tour(x + 1, y + 2);
  }
  if (x + 2 <  BOARD_SIZE && y - 1 >= 0 && !board[x + 2][y - 1]) {
    tour(x + 2, y - 1);
  }
  if (x + 2 <  BOARD_SIZE && y + 1 <  BOARD_SIZE && !board[x + 2][y + 1]) {
    tour(x + 2, y + 1);
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
