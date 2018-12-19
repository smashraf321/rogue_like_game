#ifndef PARSERMON_H
# define PARSERMON_H

# include "cdice.h"
# include <fstream>
# include <vector>
# include <cstring>
# include <ncurses.h>
using namespace std;

# include "npc.h"

# define PEEK fin.peek()

# define NEWLINE 10
# define SPACE 32
# define TAB 9
# define VTAB 11

class mondesc
{

public:
  string name;
  string desc;
  vector<int> colors;
  vector<string> abilitiestring;
  int abilities;
  int rarity;
  unsigned char symbol;
  cdice speed;
  cdice hitpoints;
  cdice damage;

  mondesc()
  {name="";desc="";abilities=rarity=0;symbol=' ';}
  int parsemon(ifstream& fin);

};

int fileparser(char* filename);

int deschecker(string word);

#endif
