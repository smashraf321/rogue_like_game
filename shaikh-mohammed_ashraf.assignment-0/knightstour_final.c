#include <stdio.h>

int N = 0;
int row_c[8] = {-1,-2,-2,-1,1,2,2,1};
int col_c[8] = {-2,-1,1,2,2,1,-1,-2};

void place(int board[5][5], int pattern[25], int n, int row, int col);

int main (int argc, char* argv[])
{
  int board[5][5], pattern[25], i, j, n, row, col, ii, ij, jj;
  int count = 0;

  for( i = 0; i < 5; i++)
   for( j = 0; j < 5; j++)
     {

         for( ii = 0; ii < 25; ii++)
	   pattern[ii] = 0;

	 for( ij = 0; ij < 5; ij++)
	   for( jj = 0; jj < 5; jj++)
	     board[ij][jj] = 0;


       pattern[0] = (j+1)+(i*5);
       board[i][j] = 1;
 
       row = i;
       col = j;

       n = 1;

       printf(" count is: %d \n", count);
       count++;

       place( board, pattern, n, row, col);
     }

  printf(" The total open tours: %d \n", N);

  return 0;
}

void  place(int board[5][5], int pattern[25], int n, int row, int col)
{
  int c, o, new_row, new_col;

  if (n == 25)
    {
      N++;
      printf("The pattern is: \n");
      for(o = 0; o < 25; o++)
	{
	  printf(" %d",pattern[o]);
	}
      printf("\n");
    }

  for (c = 0; c < 8; c++)
    {
      new_row = row + row_c[c];
      new_col = col + col_c[c];
       
      if ( new_row >= 0 && new_row <= 4 && new_col >= 0 && new_col <= 4 )
	{
	  if ( board[new_row][new_col] == 0 )
	    {
	      board[new_row][new_col] = 1;
	      pattern[n] = (new_col + 1) + (new_row * 5); 
	      place ( board, pattern, n+1, new_row, new_col);
	      board[new_row][new_col] = 0;
	    }
	}
    }


}
