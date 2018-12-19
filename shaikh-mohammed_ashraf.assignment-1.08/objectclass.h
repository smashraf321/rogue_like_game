#ifndef OBJECTCLASS_H
# define OBJECTCLASS_H

# include "dims.h"
//# include <cstring>
# include "descriptions.h"
/*
typedef enum object_type {
  objtype_no_type,
  objtype_WEAPON,
  objtype_OFFHAND,
  objtype_RANGED,
  objtype_LIGHT,
  objtype_ARMOR,
  objtype_HELMET,
  objtype_CLOAK,
  objtype_GLOVES,
  objtype_BOOTS,
  objtype_AMULET,
  objtype_RING,
  objtype_SCROLL,
  objtype_BOOK,
  objtype_FLASK,
  objtype_GOLD,
  objtype_AMMUNITION,
  objtype_FOOD,
  objtype_WAND,
  objtype_CONTAINER
} object_type_t;
*/

class object_class{
public:
  std::string name, desc;
  char symbol;
  object_type_t type;
  uint32_t color;
  pair_t position;
  dice damage;
  uint32_t hit, dodge, defence, weight, speed, attribute, value;
  bool artifact;
  uint32_t rarity;
  bool picked_up;
};

void obj_factory(dungeon *d, int where_from);

#endif
