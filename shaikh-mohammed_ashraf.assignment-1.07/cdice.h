#ifndef CDICE_H
# define CDICE_H

# include <iostream>
using namespace std;

class cdice
{

private:
  int base;
  int dice;
  int sides;
public:
  void dprint()
  {cout<<base<<"+"<<dice<<"d"<<sides<<endl;}

  void reset(int b=0, int d=0, int s=0)
  {base = b; dice = d; sides = s;}

  cdice(int b=0, int d=0, int s=0)
  {base = b; dice = d; sides = s;}

};

#endif
