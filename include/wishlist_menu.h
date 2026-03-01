#ifndef WISHLIST_MENU_H
#define WISHLIST_MENU_H

#include "bn_regular_bg_ptr.h"
#include "bn_sprite_ptr.h"
#include "bn_sprite_text_generator.h"
#include "bn_vector.h"

class WishlistMenu {
 public:
  enum class State {
    hidden,
    opening,
    open,
    closing,
  };

  explicit WishlistMenu(bn::sprite_text_generator& text_generator);

  void show(const bn::vector<int, 16>& prices, int index);
  void hide();
  void update(const bn::vector<int, 16>& prices);
  void set_cursor_index(int index);
  void set_cursor_x_offset(int x_offset);
  void refresh_text(const bn::vector<int, 16>& prices);

  [[nodiscard]] bool fully_open() const;
  [[nodiscard]] bool hidden() const;

 private:
  void update_cursor_position();

  bn::sprite_text_generator& menu_text_generator;
  bn::vector<bn::sprite_ptr, 60> text_sprites;
  bn::regular_bg_ptr menu_background;
  bn::sprite_ptr cursor;
  int menu_y;
  int cursor_index;
  int cursor_x_offset;
  State state;
};

#endif
