
#include "hgvars.h"

char semantic[] = {'R','L','G',3,2,7,'-','F',2,0,1,8}; // initial text in the file

//-------------j, i, x, y-------------//
int mindim[4]={2, 2, 3, 2}; // minimum coordinates and dimensions of room
int maxdim[4]={74,17,15,10}; // maximum      "      "      "      "

// to cycle through the 8 directions from any node
int di[8]={-1,-1,-1, 0, 1, 1, 1, 0}; 
int dj[8]={-1, 0, 1, 1, 1, 0,-1,-1};

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

void ntdjikstra(); // function for implementing djikstra's algorithm for non tunneling monsters

void tdjikstra(); // function for implementing djikstra's algorithm for tunneling monsters

void dprint(); // function for printing out the generated dungeon to the screen

void ddprint(); // function for printing out the last digit of distance to nodes from PC(@) to the screen

void sortnt(int* startting_node, int* ending_node); // sorting the queue of seen and to visit next nodes in increasing order of their distances from PC(@)

void sortt(int* startting_node, int* ending_node); // sorting the queue of seen and to visit next nodes in increasing order of their distances from PC(@)

void swap(int* a, int* b); // swap two integers

/***************************************************************************************************/

int main (int argc, char* argv[])
{
  srand(time(NULL)); // setting seed for random number generator

  int z; // support variable for going through arguments entered from command line

  gvars_def(); // for defining initial values of global variables

  path = malloc(strlen(getenv("HOME")) + strlen("/.rlg327/dungeon") + 1);
  strcpy(path,getenv("HOME"));
  strcat(path,"/.rlg327/dungeon");

  fversion = 0; // file version is always zero

  sv = ld = 0; // the flags for save and load are initially zero

  nhz = 0; // initial number of rooms with zero hardness are zero

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

  ntdjikstra(); // implement djikstra's algorithm for non tunneling monsters
  tdjikstra(); // implement djikstra's algorithm for tunneling monsters

  dprint(); // print out the dungeon
  ddprint(); // print out distance from PC(@)

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
	    {
	      return 0;
	    }
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
	    nhz++;
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
  int i,j;
  
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

  for(i=0;i<21;i++)
    for(j=0;j<80;j++)
      {
	if(h[i][j]==0)
	  nhz++;
      }
  
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
  d[pciy][pcjx]='@';
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

void ntdjikstra()
{
  int snuqnt[nhz], vsnt[nhz];
  int sdir, cni, cnj, nni, nnj;
  int dm; // distance to me
  int i,j; // support variables
  int nv; // number of nodes visited
  int npv;

  int *add, *del;

  for(i=0;i<21;i++) 
    for(j=0;j<80;j++)
      dant[i][j]=9999; // making all nodes distance from a as infinite or rather 9999, a really large number

  for(i=0;i<nhz;i++)
    {
      snuqnt[i] = vsnt[i] = 0;
    }

  cni = pciy;
  cnj = pcjx;

  dant[cni][cnj] = nni = nnj = nv = 0;
  
  add = del = snuqnt;

  vsnt[nv] = cni * 80 + cnj;

  nv++;

  do
    {
      
      npv = 1;

      for(sdir = 0; sdir < 8; sdir++)
	{
	  
	  nni = cni + di[sdir];
	  nnj = cnj + dj[sdir];

	  if( h[nni][nnj] == 0 && nni != 0 && nnj != 0 && nni != 20 && nnj != 79)
	    {

	      npv = 0;

	      dm = 1;

	      if( ( dant[cni][cnj] + dm ) < dant[nni][nnj] )
		{
		  
		  dant[nni][nnj] = dant[cni][cnj] + dm;
		  *add = nni * 80 + nnj;

		  if(add == &snuqnt[nhz-1])
		    add = snuqnt;
		  else
		    add++;
		  
		}
	      
	    }
	  
	}
      
      if(*del == 0 && npv == 1)
	break;
      
      sortnt(del,add-1);

      vsnt[nv] = *del;

      cni = (*del)/80;
      cnj = (*del)%80;

      if(del == &snuqnt[nhz-1])
        del = snuqnt;
      else
        del++;
      
      nv++;
      

    }while(nv<nhz);

  // ------------------------ for debugging
  printf("\n nv: %d \n",nv);
  printf("\n  * del-1: %d  * add-1: %d  del: %p  add: %p\n",*(del-1),*(add-1),del,add);
  //-------------------------
}

void tdjikstra()
{
  int snuqt[2000], vst[2000];
  int sdir, cni, cnj, nni, nnj;
  int dm; // distance to me
  int i,j; // support variables
  int nv; // number of nodes visited

  int *add, *del;

  for(i=0;i<21;i++) 
    for(j=0;j<80;j++)
      dat[i][j]=9999; // making all nodes distance from a as infinite or rather 9999, a really large number

  for(i=0;i<2000;i++)
    {
     
	snuqt[i] = vst[i] = 0;
      
    }

  cni = pciy;
  cnj = pcjx;

  dat[cni][cnj] = nni = nnj = nv = 0;
  
  add = del = snuqt;

  vst[nv] = cni * 80 + cnj;

  nv++;

  do
    {

      for(sdir = 0; sdir < 8; sdir++)
	{
	  
	  nni = cni + di[sdir];
	  nnj = cnj + dj[sdir];

	  if( h[nni][nnj] != 255  && nni != 0 && nnj != 0 && nni != 20 && nnj != 79)
	    {

	      dm = h[cni][cnj]/85 + 1;

	      if( ( dat[cni][cnj] + dm ) < dat[nni][nnj] )
		{
		  
		  dat[nni][nnj] = dat[cni][cnj] + dm;
		  *add = nni * 80 + nnj;

		  if(add == &snuqt[1999])
		    add = snuqt;
		  else
		    add++;
		  
		}
	      
	    }
	  
	}
      
      sortt(del,add-1);

      vst[nv] = *del;

      cni = (*del)/80;
      cnj = (*del)%80;

      if(del == &snuqt[1999])
        del = snuqt;
      else
        del++;
      
      nv++;
      
    }while(del != add);

  // ------------------------ for debugging
  printf("\n nv: %d \n",nv);
  printf("\n  * del-1: %d  * add-1: %d  *del: %d  *add: %d\n",*(del-1),*(add-1),*del,*add);
  //-------------------------
}

void sortnt( int* start, int* end)
{

  int i,sb;
  int *p;

  sb = 0;

  for(i = 0; i < abs(end - start); i++)
    {
      for(p = start; p < (end - i); p++)
      {
	if(dant[(*p)/80][(*p)%80] > dant[(*(p+1))/80][(*(p+1))%80])
	  {
	    swap(p,p+1);
	    sb = 1;
	  }
      }
      if(sb == 0)
	break;
    }
}

void sortt( int* start, int* end)
{

  int i,sb;
  int *p;

  sb = 0;

  for(i = 0; i < abs(end - start); i++)
    {
      for(p = start; p < (end - i); p++)
      {
	if(dat[(*p)/80][(*p)%80] > dat[(*(p+1))/80][(*(p+1))%80])
	  {
	    swap(p,p+1);
	    sb = 1;
	  }
      }
      if(sb == 0)
	break;
    }
}


void swap(int* a, int* b)
{
  int t;

  t = *a;
  *a = *b;
  *b = t;
}

void ddprint()
{
  int i,j;
  char ddant[21][80],ddat[21][80];
  printf("\n");

  // -------------------------- for debugging 
  printf("\n nhz: %d \n",nhz);
  printf("\n");
  // --------------------------

  for(i=0;i<21;i++)
    for(j=0;j<80;j++)
      {
	if(i==0 || i==20)	 
	  ddant[i][j]='-';	 
	else if(j==0 || j==79)
	  ddant[i][j]='|';
	else if(h[i][j]== 0 && dant[i][j]==9999)
	  ddant[i][j]='X';
	else if(dant[i][j]==9999)
	  ddant[i][j]=' ';
	else
	  ddant[i][j]=dant[i][j]%10;
      }

  ddant[pciy][pcjx]='@';
  
  for(i=0;i<21;i++)
    {
      for(j=0;j<80;j++)
	{
	  if(j==0)
	    printf("\t");
	  if(ddant[i][j]<=9 && ddant[i][j]>=0)
	    printf("%hhd",ddant[i][j]);
	  else
	    printf("%c",ddant[i][j]);
	}
      printf("\n");
    }

  printf("\n");
  printf("\n");

   for(i=0;i<21;i++)
    for(j=0;j<80;j++)
      {
	if(i==0 || i==20)	 
	  ddat[i][j]='-';	 
	else if(j==0 || j==79)
	  ddat[i][j]='|';
	else
	  ddat[i][j]=dat[i][j]%10;
      }

  ddat[pciy][pcjx]='@';
  
  for(i=0;i<21;i++)
    {
      for(j=0;j<80;j++)
	{
	  if(j==0)
	    printf("\t");
	  if(ddat[i][j]<=9 && ddat[i][j]>=0)
	    printf("%hhd",ddat[i][j]);
	  else
	    printf("%c",ddat[i][j]);
	}
      printf("\n");
    }
}
