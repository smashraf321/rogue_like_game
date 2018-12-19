
#include<stdio.h>
#include<time.h>
#include<stdlib.h>
#include<math.h>
#include<endian.h>
#include<string.h>

char d[21][80]; // 2D char array to represent entire dungeon
char* path; // array(pointer) for storing absolute path of the file
unsigned char h[21][80]; // 2D array to store hardness values
unsigned char* r; // Pointer to hold info on rooms
unsigned char pcjx; // character to hold the x position of PC(@)
unsigned char pciy; // character to hold the y position of PC(@)
int n; // number of rooms
int cn; // connected number of rooms
int ld; // flag for load
int sv; // flag for save
unsigned int fversion; // for storing file version
unsigned int fsize; // for storing the size of the file

char semantic[] = {'R','L','G',3,2,7,'-','F',2,0,1,8}; // initial text in the file

//-------------j, i, x, y-------------//
int mindim[4]={2, 2, 3, 2}; // minimum coordinates and dimensions of room
int maxdim[4]={74,17,15,10}; // maximum      "      "      "      "

void dbgen(); // function for generating an empty dungeon with borders

int rndm(int u, int l); // function for generating random numbers within a range

void rgen(); // function for generating rooms, i.e, their coordinates and dimensions

int rplace(); // function for placing the generated rooms in the dungeon 

void rconnect(); // function for connecting all rooms

void cconnect(int idest, int isrc, int jdest, int jsrc); // function for drawing corridors between rooms

void ldgen(); // function for generating a loaded dungeon

void load(); // for loading a dungeon from the disk

void save(); // for saving a dungeon to the disk

void gen(); // function for the generation of actual dungeon

void dprint(); // function for printing out the generated dungeon to the screen

/***************************************************************************************************/

int main (int argc, char* argv[])
{
  srand(time(NULL)); // setting seed for random number generator

  int z; // support variable for going through arguments entered from command line

  path = malloc(strlen(getenv("HOME")) + strlen("/.rlg327/dungeon") + 1);
  strcpy(path,getenv("HOME"));
  strcat(path,"/.rlg327/dungeon");

  fversion = 0; // file version is always zero

  sv = ld = 0; // the flags for save and load are initially zero

  for(z=1; z < argc; z++) // going through arguments entered on command line to see if there was any switch for load or save
    {
      if(strcmp(argv[z],"--load")==0)
	ld = 1;
      if(strcmp(argv[z],"--save")==0)
	sv = 1;
    }

  if(argc > 1) // for seeing if utter gibberish was entered on command line
    if((ld == 0) && (sv == 0))
      {
	fprintf(stderr," \n Not even one valid argument! \n");
	return -1;
      }

  if(ld) // if flag for load is 1
    load(); // load(read) the dungeon
  else
    gen(); // actual dungeon generation

  dprint(); // print out the dungeon

  if(sv) // if flag for save is 1
    save(); // save(write) the dungeon

  free(path); // freeing the dynamically allocated variables
  free(r);

  return 0;
}

/***************************************************************************************************/

void dbgen()
{
 int i,j;
  for(i=0;i<21;i++)
    for(j=0;j<80;j++)
      {
	if(i==0 || i==20) // drawing horizontal borders
	  {
	    d[i][j]='-';
	    h[i][j]=255;
	  }
	else if(j==0 || j==79) // drawing vertical borders
	  {
	    d[i][j]='|';
	    h[i][j]=255;
	  }
	else 
	  {
	    d[i][j]=' '; // empty space elsewhere
	    h[i][j]=150;
	  }
      }
}

int rndm(int u, int l)
{
  return (rand()%(u-l+1))+l; // generating random numbers in a range
}

void rgen()
{
  int i,j;

  for(j=0;j<4;j++) // generating rooms of random dimensions and at random coordinates
    for(i=0;i<n;i++)
      {
	*(r+i*4+j)=rndm(maxdim[j],mindim[j]);
      }
}

int rplace()
{
  int i,j,k;

  for(k=0;k<n;k++) // actually placing the rooms in the dungeon
    for(i=(*(r+k*4+1))-1;i<((*(r+k*4+1))+(*(r+k*4+3))+1);i++) // the initializations and conditions are such that there's at least one row and column gap between rooms
      for(j=(*(r+k*4))-1;j<((*(r+k*4))+(*(r+k*4+2))+1);j++)      
	{
	  if(d[i][j]!=' ') // for overlap prevention
	    return 0;
	  if(!(i==(*(r+k*4+1)+(*(r+k*4+3)))||j==(*(r+k*4)+(*(r+k*4+2)))||i==(*(r+k*4+1)-1)||j==(*(r+k*4)-1))) // ensuring empty space in gap between rooms
	    {
	      d[i][j]='.';                     
	      h[i][j]=0;
	    }
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
	    {
	      d[ic][jc]='#';
	      h[ic][jc]=0;
	    }
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
	    {
	      d[ic][jc]='#';
	      h[ic][jc]=0;
	    }
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

  // storing location of PC
  pcjx = *r; 
  pciy = *(r+1);

  d[pciy][pcjx]='@';

  for(cr=0;cr<(n-1);cr++) // this condition is to ensure that all the rooms are connected by at least one corridor
    {
      cconnect(*(r+(cr+1)*4+1),*(r+cr*4+1),*(r+(cr+1)*4),*(r+cr*4)); // connecting adjacent rooms in array
    }
}

void ldgen()
{
  int i,j,k;

  for(i=0;i<21;i++)
    for(j=0;j<80;j++)
      {
	if(i==0 || i==20) // drawing horizontal borders
	  {
	    d[i][j]='-';
	  }
	else if(j==0 || j==79) // drawing vertical borders
	  {
	    d[i][j]='|';
	  }
	else if(h[i][j]==0)
	  {
	    d[i][j]='#'; // all spots with 0 hardness are #
	  }
	else 
	  {
	    d[i][j]=' '; // empty space elsewhere
	  }
      }

  for(k=0;k<n;k++)
    for(i=(*(r+k*4+1));i<((*(r+k*4+1))+(*(r+k*4+3)));i++)
      for(j=(*(r+k*4));j<((*(r+k*4))+(*(r+k*4+2)));j++)
	{
	  if(h[i][j]==0)
	    d[i][j]='.';
	}
  
  if((h[pciy][pcjx]==0)&&((pciy<21)&&(pciy>0))&&((pcjx<80)&&(pcjx>0)))
    d[pciy][pcjx]='@';
  
}

void load()
{

  FILE* f;

  f = fopen(path,"r");

  fread(semantic,1,12,f);

  fread(&fversion,4,1,f);

  *((int*) &fversion) = be32toh(*((int*) &fversion));

  fread(&fsize,4,1,f);

  *((int*) &fsize) = be32toh(*((int*) &fsize));

  n = (fsize - 1702)/4;

  r = malloc(n*4);

  fread(&pcjx,1,1,f);

  fread(&pciy,1,1,f);

  fread(&h,1,1680,f);

  fread(r,1,(fsize - 1702),f);

  ldgen();

  fclose(f);
}

void gen()
{
  n = rndm(8,5); // no. of rooms to be between 5 and 8
  r = malloc(n*4);

  cn = 0; // connected rooms are initially zero

  do // keep generating rooms till valid rooms are generated
    {
      dbgen(); // for always starting from base design, i.e, an empty dungeon
      rgen();
    }
  while(!(rplace()));

  rconnect(); // connect the rooms once they're generated
  
}

void save()
{
  FILE* f;

  f = fopen(path,"w");

  fwrite(semantic,1,12,f);

  *((int*) &fversion) = htobe32(*((int*) &fversion));

  fwrite(&fversion,4,1,f);

  fsize = (n * 4) + 1702;

  *((int*) &fsize) = htobe32(*((int*) &fsize));

  fwrite(&fsize,4,1,f);

  fwrite(&pcjx,1,1,f);

  fwrite(&pciy,1,1,f);

  fwrite(&h,1,1680,f);

  fwrite(r,1,n*4,f);

  fclose(f);

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
