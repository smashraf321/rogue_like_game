
# include "utils.h"
# include "objectclass.h"
# include "dungeon.h"

# define MAINFIRSTTIME 1
# define STAIRSUPNDOWN 2

static uint32_t max_object_cells(dungeon *d)
{
  uint32_t i;
  uint32_t sum;

  for (i = sum = 0; i < d->num_rooms; i++) {
    sum += d->rooms[i].size[dim_y] * d->rooms[i].size[dim_x];
  }

  return sum;
}

void obj_factory(dungeon *d, int where_from)
{
  uint32_t i;
  object_class *o;
  uint32_t room;
  pair_t p;
  uint32_t num_cells;
  uint32_t num_objects;
  uint32_t num_poss_objs;
  uint32_t obj_desc_size = d->object_descriptions.size();
  num_cells = max_object_cells(d);
  num_poss_objs = 10 < obj_desc_size ? obj_desc_size : 10;
  num_objects = num_poss_objs < num_cells ? num_poss_objs : num_cells;

  for (i = 0; i < num_objects; i++) {
    int index = rand()%obj_desc_size;
    uint32_t rrty = (rand()%100);
    bool excd_rrty = rrty > d->object_descriptions[index].rarity;
    bool uniq = d->object_descriptions[index].artifact;

    //rarity n uniqueness check
    if(excd_rrty){
      i--;
      continue;
    }/*
    if(uniq && d->object_descriptions[index].uniqueness){
      i--;
      continue;
    }*/
    if(uniq && d->object_descriptions[index].uniqueness && where_from == MAINFIRSTTIME){
      i--;
      continue;
    }
    if(uniq && where_from == STAIRSUPNDOWN && d->object_descriptions[index].uniqueness && !d->object_descriptions[index].picked_up){
      i--;
      continue;
    }
    
    //make changes here to incorporate checking
    // if alive from object_descriptions vector
    //for alive from
    o = new object_class;

    do {
      room = rand_range(0, d->num_rooms - 1);
      p[dim_y] = rand_range(d->rooms[room].position[dim_y],
                            (d->rooms[room].position[dim_y] +
                             d->rooms[room].size[dim_y] - 1));
      p[dim_x] = rand_range(d->rooms[room].position[dim_x],
                            (d->rooms[room].position[dim_x] +
                             d->rooms[room].size[dim_x] - 1));
    } while (d->object_map[p[dim_y]][p[dim_x]]);

    o->position[dim_y] = p[dim_y];
    o->position[dim_x] = p[dim_x];
    d->object_map[p[dim_y]][p[dim_x]] = o;
    o->picked_up = false;

    o->name = d->object_descriptions[index].get_name();
    o->desc = d->object_descriptions[index].get_description();
    o->color = d->object_descriptions[index].get_color();
    o->type = d->object_descriptions[index].get_type();
    o->symbol = object_symbol[o->type];

    o->hit = d->object_descriptions[index].get_hit().roll();
    o->dodge = d->object_descriptions[index].get_dodge().roll();
    o->defence = d->object_descriptions[index].get_defence().roll();
    o->weight = d->object_descriptions[index].get_weight().roll();
    o->speed = d->object_descriptions[index].get_speed().roll();
    o->attribute = d->object_descriptions[index].get_attribute().roll();
    o->value = d->object_descriptions[index].get_value().roll();
    o->damage = d->object_descriptions[index].get_damage();

    //o->artifact = d->object_descriptions[index].artifact;
    o->artifact = uniq;
    o->rarity = d->object_descriptions[index].rarity;
    d->object_descriptions[index].uniqueness = true;

    d->object_map[p[dim_y]][p[dim_x]] = o;

  }

}
