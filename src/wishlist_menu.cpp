#include "wishlist_menu.h"

#include "bn_regular_bg_items_overlay.h"
#include "bn_sprite_items_cursor.h"
#include "bn_string.h"
#include "wishlist_data.h"
#include "wishlist_logic.h"

namespace {
constexpr int MENU_VISIBLE_Y = 0;
constexpr int MENU_HIDDEN_Y = -100;
constexpr int MENU_SLIDE_SPEED = 8;

bn::fixed_point get_cursor_pos(int index, int menu_y) {
  return bn::fixed_point(12, -61 + index * 12 + menu_y);
}

void redraw_wishlist(bn::sprite_text_generator& text_generator,
                     bn::vector<bn::sprite_ptr, 60>& text_sprites,
                     const bn::vector<bool, 16>& purchased, int menu_y) {
  text_sprites.clear();
  text_generator.set_left_alignment();
  text_generator.generate(20, -72 + menu_y, "To Buy", text_sprites);
  const int item_count = wishlist::item_count();
  for (int i = 0; i < item_count; ++i) {
    text_generator.generate(20, -60 + i * 12 + menu_y, wishlist::item_name(i),
                            text_sprites);
  }

  text_generator.set_right_alignment();
  text_generator.generate(112, -72 + menu_y, "$", text_sprites);

  for (int i = 0; i < item_count; ++i) {
    if (purchased.at(i)) {
      text_generator.generate(116, -60 + i * 12 + menu_y, "--", text_sprites);
    } else {
      text_generator.generate(116, -60 + i * 12 + menu_y,
                              bn::to_string<8>(wishlist::base_price(i)),
                              text_sprites);
    }
  }
}
}  // namespace

WishlistMenu::WishlistMenu(bn::sprite_text_generator& text_generator)
    : menu_text_generator(text_generator),
      menu_background(bn::regular_bg_items::overlay.create_bg(0, 0)),
      cursor(bn::sprite_items::cursor.create_sprite(
          get_cursor_pos(0, MENU_HIDDEN_Y))),
      menu_y(MENU_HIDDEN_Y),
      cursor_index(0),
      cursor_x_offset(0),
      state(State::hidden) {
  menu_background.set_priority(1);
  menu_background.set_visible(false);
  menu_background.set_y(MENU_HIDDEN_Y);

  cursor.set_bg_priority(0);
  cursor.set_visible(false);
}

void WishlistMenu::show(const bn::vector<bool, 16>& purchased) {
  menu_y = MENU_HIDDEN_Y;
  cursor_index = wishlist::logic::first_unpurchased_index(purchased);

  if (cursor_index < 0 || cursor_index >= wishlist::item_count()) {
    cursor.set_visible(false);
  } else {
    cursor.set_visible(true);
  }
  cursor_x_offset = 0;
  menu_background.set_y(menu_y);
  menu_background.set_visible(true);
  update_cursor_position();
  redraw_wishlist(menu_text_generator, text_sprites, purchased, menu_y);
  state = State::opening;
}

void WishlistMenu::hide() {
  if (state != State::hidden) {
    state = State::closing;
  }
}

void WishlistMenu::update(const bn::vector<bool, 16>& purchased) {
  if (state == State::hidden || !menu_background.visible()) {
    return;
  }

  if (state == State::opening) {
    menu_y += MENU_SLIDE_SPEED;
    if (menu_y > MENU_VISIBLE_Y) {
      menu_y = MENU_VISIBLE_Y;
      state = State::open;
    }
    menu_background.set_y(menu_y);
    redraw_wishlist(menu_text_generator, text_sprites, purchased, menu_y);
  } else if (state == State::closing) {
    menu_y -= MENU_SLIDE_SPEED;
    if (menu_y < MENU_HIDDEN_Y) {
      menu_y = MENU_HIDDEN_Y;
      state = State::hidden;
      menu_background.set_visible(false);
      cursor.set_visible(false);
      text_sprites.clear();
      cursor_x_offset = 0;
    }

    menu_background.set_y(menu_y);
    if (state != State::hidden) {
      redraw_wishlist(menu_text_generator, text_sprites, purchased, menu_y);
    }
  }

  update_cursor_position();
}

void WishlistMenu::move_cursor(int delta,
                               const bn::vector<bool, 16>& purchased) {
  if (cursor_index < 0) {
    return;
  }

  cursor_index = move_cursor_available(cursor_index, delta, purchased);
  update_cursor_position();
}

void WishlistMenu::set_cursor_x_offset(int x_offset) {
  cursor_x_offset = x_offset;
  update_cursor_position();
}

void WishlistMenu::refresh_text(const bn::vector<bool, 16>& purchased) {
  if (menu_background.visible()) {
    redraw_wishlist(menu_text_generator, text_sprites, purchased, menu_y);
  }
}

bool WishlistMenu::is_focused() const {
  return state == State::open || state == State::opening;
}

int WishlistMenu::cursor_index_value() const { return cursor_index; }

// This is kept as a separate function to allow unit testing in the future.
int WishlistMenu::move_cursor_available(int current_index, int delta,
                                        const bn::vector<bool, 16>& purchased) {
  const int original_index = current_index;
  int idx = current_index + delta;

  while (idx >= 0 && idx < int(purchased.size()) && purchased[idx]) {
    idx += delta;
  }

  if (idx < 0 || idx >= int(purchased.size())) {
    return original_index;
  }

  return idx;
}

void WishlistMenu::update_cursor_position() {
  bn::fixed_point base_pos = get_cursor_pos(cursor_index, menu_y);
  cursor.set_position(
      bn::fixed_point(base_pos.x() + cursor_x_offset, base_pos.y()));
}
