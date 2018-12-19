
#include<stdio.h>
#include<time.h>
#include<stdlib.h>
#include<math.h>

char d[21][80]; // 2D char array to represent entire dungeon
int r[8][4]; // 2D int array to store info on rooms
int n; // number of rooms
int cn; // connected number of rooms

//-------------i, j, x, y-------------//
int mindim[4]={2, 2, 3, 2}; // minimum coordinates and dimensions of room
int maxdim[4]={10,64,15,10}; // maximum      "      "      "      "

void dbgen(); // function for generating an empty dungeon with borders

int rndm(int u, int l); // function for generating random numbers within a range

void rgen(); // function for generating rooms, i.e, their coordinates and dimensions

// int gap(int i, int j, int k); // function for generating (essentially ensuring) gaps (of 3) between rooms. Not using this as takes a while to generate such a dungeon as the screen is small

int rplace(); // function for placing the generated rooms in the dungeon 

void rconnect(); // function for connecting all rooms

void cconnect(int idest, int isrc, int jdest, int jsrc); // function for drawing corridors between rooms

void dprint(); // function for printing out the generated dungeon to the screen

int main (int argc, char* argv[])
{
  srand(time(NULL));
  
  n = rndm(8,5); // no. of rooms to be between 5 and 8
  cn = 0; // connected rooms are initially zero

  do // keep generating rooms till valid rooms are generated
    {
      dbgen(); // for always starting from base design, i.e, an empty dungeon
      rgen();
    }
  while(!(rplace()));

  rconnect(); // connect the rooms once they're generated
  dprint(); // print out the dungeon
  return 0;
}

void dbgen()
{
 int i,j;
  for(i=0;i<21;i++)
    for(j=0;j<80;j++)
      {
	if(i==0 || i==20) // drawing horizontal borders
	  d[i][j]='-';
	else if(j==0 || j==79) // drawing vertical borders
	  d[i][j]='|';
	else 
	  d[i][j]=' '; // empty space elsewhere
      }
}

int rndm(int u, int l)
{
  return (rand()%(u-l+1))+l; // generating random numbers in a range
}

void rgen()
{
  int i,j;

  for(i=0;i<8;i++) // making all the initial room information 0
    for(j=0;j<4;j++)
      r[i][j]=0;

  for(j=0;j<4;j++) // generating rooms of random dimensions and at random coordinates
    for(i=0;i<n;i++)
      {
	r[i][j]=rndm(maxdim[j],mindim[j]);
      }
}
/*
int gap(int i, int j, int k)
{
  return (i==(r[k][0]+r[k][3])||i==(r[k][0]+r[k][3]+1)||i==(r[k][0]+r[k][3]+2)||j==(r[k][1]+r[k][2])||j==(r[k][1]+r[k][2]+1)||j==(r[k][1]+r[k][2]+2)||i==(r[k][0]-1)||i==(r[k][0]-2)||i==(r[k][0]-3)||j==(r[k][1]-1)||j==(r[k][1]-2)||j==(r[k][1]-3)); // ensuring 3 empty spaces in gap between rooms
}
*/
int rplace()
{
  int i,j,k;

  for(k=0;k<n;k++) // actually placing the rooms in the dungeon
    for(i=r[k][0]-1;i<((r[k][0]+r[k][3])+1);i++) // the initializations and conditions are such that there's at least one row and column gap between rooms
      for(j=r[k][1]-1;j<((r[k][1]+r[k][2])+1);j++) 
	{
	  if(d[i][j]!=' ') // for overlap prevention
	    return 0;
	  if(!(i==(r[k][0]+r[k][3])||j==(r[k][1]+r[k][2])||i==(r[k][0]-1)||j==(r[k][1]-1))) // ensuring empty space in gap between rooms
	     d[i][j]='.';
	}
  return 1;
}

void cconnect(int id, int is, int jd, int js)
{
  int i,j,c,zg,ic,jc,endi,endj,inci,incj;

  i=j=inci=incj=0;
  ic=is+1;
  jc=js+1;
  endi=id-is; // distance between start and destination, so i can iterate with successive increments till it reaches destination
  endj=jd-js; //                     "                      j                        "                       "

  if(endi!=0)
    inci=abs(endi)/endi; // for easier movement vertically, all we need to do is add inci, up or down is automatically taken care of
  if(endj!=0)
    incj=abs(endj)/endj; //          "          horizontally              "                left or right              "       

  while(i<abs(endi)||j<abs(endj)) // loop till both i and j of destination are reached
    {
      c=0;
      zg = rndm(6,4); // for ensuring zig-zag corridors, a corridor can be straight only for 4 or 6 spaces, provided the destination (either along x or y direction) is not in straight line along the start
      while((i<abs(endi))&&(c<zg)) // vertical movement
	{
	  if(!(d[ic][jc]=='.')) // draw a corridor if no room is found
	    d[ic][jc]='#';
	  if((d[ic-inci][jc]=='#')&&(d[ic][jc]=='.')&&(d[ic+inci][jc]=='.')) // entered a new room
	    cn++; // for keeping track of how many connections are present between rooms. Not really used in this program
	  ic+=inci;
	  i++;
	  c++;
	}
      c=0;
      zg = rndm(6,4);
      while((j<abs(endj))&&(c<zg)) // horizontal movement
	{
	  if(!(d[ic][jc]=='.'))
	    d[ic][jc]='#';
	  if((d[ic][jc-incj]=='#')&&(d[ic][jc]=='.')&&(d[ic][jc+incj]=='.'))
	    cn++;
	  jc+=incj;
	  j++;
	  c++;
	}
    }
}

void rconnect()
{
  int cr;

  for(cr=0;cr<(n-1);cr++) // this condition is to ensure that all the rooms are connected by at least one corridor
    {
      cconnect(r[cr+1][0],r[cr][0],r[cr+1][1],r[cr][1]); // connecting adjacent rooms in array
    }
}

void dprint()
{
  int i,j;
  printf("\n");
  for(i=0;i<21;i++)
    {
      for(j=0;j<80;j++)
	{
	  if(j==0)
	    printf("\t");
	  printf("%c",d[i][j]);
	}
      printf("\n");
    }
}
