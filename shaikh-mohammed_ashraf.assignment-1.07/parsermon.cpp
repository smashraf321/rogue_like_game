# include "parsermon.h"
# include <cstdlib>

using namespace std;

int deschecker(string word)
{
  int chk0,chk1,chk2,invalid;

  if(word[0] == 'B'){
    chk0 = 1;
  }else if(word[0] == 'N'){
    chk0 = 1;
  }else if(word[0] == 'D'){
    chk0 = 1;
  }else if(word[0] == 'C'){
    chk0 = 1;
  }else if(word[0] == 'S'){
    chk0 = 1;
  }else if(word[0] == 'A'){
    chk0 = 1;
  }else if(word[0] == 'H'){
    chk0 = 1;
  }else if(word[0] == 'R'){
    chk0 = 1;
  }else if(word[0] == 'E'){
    chk0 = 1;
  }else{
    chk0 = 0;
  }

  if(word[1] == 'E'){
    chk1 = 1;
  }else if(word[1] == 'A'){
    chk1 = 1;
  }else if(word[1] == 'O'){
    chk1 = 1;
  }else if(word[1] == 'P'){
    chk1 = 1;
  }else if(word[1] == 'B'){
    chk1 = 1;
  }else if(word[1] == 'Y'){
    chk1 = 1;
  }else if(word[1] == 'R'){
    chk1 = 1;
  }else if(word[1] == 'N'){
    chk1 = 1;
  }else{
    chk1 = 0;
  }

  if(word[2] == 'G'){
    chk2 = 1;
  }else if(word[2] == 'M'){
    chk2 = 1;
  }else if(word[2] == 'S'){
    chk2 = 1;
  }else if(word[2] == 'L'){
    chk2 = 1;
  }else if(word[2] == 'E'){
    chk2 = 1;
  }else if(word[2] == 'I'){
    chk2 = 1;
  }else if(word[2] == ' '){
    chk2 = 1;
  }else if(word[2] == 'T'){
    chk2 = 1;
  }else{
    chk2 = 0;
  }

  if(chk0 && chk1 && chk2)
    invalid = 1;
  else
    invalid = 0;

  return invalid;
}

int mondesc::parsemon(ifstream& fin)
{
  //cout<<"you in parsemon"<<endl;
  string st, word;
  int base, dice, sides, clr, abil, invalid;
  int field_tracker;
  bool has_name,has_desc,has_color,has_abil,has_speed,has_hp,has_dam,has_rrty,has_sym;

  field_tracker = invalid = 0;
  has_name = has_sym = has_speed = has_hp = has_dam = has_abil = has_desc = has_color = has_rrty = false;

  fin>>st;
  //reading first word, i.e a NAME, ABIL, etc. in this case

  while(st!="END"){

      if(st == "NAME"){
        if(has_name){
          invalid = 1;
          break;
        }
        has_name = true;

        /* // overly complicated way of reading name
        fin>>name;
        while(PEEK != NEWLINE){
          if(PEEK == SPACE){
            name += fin.get();
          }
          else{
            fin>>word;
            name+=word;
          }
        }*/

        char ch = fin.get(); //eating the empty space after NAME
        if(ch != SPACE){
          invalid = 1;
          break;
        }
        getline(fin,name);
        if(strlen(name.c_str())<1 || deschecker(name)){
          invalid = 1;
          break;
        }

        field_tracker++;
      }

      else if(st == "DESC"){
        if(has_desc){
          invalid = 1;
          break;
        }
        has_desc = true;
        fin.get(); //eats the newline
        word = "";
        do {
          if(PEEK == NEWLINE){
            desc+=fin.get();
          }
          else{
            getline(fin,word);

            if(strlen(word.c_str())>77 || deschecker(word)){
              invalid = 1;
              break;
            }
            if(strlen(word.c_str())==1 && word[0]=='.')
              break;
            desc+=word;
            desc+="\n";
          }

        } while( PEEK != EOF );

        desc[strlen(desc.c_str())-1]='\0'; // to remove the last newline. Not really required but better practise.

        if(invalid)
          break;
        // Different logic for parsing DESC
        /*while(PEEK != '.')
        {
          if(PEEK==NEWLINE || PEEK==SPACE || PEEK==TAB || PEEK==VTAB)
            desc+=fin.get();
          else{
            fin>>word; // for not consuming newline
            desc+=word;
          }
        }
        getline(fin,st); // for eating '.' and newline*/
        field_tracker++;
      }

      else if(st == "COLOR"){
        if(has_color){
          invalid = 1;
          break;
        }
        has_color = true;
        while(PEEK != NEWLINE){
          fin>>word;
          if(word == "BLACK"){
            clr = COLOR_BLACK;
            //clr = 0;
          }else if(word == "RED"){
            clr = COLOR_RED;
            //clr = 1;
          }else if(word == "GREEN"){
            clr = COLOR_GREEN;
            //clr = 2;
          }else if(word == "YELLOW"){
            clr = COLOR_YELLOW;
            //clr = 3;
          }else if(word == "BLUE"){
            clr = COLOR_BLUE;
            //clr = 4;
          }else if(word == "MAGENTA"){
            clr = COLOR_MAGENTA;
            //clr = 5;
          }else if(word == "CYAN"){
            clr = COLOR_CYAN;
            //clr = 6;
          }else if(word == "WHITE"){
            clr = COLOR_WHITE;
            //clr = 7;
          }else{
            //invalid COLOR
            invalid = 1;
            break;
          }
          colors.push_back(clr);
        }

        if((int)colors.size() < 1){
          invalid = 1;
          break;
        }

        fin.get(); // for eating newline
        field_tracker++;
      }

      else if(st == "SPEED"){
        if(has_speed){
          invalid = 1;
          break;
        }
        has_speed = true;
        getline(fin,word,'+');
        if(strlen(word.c_str())==0){
          invalid = 1;
          break;
        }
        base = atoi(word.c_str());
        getline(fin,word,'d');
        if(strlen(word.c_str())==0){
          invalid = 1;
          break;
        }
        dice = atoi(word.c_str());
        getline(fin,word);
        if(strlen(word.c_str())==0){
          invalid = 1;
          break;
        }
        sides = atoi(word.c_str());
        speed.reset(base,dice,sides);
        field_tracker++;
      }

      else if(st == "ABIL"){
        if(has_abil){
          invalid = 1;
          break;
        }
        has_abil = true;
        while(PEEK != NEWLINE){
          fin>>word;
          abilitiestring.push_back(word);
          if(word == "SMART"){
            abil = NPC_SMART;
          }else if(word == "TELE"){
            abil = NPC_TELE;
          }else if(word == "TUNNEL"){
            abil = NPC_TUNNEL;
          }else if(word == "ERRATIC"){
            abil = NPC_ERRATIC;
          }else if(word == "PASS"){
            abil = NPC_PASS;
          }else if(word == "UNIQ"){
            abil = NPC_UNIQ;
          }else if(word == "PICKUP"){
            abil = NPC_PICKUP;
          }else if(word == "DESTROY"){
            abil = NPC_DESTROY;
          }else if(word == "BOSS"){
            abil = NPC_BOSS;
          }else{
            // invalid ABIL
            invalid = 1;
            break;
          }
          abilities = abilities | abil;
        }
        fin.get(); // for eating newline
        field_tracker++;
      }

      else if(st == "HP"){
        if(has_hp){
          invalid = 1;
          break;
        }
        has_hp = true;
        getline(fin,word,'+');
        if(strlen(word.c_str())==0){
          invalid = 1;
          break;
        }
        base = atoi(word.c_str());
        getline(fin,word,'d');
        if(strlen(word.c_str())==0){
          invalid = 1;
          break;
        }
        dice = atoi(word.c_str());
        getline(fin,word);
        if(strlen(word.c_str())==0){
          invalid = 1;
          break;
        }
        sides = atoi(word.c_str());
        hitpoints.reset(base,dice,sides);
        field_tracker++;
      }

      else if(st == "DAM"){
        if(has_dam){
          invalid = 1;
          break;
        }
        has_dam = true;
        getline(fin,word,'+');
        if(strlen(word.c_str())==0){
          invalid = 1;
          break;
        }
        base = atoi(word.c_str());
        getline(fin,word,'d');
        if(strlen(word.c_str())==0){
          invalid = 1;
          break;
        }
        dice = atoi(word.c_str());
        getline(fin,word);
        if(strlen(word.c_str())==0){
          invalid = 1;
          break;
        }
        sides = atoi(word.c_str());
        damage.reset(base,dice,sides);
        field_tracker++;
      }

      else if(st == "SYMB"){
        if(has_sym){
          invalid = 1;
          break;
        }
        has_sym = true;

        fin.get();
        getline(fin,word);

        if(strlen(word.c_str()) != 1){
          invalid = 1;
          break;
        }

        symbol = word[0];

        field_tracker++;
      }

      else if(st == "RRTY"){
        if(has_rrty){
          invalid = 1;
          break;
        }
        has_rrty = true;
        //getline(fin,word);
        fin>>word;
        if(deschecker(word)){
          invalid = 1;
          break;
        }
        rarity = atoi(word.c_str());
        fin.get(); // eating newline after
        field_tracker++;
      }
      else{
        invalid = 1;
        break;
      }

    fin>>st;
  }
if(field_tracker!=9)
  invalid = 1;
return invalid;
}

int fileparser(char* filename)
{
  ifstream fin(filename);
 string st,word,header;
 string colorlist[] = {"BLACK","RED","GREEN","YELLOW","BLUE","MAGENTA","CYAN","WHITE"};

 getline(fin,st);

 if(st != "RLG327 MONSTER DESCRIPTION 1"){
  cout<<" Not a Valid File!! "<<endl;
  fin.close();
  return 0;
 }

 cout<<endl;

 vector<mondesc> mons;

 do {
   fin>>st;
   //read first word after rlg327...
   //Should be BEGIN
   if(st=="BEGIN" && PEEK!=EOF){

     mondesc mon;

     header = st;
     getline(fin,word);
     //should be Monster
     header += word;

     if(!(mon.parsemon(fin))){
       mons.push_back(mon);
     }

   }
 } while(PEEK != EOF); // or (st != NULL) or (st)

 vector<mondesc> :: iterator itmon;
 // diplay the values....header has begin monster in it, should display END manually
 for(itmon = mons.begin(); itmon != mons.end(); itmon++)
 {
   cout<<itmon->name<<endl;
   cout<<itmon->desc<<endl;
   vector<int> :: iterator itcolor;
   for(itcolor = (itmon->colors).begin(); itcolor != (itmon->colors).end(); itcolor++)
   {
     cout<<colorlist[*itcolor]<<" ";
   }
   cout<<endl;

   // Uncomment if you want to see the Abilities being printed from string vector
   // So if file has say UNIQ twice, it'll print out twice just like input file
   /*
   cout<<"ABIL";
   vector<string> :: iterator itabil;
   for(itabil = (itmon->abilitiestring).begin(); itabil != (itmon->abilitiestring).end(); itabil++)
   {
     cout<<" "<<*itabil;
   }
   cout<<endl;
   // prints out integer ability
   cout<<"ACTUAL int ABIL "<<itmon->abilities<<endl;
   */

   //cout<<"SPEED ";
   (itmon->speed).dprint();

   //cout<<"ABIL";
   if((itmon->abilities & NPC_SMART) == NPC_SMART){
     cout<<"SMART ";
   }
   if((itmon->abilities & NPC_TELE) == NPC_TELE){
     cout<<"TELE ";
   }
   if((itmon->abilities & NPC_TUNNEL) == NPC_TUNNEL){
     cout<<"TUNNEL ";
   }
   if((itmon->abilities & NPC_ERRATIC) == NPC_ERRATIC){
     cout<<"ERRATIC ";
   }
   if((itmon->abilities & NPC_PASS) == NPC_PASS){
     cout<<"PASS ";
   }
   if((itmon->abilities & NPC_UNIQ) == NPC_UNIQ){
     cout<<"UNIQ ";
   }
   if((itmon->abilities & NPC_PICKUP) == NPC_PICKUP){
     cout<<"PICKUP ";
   }
   if((itmon->abilities & NPC_DESTROY) == NPC_DESTROY){
     cout<<"DESTROY ";
   }
   if((itmon->abilities & NPC_BOSS) == NPC_BOSS){
     cout<<"BOSS ";
   }
   cout<<endl;

   //cout<<"HP ";
   (itmon->hitpoints).dprint();
   //cout<<"DAM ";
   (itmon->damage).dprint();

   cout<<itmon->symbol<<endl;

   cout<<itmon->rarity<<"\n"<<endl;

 }

 fin.close();
 return 0 ;
}
