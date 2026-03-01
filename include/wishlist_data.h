#ifndef WISHLIST_DATA_H
#define WISHLIST_DATA_H

#include "bn_fixed_point.h"
#include "bn_sprite_item.h"
#include "bn_sprite_ptr.h"
#include "bn_vector.h"

namespace wishlist {

enum ItemIndex {
  CLOCK = 0,
  COOKIES = 1,
  BONSAI = 2,
  VINES = 3,
  TOPIARY = 4,
  ART = 5,
  CACTUS = 6,
  KITTY = 7,
  WIFI = 8,
  ITEM_COUNT = 9,
};

struct ItemData {
  const char* name;
  int price;
  bn::fixed_point pos;
  const bn::sprite_item* sprite_item;
};

const ItemData& item(int index);
const char* item_name(int index);
int item_count();

void initialize_prices(bn::vector<int, 16>& prices);
void initialize_upgrades(bn::vector<bn::sprite_ptr, 16>& upgrades);

}  // namespace wishlist

#endif