#include "wishlist_data.h"

#include "bn_sprite_items_bonsai.h"
#include "bn_sprite_items_cactus1.h"
#include "bn_sprite_items_clock.h"
#include "bn_sprite_items_cookies.h"
#include "bn_sprite_items_painting.h"
#include "bn_sprite_items_sylvester.h"
#include "bn_sprite_items_topiary.h"
#include "bn_sprite_items_typist.h"
#include "bn_sprite_items_vines.h"

namespace wishlist {
namespace {

const ItemData items[] = {
    {"Clock", 30, bn::fixed_point(-18, -18), &bn::sprite_items::clock},
    {"Cookies", 15, bn::fixed_point(-55, 9), &bn::sprite_items::cookies},
    {"Bonsai", 70, bn::fixed_point(-86, -23), &bn::sprite_items::bonsai},
    {"Vines", 20, bn::fixed_point(-70, -19), &bn::sprite_items::vines},
    {"Topiary", 40, bn::fixed_point(112, 48), &bn::sprite_items::topiary},
    {"Art", 55, bn::fixed_point(-53, -8), &bn::sprite_items::painting},
    {"Cactus", 22, bn::fixed_point(-116, 19), &bn::sprite_items::cactus1},
    {"Kitty", 100, bn::fixed_point(-12, 23), &bn::sprite_items::sylvester},
    {"Wi-fi", 125, bn::fixed_point(-35, 42), &bn::sprite_items::typist},
};

}  // namespace

const ItemData& item(int index) { return items[index]; }

const char* item_name(int index) { return item(index).name; }

int item_count() { return ITEM_COUNT; }

void initialize_prices(bn::vector<int, 16>& prices) {
  prices.clear();
  for (int i = 0; i < ITEM_COUNT; ++i) {
    prices.push_back(items[i].price);
  }
}

void initialize_upgrades(bn::vector<bn::sprite_ptr, 16>& upgrades) {
  upgrades.clear();
  for (int i = 0; i < ITEM_COUNT; ++i) {
    upgrades.push_back(items[i].sprite_item->create_sprite(items[i].pos));
  }
}

}  // namespace wishlist