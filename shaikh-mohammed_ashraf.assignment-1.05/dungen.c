
//#include<stdio.h>
#include<ncurses.h>
#include<time.h>
#include<stdlib.h>
#include<math.h>
#include<endian.h>
//#include<machine/endian.h>
#include<string.h>
#include<ctype.h>
#include<unistd.h>

#define TELEPATH 0X00000001
#define INTELLIGENT 0X00000002
#define TUNNELING 0X00000004
#define ERRATIC 0X00000008

char d[21][80]; // 2D char array to represent entire dungeon
char* path; // array(pointer) for storing absolute path of the file
unsigned char h[21][80]; // 2D array to store hardness values
unsigned char* r; // Pointer to hold info on rooms
unsigned char pcjx; // character to hold the x position of PC(@)
unsigned char pciy; // character to hold the y position of PC(@)
int n; // number of rooms
int nhz; // number of nodes with hardness zero
int ld; // flag for load
int sv; // flag for save
int dant[21][80],dat[21][80]; // 2D arrays to store the distances of every node to PC(@) for both tunneling and non tunneling monsters
unsigned int fversion; // for storing file version
unsigned int fsize; // for storing the size of the file

int pisdead;
int no_of_monsters; // number of monsters with hardcoded 5
int no_of_alive_monsters;
char character_map[21][80]; // to store info on where characters are
int quit;
int menu_stair_active;

struct mon_q_wl
{
  char char_PC_M;
  int speed;
  int next_turn;
  int status_DOA;
  int location_i;
  int location_j;
  int special_info;

} * waiting_list; // this is my priority queue for deciding next turn


//---------------------------------------------------------------------------------------------------------------------
char semantic[] = {'R','L','G',3,2,7,'-','F',2,0,1,8}; // initial text in the file
int valid_user_input_game[22] = {'7','y','8','k','9','u','6','l','3','n','2','j','1','b','4','h','5',' ','Q','<','>','m'};
int valid_user_input_menu[3] = {27,KEY_UP,KEY_DOWN};
//-------------j, i, x, y-------------//
int mindim[4]={2, 2, 3, 2}; // minimum coordinates and dimensions of room
int maxdim[4]={74,17,15,10}; // maximum      "      "      "      "

// to cycle through the 8 directions from any node
int di[8]={-1,-1,-1, 0, 1, 1, 1, 0};
int dj[8]={-1, 0, 1, 1, 1, 0,-1,-1};

//---------------------------------------------------------------------------------------------------------------------

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





void monster_generator(); // populates the waiting_list queue

int valid_location_for_monsters(int i_loc, int j_loc);

int player_is_alive(); // checks whether PC is player_is_alive

int all_monsters_dead(); // checks if all monsters dead and PC alive

void time_to_move(); // for taking in the first in line

void queue_reorder(); // for reordering the priority queue

void game_print(); // printing the dungen game frames

int within_LOS();

void move_to_PC();

void tunnel_to_PC_v_LOS();

void tdjikstra_to_PC();

void ntdjikstra_to_PC();

//void queue_display();

void new_dungen();

int invalid_user_input_game(int user_input);

int invalid_user_input_menu(int menu_input, int* s_pos, int* e_pos, int* t_f, int* t_r, WINDOW* menu_win);
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

  nhz = 0; // initial number of rooms with zero hardness are zero

  no_of_monsters = 5;

  pisdead=1; // player is alive initially

  menu_stair_active = quit = 0;

  for(z=1; z < argc; z++) // going through arguments entered on command line to see if there was any switch for load or save
    {
      if(strcmp(argv[z],"--load")==0)
	ld = 1;
      if(strcmp(argv[z],"--save")==0)
	sv = 1;
      if(strcmp(argv[z],"--nummon")==0)
      no_of_monsters = atoi(argv[z+1]);
    }

  if(no_of_monsters == 0)
  {
    printf("\n No monsters given so you automatically win! \n\n");
    free(path);
    return 0;
  }
  if(ld) // if flag for load is 1
    load(); // load(read) the dungeon
  else
    gen(); // actual dungeon generation


  if(no_of_monsters >= nhz - 5) // for making sure monsters not more than certain limit
  no_of_monsters = nhz - 5;
  no_of_alive_monsters = no_of_monsters;
  waiting_list = malloc((no_of_monsters+1) * sizeof( *waiting_list));

  monster_generator();

  ntdjikstra(); // implement djikstra's algorithm for non tunneling monsters
  tdjikstra(); // implement djikstra's algorithm for tunneling monsters

  //printf("\n \n Here's the Dungeon Map, Good Luck! \n \n ");

  //dprint(); // print out the dungeon

  //printf("\n \n");

  //queue_display();

//------------- GAME LOGIC BEGINS ---------------------------------------------------------------------------------------

  //while(pisdead)
  initscr();
  cbreak();
  //raw();
  keypad(stdscr,TRUE);
  noecho();
  while(player_is_alive() && !quit )
  {
    if(all_monsters_dead())
    {
      break;
    }

    game_print();

    time_to_move();

    queue_reorder();

    //queue_display();

    //usleep(50000);

  }
  endwin();

  if(quit)
    printf("\n You Quit! \n\n");
  else if(all_monsters_dead())
    printf("\n You Win!!! \n\n");
  else
    printf("\n You lost, Better Luck next time!! \n\n");

//------------------------------------------------------------------------------------------------------------------------

  //ddprint(); // print out distance from PC(@)

  if(sv) // if flag for save is 1
    save(); // save(write) the dungeon

  free(r);
  free(waiting_list);
  free(path); // freeing the dynamically allocated variables


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
	  if(!(d[ic][jc]=='.' || d[ic][jc]=='<' || d[ic][jc]=='>')) // draw a corridor if no room is found
	    {
	      d[ic][jc]='#';
	      h[ic][jc]=0;
	    }
	  ic+=inci;
	  i++;
	  c++;
	}
      c=0;
      zg = rndm(6,4);
      while((j<abs(endj))&&(c<zg)) // horizontal movement
	{
	  if(!(d[ic][jc]=='.' || d[ic][jc]=='<' || d[ic][jc]=='>'))
	    {
	      d[ic][jc]='#';
	      h[ic][jc]=0;
	    }
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

  d[*(r+1)+1][*r+2] = '<';
  d[*(r + 1*4 + 1) + 1][*(r + 1*4) + 2] = '>';

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
  r = malloc(n*4); // n*4 as 4 is the size of int data type

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
/*
  // ------------------------ for debugging
  printf("\n nv: %d \n",nv);
  printf("\n  * del-1: %d  * add-1: %d  del: %p  add: %p\n",*(del-1),*(add-1),del,add);
  //-------------------------
*/
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
/*
  // ------------------------ for debugging
  printf("\n nv: %d \n",nv);
  printf("\n  * del-1: %d  * add-1: %d  *del: %d  *add: %d\n",*(del-1),*(add-1),*del,*add);
  //-------------------------
*/
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

/*
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
*/

void monster_generator()
{
  int i,j,i_loc,j_loc;

  for(i = 0; i < 21; i++)
  for(j = 0; j < 80; j++)
  character_map[i][j] = 99;

  waiting_list[0].char_PC_M = '@';
  waiting_list[0].speed = 10;
  waiting_list[0].next_turn = 0;
  waiting_list[0].status_DOA = 1;
  waiting_list[0].location_i = pciy;
  waiting_list[0].location_j = pcjx;
  waiting_list[0].special_info = 0;

  character_map[pciy][pcjx] = '@';

  waiting_list[1].char_PC_M = 5;
  waiting_list[1].speed = 5;
  waiting_list[1].next_turn = 0;
  waiting_list[1].status_DOA = 1;
  waiting_list[1].special_info = 0;

  for(i = 1; i <= no_of_monsters; i++)
  {
    if(i == 1)
    {
        i_loc = rndm(19,1);
        j_loc = rndm(78,1);

        while(!valid_location_for_monsters(i_loc,j_loc))
        {
          i_loc = rndm(19,1);
          j_loc = rndm(78,1);
        }

        waiting_list[i].location_i = i_loc;
        waiting_list[i].location_j = j_loc;

        character_map[i_loc][j_loc] = waiting_list[i].char_PC_M;
    }
    else
    {
      waiting_list[i].char_PC_M = rndm(15,0);
      waiting_list[i].speed = rndm(20,5);
      waiting_list[i].next_turn = 0;
      waiting_list[i].status_DOA = 1;
      waiting_list[i].special_info = 0;

      i_loc = rndm(19,1);
      j_loc = rndm(78,1);

      //((h[i_loc][j_loc] == 0) && (character_map[i_loc][j_loc] == 99) && (!(i_loc == pciy && j_loc == pcjx)) && (!(i_loc == (pciy+1) && j_loc == pcjx)) && (!(i_loc == pciy && j_loc == (pcjx+1))) )
      while(!valid_location_for_monsters(i_loc,j_loc))
      {
        i_loc = rndm(19,1);
        j_loc = rndm(78,1);
      }

      waiting_list[i].location_i = i_loc;
      waiting_list[i].location_j = j_loc;

      character_map[i_loc][j_loc] = waiting_list[i].char_PC_M;
    }
  }
}

int valid_location_for_monsters(int i_loc, int j_loc)
{
  int c_1,c_2,c_3,c_4,c_5,c_6,c_7;

  c_7 = 0;
  c_1 = c_2 = c_3 = 0;
  c_4 = c_5 = c_6 = 0;

  if(h[i_loc][j_loc] == 0)
    c_1 = 1;
  if(character_map[i_loc][j_loc] == 99)
    c_2 = 1;
  if(i_loc == pciy && j_loc == pcjx)
    c_3 = 1;
  if(i_loc == (pciy+1) && j_loc == pcjx)
    c_4 = 1;
  if(i_loc == pciy && j_loc == (pcjx+1))
    c_5 = 1;
  if(i_loc == (pciy+1) && j_loc == (pcjx+1))
    c_6 = 1;
  if(i_loc == (pciy+1) && j_loc == (pcjx+2))
    c_7 = 1;

  return ( c_1 && c_2 && !c_3 && !c_4 && !c_5 && !c_6 && !c_7 );

}

int player_is_alive()
{
  int i,a;
  a = 1;
  for(i = 0; i <= no_of_monsters; i++)
  {
    if(waiting_list[i].char_PC_M == '@')
    {
      a = waiting_list[i].status_DOA;
      break;
    }
  }
  return a;
}

int all_monsters_dead()
{
 //return no_of_alive_monsters;
 int i,c;
 c = 0;
 for(i = 0; i<= no_of_monsters; i++)
  {
    if(!(waiting_list[i].char_PC_M == '@') && (waiting_list[i].status_DOA == 0))
    c++;
  }
  if(c == no_of_monsters)
    return 1;
  else
    return 0;
}

void time_to_move()
{
  int user_input;
  user_input = 0;
  if(!(waiting_list[0].char_PC_M == '@' || waiting_list[0].status_DOA == 0 || menu_stair_active == 1))
  {
    if(within_LOS())
    {
      //printf("\n PC detected in LOS \n");
      move_to_PC();
    }
    else
    {
      if((waiting_list[0].char_PC_M & TELEPATH) == TELEPATH )
      {
        if((waiting_list[0].char_PC_M & (INTELLIGENT | TUNNELING)) == (TUNNELING))
        {
          tunnel_to_PC_v_LOS();
        }
        else if((waiting_list[0].char_PC_M & (INTELLIGENT | TUNNELING)) == (INTELLIGENT | TUNNELING))
        {
          //use tdjikstra to go to pc, eat monsters along the way and draw hashes when tunneling
          tdjikstra_to_PC();
        }
        else
        {
          if((waiting_list[0].char_PC_M & (INTELLIGENT | TUNNELING)) == (INTELLIGENT))
          {
            // use ntdjikstra to go to PC, eat monsters, no need to draw hashes since can only go where there's zero hardness
            ntdjikstra_to_PC();
          }

        }

      }

    }

  }
  else
  {
    if(waiting_list[0].char_PC_M == '@' && waiting_list[0].status_DOA == 1)
    {
      do {
        user_input = getch();
      } while(invalid_user_input_game(user_input));

      ntdjikstra();
      tdjikstra();
      /*
      if(user_input == '7' || user_input == 'y')
      {
        if(h[pciy+di[0]][pcjx+dj[0]] == 0)
          {
            character_map[pciy][pcjx] = 99;
            pciy += di[0];
            pcjx += dj[0];
            character_map[pciy][pcjx] = '@';
            waiting_list[0].location_i = pciy;
            waiting_list[0].location_j = pcjx;
          }
      }
      else if(user_input == '8' || user_input == 'k')
      {
        if(h[pciy+di[1]][pcjx+dj[1]] == 0)
          {
            character_map[pciy][pcjx] = 99;
            pciy += di[1];
            pcjx += dj[1];
            character_map[pciy][pcjx] = '@';
            waiting_list[0].location_i = pciy;
            waiting_list[0].location_j = pcjx;
          }
      }
      else if(user_input == '9' || user_input == 'u')
      {
        if(h[pciy+di[2]][pcjx+dj[2]] == 0)
          {
            character_map[pciy][pcjx] = 99;
            pciy += di[2];
            pcjx += dj[2];
            character_map[pciy][pcjx] = '@';
            waiting_list[0].location_i = pciy;
            waiting_list[0].location_j = pcjx;
          }
      }
      else if(user_input == '6' || user_input == 'l')
      {
        if(h[pciy+di[3]][pcjx+dj[3]] == 0)
          {
            character_map[pciy][pcjx] = 99;
            pciy += di[3];
            pcjx += dj[3];
            character_map[pciy][pcjx] = '@';
            waiting_list[0].location_i = pciy;
            waiting_list[0].location_j = pcjx;
          }
      }
      else if(user_input == '3' || user_input == 'n')
      {
        if(h[pciy+di[4]][pcjx+dj[4]] == 0)
          {
            character_map[pciy][pcjx] = 99;
            pciy += di[4];
            pcjx += dj[4];
            character_map[pciy][pcjx] = '@';
            waiting_list[0].location_i = pciy;
            waiting_list[0].location_j = pcjx;
          }
      }
      else if(user_input == '2' || user_input == 'j')
      {
        if(h[pciy+di[5]][pcjx+dj[5]] == 0)
          {
            character_map[pciy][pcjx] = 99;
            pciy += di[5];
            pcjx += dj[5];
            character_map[pciy][pcjx] = '@';
            waiting_list[0].location_i = pciy;
            waiting_list[0].location_j = pcjx;
          }
      }
      else if(user_input == '1' || user_input == 'b')
      {
        if(h[pciy+di[6]][pcjx+dj[6]] == 0)
          {
            character_map[pciy][pcjx] = 99;
            pciy += di[6];
            pcjx += dj[6];
            character_map[pciy][pcjx] = '@';
            waiting_list[0].location_i = pciy;
            waiting_list[0].location_j = pcjx;
          }
      }
      else if(user_input == '4' || user_input == 'h')
      {
        if(h[pciy+di[7]][pcjx+dj[7]] == 0)
          {
            character_map[pciy][pcjx] = 99;
            pciy += di[7];
            pcjx += dj[7];
            character_map[pciy][pcjx] = '@';
            waiting_list[0].location_i = pciy;
            waiting_list[0].location_j = pcjx;
          }
      }
      else
      {
        if(user_input == 'Q')
          quit = 1;
      }*/
    }

  }

}

int within_LOS()
{
  int id, is, jd, js;
  int dx, dy, stepx, stepy, frac;

  id = pciy;
  jd = pcjx;

  is = waiting_list[0].location_i;
  js = waiting_list[0].location_j;

  dx = id - is;
  dy = jd - js;

  if(dy < 0)
  {
    dy = -1 * dy;
    stepy = -1;
  }
  else
  {
    stepy = 1;
  }

  if(dx < 0)
  {
    dx = -1 * dx;
    stepx = -1;
  }
  else
  {
    stepx = 1;
  }

  dy <<= 1;
  dx <<= 1;

  if(dx > dy)
  {
    frac = dy - (dx >> 1);
    while(is != id)
    {
      is += stepx;
      if(frac >= 0)
      {
        js += stepy;
        frac -= dx;
      }
      frac += dy;
      if(!(h[is][js] == 0))
        return 0;

    }
  }
  else
  {
    frac = dx - (dy >> 1);
    while (js != jd)
    {
      js += stepy;
      if(frac >= 0)
      {
        is += stepx;
        frac -= dy;
      }
      frac += dx;
      if(!(h[is][js] == 0))
        return 0;
    }
  }

/*
    i=j=inci=incj=0;
    ic=is;
    jc=js;
    endi=id-is; // distance between start and destination, so i can iterate with successive increments till it reaches destination
    endj=jd-js; //                     "                      j                        "                       "

    if(endi!=0)
      inci=abs(endi)/endi; // for easier movement vertically, all we need to do is add inci, up or down is automatically taken care of
    if(endj!=0)
      incj=abs(endj)/endj; //          "          horizontally              "                left or right              "

    while(i<abs(endi)||j<abs(endj)) // loop till both i and j of destination are reached
      {
        //c = 0;
        if(i<abs(endi)) // vertical movement
  	     {
  	        if(!(h[ic][jc]==0))
  	         {
  	            return 0;
  	         }
  	        ic+=inci;
  	        i++;

         }

        if(j<abs(endj)) // horizontal movement
        	{
        	  if(!(h[ic][jc]==0))
        	    {
                return 0;
        	    }

        	  jc+=incj;
        	  j++;

        	}

      }*/


      return 1;
}

void move_to_PC()
{
  int id, is, jd, js;
  int ic, jc, dx, dy, stepx, stepy, frac, ii;

  id = pciy;
  jd = pcjx;

  ic = is = waiting_list[0].location_i;
  jc = js = waiting_list[0].location_j;

  dx = id - is;
  dy = jd - js;

  if(dy < 0)
  {
    dy = -1 * dy;
    stepy = -1;
  }
  else
  {
    stepy = 1;
  }

  if(dx < 0)
  {
    dx = -1 * dx;
    stepx = -1;
  }
  else
  {
    stepx = 1;
  }

  dy <<= 1;
  dx <<= 1;

  if(dx > dy)
  {
    frac = dy - (dx >> 1);
    if(ic != id)
    {
      ic += stepx;
      if(frac >= 0)
      {
        jc += stepy;
        //frac -= dx;
      }
      //frac += dy;

      if(character_map[ic][jc] != 99)
      {
        for(ii=0;ii<=no_of_monsters;ii++)
        {
          if(waiting_list[ii].location_i == ic && waiting_list[ii].location_j == jc)
            {
              waiting_list[ii].status_DOA = 0;
              if(waiting_list[ii].char_PC_M != '@')
                no_of_alive_monsters--;
              //break;
            }
        }

      }
      waiting_list[0].location_i = ic;
      waiting_list[0].location_j = jc;
      character_map[ic][jc] = waiting_list[0].char_PC_M;
      character_map[is][js] = 99;
      //printf("\n ic: %d jc: %d is: %d js: %d \n",ic, jc, is, js);
      if(!(d[is][js]=='.' || d[is][js]=='<' || d[is][js]=='>'))
        d[is][js]='#';


    }
  }
  else
  {
    frac = dx - (dy >> 1);
    if(jc != jd)
    {
      jc += stepy;
      if(frac >= 0)
      {
        ic += stepx;
        //frac -= dy;
      }
      //frac += dx;


      if(character_map[ic][jc] != 99)
      {
        for(ii=0;ii<=no_of_monsters;ii++)
        {
          if(waiting_list[ii].location_i == ic && waiting_list[ii].location_j == jc)
            {
              waiting_list[ii].status_DOA = 0;
              if(waiting_list[ii].char_PC_M != '@')
                no_of_alive_monsters--;
              //break;
            }
        }

      }
      waiting_list[0].location_i = ic;
      waiting_list[0].location_j = jc;
      character_map[ic][jc] = waiting_list[0].char_PC_M;
      character_map[is][js] = 99;
      //printf("\n ic: %d jc: %d is: %d js: %d \n",ic, jc, is, js);
      if(!(d[is][js]=='.' || d[is][js]=='<' || d[is][js]=='>'))
        d[is][js]='#';
    }
  }

/*
    i=j=inci=incj=c=0;

    endi=id-is; // distance between start and destination, so i can iterate with successive increments till it reaches destination
    endj=jd-js; //                     "                      j                        "                       "

    if(endi!=0)
      inci=abs(endi)/endi; // for easier movement vertically, all we need to do is add inci, up or down is automatically taken care of
    if(endj!=0)
      incj=abs(endj)/endj; //          "          horizontally              "                left or right              "

    ic=is+inci;
    jc=js+incj;

    if((i<abs(endi)||j<abs(endj))) // to see if both i and j of destination are reached
      {
  	        if(h[ic][jc]==0)
  	         {
  	            if(character_map[ic][jc] != 99)
                {
                  for(ii=0;ii<=no_of_monsters;ii++)
                  {
                    if(waiting_list[ii].location_i == ic && waiting_list[ii].location_j == jc)
                      {
                        waiting_list[ii].status_DOA = 0;
                        break;
                      }
                  }

                }
                waiting_list[0].location_i = ic;
                waiting_list[0].location_j = jc;
                character_map[ic][jc] = waiting_list[0].char_PC_M;
                character_map[is][js] = 99;
                if(d[is][js]!='.')
            		  d[is][js]='#';

  	         }
      }
 */
}

void tunnel_to_PC_v_LOS()
{
  int id, is, jd, js;
  int i,ii,j,c,ic,jc,endi,endj,inci,incj;

  id = pciy;
  jd = pcjx;

  is = waiting_list[0].location_i;
  js = waiting_list[0].location_j;

    i=j=inci=incj=c=0;

    endi=id-is; // distance between start and destination, so i can iterate with successive increments till it reaches destination
    endj=jd-js; //                     "                      j                        "                       "

    if(endi!=0)
      inci=abs(endi)/endi; // for easier movement vertically, all we need to do is add inci, up or down is automatically taken care of
    if(endj!=0)
      incj=abs(endj)/endj; //          "          horizontally              "                left or right              "

    ic=is+inci;
    jc=js+incj;

    if((i<abs(endi)||j<abs(endj))) // to see if both i and j of destination are reached
      {
  	        if((h[ic][jc]<=85))
  	         {
  	            if(character_map[ic][jc] != 99)
                {
                  for(ii=0;ii<=no_of_monsters;ii++)
                  {
                    if(waiting_list[ii].location_i == ic && waiting_list[ii].location_j == jc)
                      {
                        waiting_list[ii].status_DOA = 0;
                        if(waiting_list[ii].char_PC_M != '@')
                          no_of_alive_monsters--;
                        //break;
                      }
                  }

                }
                waiting_list[0].location_i = ic;
                waiting_list[0].location_j = jc;
                character_map[ic][jc] = waiting_list[0].char_PC_M;
                character_map[is][js] = 99;

            		if(!(d[is][js]=='.' || d[is][js]=='<' || d[is][js]=='>'))
            		  d[is][js]='#';

                if(!(d[ic][jc]=='.' || d[ic][jc]=='<' || d[ic][jc]=='>'))
              		d[ic][jc]='#';

            		h[ic][jc]=0;

                nhz++;
  	         }
             else
             {
               h[ic][jc]-=85;
             }
      }


}

void tdjikstra_to_PC()
{
  int is, js, ic, jc, id, jd;
  int i,ii,min;

  is = waiting_list[0].location_i;
  js = waiting_list[0].location_j;

  ic = is + di[0];
  jc = js + dj[0];

  min = dat[ic][jc];
  id = ic;
  jd = jc;

  for(i = 1; i < 8; i++)
  {
    ic = is + di[i];
    jc = js + dj[i];

    if(dat[ic][jc] < min)
    {
      min = dat[ic][jc];
      id = ic;
      jd = jc;
    }
  }

  if((h[id][jd]<=85))
   {
      if(character_map[id][jd] != 99)
      {
        for(ii=0;ii<=no_of_monsters;ii++)
        {
          if(waiting_list[ii].location_i == id && waiting_list[ii].location_j == jd)
            {
              waiting_list[ii].status_DOA = 0;
              if(waiting_list[ii].char_PC_M != '@')
                no_of_alive_monsters--;
              //break;
            }
        }

      }
      waiting_list[0].location_i = id;
      waiting_list[0].location_j = jd;
      character_map[id][jd] = waiting_list[0].char_PC_M;
      character_map[is][js] = 99;

      if(!(d[is][js]=='.' || d[is][js]=='<' || d[is][js]=='>'))
        d[is][js]='#';

      if(!(d[id][jd]=='.' || d[id][jd]=='<' || d[id][jd]=='>'))
        d[id][jd]='#';

      h[id][jd]=0;

      nhz++;
   }
   else
   {
     h[id][jd]-=85;
   }


}

void ntdjikstra_to_PC()
{
  int is, js, ic, jc, id, jd;
  int i,ii,min;
  // di[], dj[], dant[][]

  is = waiting_list[0].location_i;
  js = waiting_list[0].location_j;

  ic = is + di[0];
  jc = js + dj[0];

  min = dant[ic][jc];
  id = ic;
  jd = jc;

  for(i = 1; i < 8; i++)
  {
    ic = is + di[i];
    jc = js + dj[i];

    if(dant[ic][jc] < min)
    {
      min = dant[ic][jc];
      id = ic;
      jd = jc;
    }
  }

  if(character_map[id][jd] != 99)
  {
    for(ii=0;ii<=no_of_monsters;ii++)
    {
      if(waiting_list[ii].location_i == id && waiting_list[ii].location_j == jd)
        {
          waiting_list[ii].status_DOA = 0;
          if(waiting_list[ii].char_PC_M != '@')
            no_of_alive_monsters--;
          //break;
        }
    }

  }
  waiting_list[0].location_i = id;
  waiting_list[0].location_j = jd;
  character_map[id][jd] = waiting_list[0].char_PC_M;
  character_map[is][js] = 99;

}

void queue_reorder()
{
  int i,j,o;
  struct mon_q_wl temp;

  temp = waiting_list[0];

  temp.next_turn = 1000/temp.speed + temp.next_turn;

  for(i = 1; i<=no_of_monsters; i++)
  waiting_list[i-1] = waiting_list[i];

  waiting_list[no_of_monsters] = temp;

  for(i = 0; i < no_of_monsters; i++)
  {
    o = 0;
    for(j = 0; j < (no_of_monsters - i); j++ )
    {
      if(waiting_list[j].next_turn > waiting_list[j+1].next_turn)
      {
        temp = waiting_list[j];
        waiting_list[j] = waiting_list[j+1];
        waiting_list[j+1] = temp;
      }

      o++;
    }

    if(o == 0)
    {
      break;
    }

  }

}

void game_print()
{

  int i,j;
  //printf("\n");
  mvprintw(0,28,"You're playing Roguelike Dungeon");
  refresh();
  for(i=0;i<21;i++)
    {
      for(j=0;j<80;j++)
      {
        //if(j==0)
          //printf("\t");

        if(character_map[i][j] != 99)
        {
          if(character_map[i][j] == '@')
            mvprintw(i+1,j,"%c",character_map[i][j]);
            //printf("%c",character_map[i][j]);
          else
            mvprintw(i+1,j,"%x",character_map[i][j]);
            //printf("%x",character_map[i][j]);
        }
	      else
        mvprintw(i+1,j,"%c",d[i][j]);
        //printf("%c",d[i][j]);
      }
      //printf("\n");
    }
    refresh();

}

void new_dungen()
{
  free(r);
  //endwin();
  gen();
  no_of_alive_monsters = no_of_monsters;
  monster_generator();
  //ntdjikstra();
  //tdjikstra();
  //initscr();
  //raw();
  //keypad(stdscr,TRUE);
  //noecho();
}

int invalid_user_input_game(int user_input)
{
  int i,ii,j,iy,ix;
  int invalid,menu_input,s_pos,e_pos,t_for,t_rev;
  char* m_dir_y[] = {"South","Same","North"};
  char* m_dir_x[] = {"East","Same","West"};
  WINDOW *menu_win;

  invalid = 1;

  for (i=0;i<22;i++)
  {
    if(user_input == valid_user_input_game[i])
    {
      if(!(user_input == '5' || user_input == ' '))
        {
                  if(user_input == '<' && d[pciy][pcjx] == '<')
                    {
                      menu_stair_active = 1;
                      new_dungen();
                      invalid = 0;
                      break;
                    }
                    else if(user_input == '>' && d[pciy][pcjx] == '>')
                    {
                      menu_stair_active = 1;
                      new_dungen();
                      invalid = 0;
                      break;
                    }
                    else if(user_input == '7' || user_input == 'y')
                    {
                      menu_stair_active = 0;
                      if(h[pciy+di[0]][pcjx+dj[0]] == 0)
                        {
                          character_map[pciy][pcjx] = 99;
                          pciy += di[0];
                          pcjx += dj[0];
                          if(character_map[pciy][pcjx] != 99)
                          {
                            for(ii=0;ii<=no_of_monsters;ii++)
                            {
                              if(waiting_list[ii].location_i == pciy && waiting_list[ii].location_j == pcjx && waiting_list[ii].char_PC_M != '@')
                                {
                                  waiting_list[ii].status_DOA = 0;
                                  no_of_alive_monsters--;
                                  break;
                                }
                            }

                          }
                          character_map[pciy][pcjx] = '@';
                          waiting_list[0].location_i = pciy;
                          waiting_list[0].location_j = pcjx;
                          invalid = 0;
                          break;
                        }
                    }
                    else if(user_input == '8' || user_input == 'k')
                    {
                      menu_stair_active = 0;
                      if(h[pciy+di[1]][pcjx+dj[1]] == 0)
                        {
                          character_map[pciy][pcjx] = 99;
                          pciy += di[1];
                          pcjx += dj[1];
                          if(character_map[pciy][pcjx] != 99)
                          {
                            for(ii=0;ii<=no_of_monsters;ii++)
                            {
                              if(waiting_list[ii].location_i == pciy && waiting_list[ii].location_j == pcjx && waiting_list[ii].char_PC_M != '@')
                                {
                                  waiting_list[ii].status_DOA = 0;
                                  no_of_alive_monsters--;
                                  break;
                                }
                            }

                          }
                          character_map[pciy][pcjx] = '@';
                          waiting_list[0].location_i = pciy;
                          waiting_list[0].location_j = pcjx;
                          invalid = 0;
                          break;
                        }
                    }
                    else if(user_input == '9' || user_input == 'u')
                    {
                      menu_stair_active = 0;
                      if(h[pciy+di[2]][pcjx+dj[2]] == 0)
                        {
                          character_map[pciy][pcjx] = 99;
                          pciy += di[2];
                          pcjx += dj[2];
                          if(character_map[pciy][pcjx] != 99)
                          {
                            for(ii=0;ii<=no_of_monsters;ii++)
                            {
                              if(waiting_list[ii].location_i == pciy && waiting_list[ii].location_j == pcjx && waiting_list[ii].char_PC_M != '@')
                                {
                                  waiting_list[ii].status_DOA = 0;
                                  no_of_alive_monsters--;
                                  break;
                                }
                            }

                          }
                          character_map[pciy][pcjx] = '@';
                          waiting_list[0].location_i = pciy;
                          waiting_list[0].location_j = pcjx;
                          invalid = 0;
                          break;
                        }
                    }
                    else if(user_input == '6' || user_input == 'l')
                    {
                      menu_stair_active = 0;
                      if(h[pciy+di[3]][pcjx+dj[3]] == 0)
                        {
                          character_map[pciy][pcjx] = 99;
                          pciy += di[3];
                          pcjx += dj[3];
                          if(character_map[pciy][pcjx] != 99)
                          {
                            for(ii=0;ii<=no_of_monsters;ii++)
                            {
                              if(waiting_list[ii].location_i == pciy && waiting_list[ii].location_j == pcjx && waiting_list[ii].char_PC_M != '@')
                                {
                                  waiting_list[ii].status_DOA = 0;
                                  no_of_alive_monsters--;
                                  break;
                                }
                            }

                          }
                          character_map[pciy][pcjx] = '@';
                          waiting_list[0].location_i = pciy;
                          waiting_list[0].location_j = pcjx;
                          invalid = 0;
                          break;
                        }
                    }
                    else if(user_input == '3' || user_input == 'n')
                    {
                      menu_stair_active = 0;
                      if(h[pciy+di[4]][pcjx+dj[4]] == 0)
                        {
                          character_map[pciy][pcjx] = 99;
                          pciy += di[4];
                          pcjx += dj[4];
                          if(character_map[pciy][pcjx] != 99)
                          {
                            for(ii=0;ii<=no_of_monsters;ii++)
                            {
                              if(waiting_list[ii].location_i == pciy && waiting_list[ii].location_j == pcjx && waiting_list[ii].char_PC_M != '@')
                                {
                                  waiting_list[ii].status_DOA = 0;
                                  no_of_alive_monsters--;
                                  break;
                                }
                            }

                          }
                          character_map[pciy][pcjx] = '@';
                          waiting_list[0].location_i = pciy;
                          waiting_list[0].location_j = pcjx;
                          invalid = 0;
                          break;
                        }
                    }
                    else if(user_input == '2' || user_input == 'j')
                    {
                      menu_stair_active = 0;
                      if(h[pciy+di[5]][pcjx+dj[5]] == 0)
                        {
                          character_map[pciy][pcjx] = 99;
                          pciy += di[5];
                          pcjx += dj[5];
                          if(character_map[pciy][pcjx] != 99)
                          {
                            for(ii=0;ii<=no_of_monsters;ii++)
                            {
                              if(waiting_list[ii].location_i == pciy && waiting_list[ii].location_j == pcjx && waiting_list[ii].char_PC_M != '@')
                                {
                                  waiting_list[ii].status_DOA = 0;
                                  no_of_alive_monsters--;
                                  break;
                                }
                            }

                          }
                          character_map[pciy][pcjx] = '@';
                          waiting_list[0].location_i = pciy;
                          waiting_list[0].location_j = pcjx;
                          invalid = 0;
                          break;
                        }
                    }
                    else if(user_input == '1' || user_input == 'b')
                    {
                      menu_stair_active = 0;
                      if(h[pciy+di[6]][pcjx+dj[6]] == 0)
                        {
                          character_map[pciy][pcjx] = 99;
                          pciy += di[6];
                          pcjx += dj[6];
                          if(character_map[pciy][pcjx] != 99)
                          {
                            for(ii=0;ii<=no_of_monsters;ii++)
                            {
                              if(waiting_list[ii].location_i == pciy && waiting_list[ii].location_j == pcjx && waiting_list[ii].char_PC_M != '@')
                                {
                                  waiting_list[ii].status_DOA = 0;
                                  no_of_alive_monsters--;
                                  break;
                                }
                            }

                          }
                          character_map[pciy][pcjx] = '@';
                          waiting_list[0].location_i = pciy;
                          waiting_list[0].location_j = pcjx;
                          invalid = 0;
                          break;
                        }
                    }
                    else if(user_input == '4' || user_input == 'h')
                    {
                      menu_stair_active = 0;
                      if(h[pciy+di[7]][pcjx+dj[7]] == 0)
                        {
                          character_map[pciy][pcjx] = 99;
                          pciy += di[7];
                          pcjx += dj[7];
                          if(character_map[pciy][pcjx] != 99)
                          {
                            for(ii=0;ii<=no_of_monsters;ii++)
                            {
                              if(waiting_list[ii].location_i == pciy && waiting_list[ii].location_j == pcjx && waiting_list[ii].char_PC_M != '@')
                                {
                                  waiting_list[ii].status_DOA = 0;
                                  no_of_alive_monsters--;
                                  break;
                                }
                            }

                          }
                          character_map[pciy][pcjx] = '@';
                          waiting_list[0].location_i = pciy;
                          waiting_list[0].location_j = pcjx;
                          invalid = 0;
                          break;
                        }
                    }
                    else if(user_input == 'm')
                    {
                      menu_stair_active = 1;
                      mvprintw(0,28,"You're now viewing menu         ");
                      refresh();
                      menu_win = newwin(19,78,2,1);
                      wborder(menu_win,'|','|','-','-','*','*','*','*');
                      //display waiting_list
                      mvwprintw(menu_win,1,22,"There is/are %d monsters in the game",no_of_alive_monsters);
                      mvwprintw(menu_win,2,38,"^");
                      mvwprintw(menu_win,17,38,"v");
                      j = 3;
                      s_pos = e_pos = t_for = t_rev = 0;
                      while ((j <= (no_of_alive_monsters + 2)) && (j<=16))
                      {
                        if(waiting_list[e_pos].char_PC_M != '@' && waiting_list[e_pos].status_DOA != 0)
                        {
                          iy = ix = 1;
                          if((pciy - waiting_list[e_pos].location_i) != 0)
                            iy = ((pciy - waiting_list[e_pos].location_i)/abs(pciy - waiting_list[e_pos].location_i)) + 1;
                          if((pcjx - waiting_list[e_pos].location_j) != 0)
                            ix = ((pcjx - waiting_list[e_pos].location_j)/abs(pcjx - waiting_list[e_pos].location_j)) + 1;

                          mvwprintw(menu_win,j,20,"Monster %x is: %2d %5s & %2d %5s",waiting_list[e_pos].char_PC_M,abs(pciy - waiting_list[e_pos].location_i),m_dir_y[iy],abs(pcjx - waiting_list[e_pos].location_j),m_dir_x[ix]);
                          t_for++;
                          j++;
                        }
                        e_pos++;
                      }
                      wrefresh(menu_win);
                      do{
                      /* mvprintw(22,3," s_pos = %d ; e_pos = %d ; t_rev = %d ; t_for = %d  ",s_pos,e_pos,t_rev,t_for);
                         refresh(); */
                        menu_input = getch();
                      }while (invalid_user_input_menu(menu_input, &s_pos, &e_pos, &t_for, &t_rev, menu_win));
                      //output blanks where list was displayed
                      wborder(menu_win,' ',' ',' ',' ',' ',' ',' ',' ');
                      mvwprintw(menu_win,1,22,"                                     ");
                      mvwprintw(menu_win,2,38," ");
                      mvwprintw(menu_win,17,38," ");
                      for(j = 3; j <= 16; j++)
                        mvwprintw(menu_win,j,20,"                                         ");
                      wrefresh(menu_win);
                      delwin(menu_win);

                      invalid = 0;
                      break;
                    }
                    else
                    {
                      if(user_input == 'Q')
                        {
                          quit = 1;
                          invalid = 0;
                          break;
                        }
                    }
          }
          else
          {
            menu_stair_active = 0;
            invalid = 0;
            break;
          }
    }
  }

  return invalid;

}

int invalid_user_input_menu(int menu_input, int* start_pos, int* end_pos, int* tracker_forward, int* tracker_reverse, WINDOW* menu_win)
{
  int i,j,iy,ix,invalid;
  char* m_dir_y[] = {"South","Same","North"};
  char* m_dir_x[] = {"East","Same","West"};

  invalid = 1;

  for(i=0;i<3;i++)
  {
    if(menu_input == valid_user_input_menu[i])
    {
      if(menu_input == 27)
      {
        invalid = 0;
        break;
      }
      if(menu_input == KEY_UP && no_of_alive_monsters > 14 && (*start_pos) > 0 && *tracker_reverse > 0)
      {
        for(j = 3; j <= 16; j++)
          mvwprintw(menu_win,j,20,"                                         ");

        j=16;

        *end_pos = (*start_pos) + 1;
        *tracker_forward = *tracker_reverse + 1;

        while( *start_pos > 0 &&j >=3)
        {
          if(waiting_list[*start_pos].char_PC_M != '@' && waiting_list[*start_pos].status_DOA != 0)
          {
            iy = ix = 1;
            if((pciy - waiting_list[*start_pos].location_i) != 0)
              iy = ((pciy - waiting_list[*start_pos].location_i)/abs(pciy - waiting_list[*start_pos].location_i)) + 1;
            if((pcjx - waiting_list[*start_pos].location_j) != 0)
              ix = ((pcjx - waiting_list[*start_pos].location_j)/abs(pcjx - waiting_list[*start_pos].location_j)) + 1;

            mvwprintw(menu_win,j,20,"Monster %x is: %2d %5s & %2d %5s",waiting_list[*start_pos].char_PC_M,abs(pciy - waiting_list[*start_pos].location_i),m_dir_y[iy],abs(pcjx - waiting_list[*start_pos].location_j),m_dir_x[ix]);
            j--;
            (*tracker_reverse)--;
          }
          (*start_pos)--;
        }

        wrefresh(menu_win);

        break;
      }
      if(menu_input == KEY_DOWN && no_of_alive_monsters > 14 && (*end_pos) <= no_of_monsters && *tracker_forward < no_of_alive_monsters)
      {
        for(j = 3; j <= 16; j++)
          mvwprintw(menu_win,j,20,"                                         ");

        j = 3;

        *start_pos = (*end_pos) - 1;
        *tracker_reverse = *tracker_forward - 1;

        while( *end_pos <= no_of_monsters && j <= 16)
        {
          if(waiting_list[*end_pos].char_PC_M != '@' && waiting_list[*end_pos].status_DOA != 0)
          {
            iy = ix = 1;
            if((pciy - waiting_list[*end_pos].location_i) != 0)
              iy = ((pciy - waiting_list[*end_pos].location_i)/abs(pciy - waiting_list[*end_pos].location_i)) + 1;
            if((pcjx - waiting_list[*end_pos].location_j) != 0)
              ix = ((pcjx - waiting_list[*end_pos].location_j)/abs(pcjx - waiting_list[*end_pos].location_j)) + 1;

            mvwprintw(menu_win,j,20,"Monster %x is: %2d %5s & %2d %5s",waiting_list[*end_pos].char_PC_M,abs(pciy - waiting_list[*end_pos].location_i),m_dir_y[iy],abs(pcjx - waiting_list[*end_pos].location_j),m_dir_x[ix]);
            (*tracker_forward)++;
            j++;
          }
          (*end_pos)++;
        }

        wrefresh(menu_win);

        break;
      }
    }
    //mvprintw(23,0,"start_pos: %d;end_pos: %d;tracker_reverse: %d;tracker_forward: %d ",*start_pos,*end_pos,*tracker_reverse,*tracker_forward);
    refresh();
  }

  return invalid;

}
/*
void queue_display()
{
  int i;

  mvprintw(23,0," Char Speed Nxt_Trn Status_DOA Loc_i Loc_j spcl_info ");
  refresh();
  for(i = 0; i<=no_of_monsters; i++ )
  {
    if(waiting_list[i].char_PC_M == '@')
    {
      mvprintw(24,0,"  %x     %d    %d      %d      %d      %d      %d ",waiting_list[i].char_PC_M, waiting_list[i].speed, waiting_list[i].next_turn, waiting_list[i].status_DOA, waiting_list[i].location_i, waiting_list[i].location_j, waiting_list[i].special_info);
      refresh();
    }
    //else
      //printf("  %x     %d    %d      %d      %d      %d      %d \n",waiting_list[i].char_PC_M, waiting_list[i].speed, waiting_list[i].next_turn, waiting_list[i].status_DOA, waiting_list[i].location_i, waiting_list[i].location_j, waiting_list[i].special_info);

    break;
  }
  //printf("\n\n");
}
*/
