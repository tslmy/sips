/**
 * @file main.cpp
 * @brief Game Boy Advance entry point and primary loop for the game.
 *
 * Sets up graphics, music, state, and contains the central per-frame loop.
 * Most game objects, UI, and upgrades managed here.
 */

#include "bn_blending.h"
#include "bn_core.h"
#include "bn_deque.h"
#include "bn_display.h"
#include "bn_keypad.h"
#include "bn_log.h"
#include "bn_music.h"
#include "bn_music_items.h"
#include "bn_random.h"
#include "bn_regular_bg_items_bg1.h"
#include "bn_regular_bg_ptr.h"
#include "bn_sound_items.h"
#include "bn_sprite_animate_actions.h"
#include "bn_sprite_items_barista.h"
#include "bn_sprite_items_blocker.h"
#include "bn_sprite_items_bonsai.h"
#include "bn_sprite_items_cactus1.h"
#include "bn_sprite_items_cash.h"
#include "bn_sprite_items_clock.h"
#include "bn_sprite_items_cookies.h"
#include "bn_sprite_items_drinker.h"
#include "bn_sprite_items_painting.h"
#include "bn_sprite_items_pigeon.h"
#include "bn_sprite_items_pigeon2.h"
#include "bn_sprite_items_popularity_bar.h"
#include "bn_sprite_items_reflect.h"
#include "bn_sprite_items_rug1.h"
#include "bn_sprite_items_steam.h"
#include "bn_sprite_items_swallow.h"
#include "bn_sprite_items_sylvester.h"
#include "bn_sprite_items_talkative.h"
#include "bn_sprite_items_till.h"
#include "bn_sprite_items_title.h"
#include "bn_sprite_items_topiary.h"
#include "bn_sprite_items_twinkle.h"
#include "bn_sprite_items_typist.h"
#include "bn_sprite_items_vines.h"
#include "bn_sprite_items_waiting.h"
#include "bn_sprite_palette_items_black_text_palette.h"
#include "bn_sprite_palette_items_white_text_palette.h"
#include "bn_sprite_palette_ptr.h"
#include "bn_sprite_text_generator.h"
#include "bn_string.h"
#include "ti_font.h"
#include "ti_person.h"
#include "wishlist_data.h"
#include "wishlist_logic.h"
#include "wishlist_menu.h"

namespace {
// Return true with probability numerator/denominator using provided RNG.
// numerator: number of successful outcomes; denominator: total outcomes.
inline bool chance(bn::random &rng, int numerator, int denominator = 100) {
  if (denominator <= 0) return false;
  if (numerator <= 0) return false;
  if (numerator >= denominator) return true;
  return rng.get_int(denominator) < numerator;
}

template <typename Action>
inline void update_if_running(Action &action) {
  if (!action.done()) {
    action.update();
  }
}

void initialize_people(bn::vector<ti::Person, 16> &people) {
  people.clear();
  for (int i = 0; i < 10; ++i) {
    const ti::START start = i % 2 == 0 ? ti::START::RIGHT : ti::START::LEFT;
    people.push_back(ti::Person(start, ti::TYPE::GREEN_SHIRT, i));
  }
}

void rebuild_available_types(const bn::vector<ti::Person, 16> &people,
                             int popularity_level, bn::vector<int, 16> &types) {
  types.clear();
  for (int i = 0; i < 14; ++i) {
    types.push_back(i);
  }

  const int active_people = bn::min(popularity_level, people.size());
  for (int i = 0; i < active_people; ++i) {
    const int used_type = int(people.at(i).get_type());
    for (int j = 0; j < types.size(); ++j) {
      if (types.at(j) == used_type) {
        types.erase(types.begin() + j);
        break;
      }
    }
  }
}
}  // namespace

int main() {
  bn::core::init();

  bn::sprite_text_generator text_generator(ti::variable_8x8_sprite_font);
  WishlistMenu wishlist_menu(text_generator);
  bn::vector<bn::sprite_ptr, 4> cash_text_sprites;
  text_generator.set_bg_priority(0);

  int cash = 535;
  int popularity_level = 1;
  bn::sprite_ptr popularity_bar =
      bn::sprite_items::popularity_bar.create_sprite(-79, -73,
                                                     popularity_level);

  // Cursor shake state
  int cursor_shake_frames_remaining = 0;
  int cursor_shake_direction = 1;

  bn::vector<bn::sprite_ptr, 16> upgrades;
  bn::vector<bool, 16> purchased;
  wishlist::initialize_purchased(purchased);
  wishlist::initialize_upgrades(upgrades);
  bn::vector<bn::sprite_ptr, 8> popularity_bonuses;

  for (bn::sprite_ptr sprite : upgrades) {
    sprite.set_visible(false);
  }
  upgrades.at(wishlist::WIFI).set_z_order(-40);

  bn::music_items::wild_strawberry.play();
  bn::music::set_volume(1);

  bn::sound_items::bustle.play(0.1);
  int bustle_timer = 0;

  // map
  bn::regular_bg_ptr map = bn::regular_bg_items::bg1.create_bg(0, 0);

  // sprite
  bn::sprite_ptr title = bn::sprite_items::title.create_sprite(16, -22);
  bn::sprite_ptr steam = bn::sprite_items::steam.create_sprite(-70, -8);
  steam.set_blending_enabled(true);
  bn::blending::set_transparency_alpha(0.5);
  bn::sprite_ptr barista = bn::sprite_items::barista.create_sprite(-97, -5);
  bn::sprite_ptr drinker = bn::sprite_items::drinker.create_sprite(-105, 44);
  bn::sprite_ptr talkative = bn::sprite_items::talkative.create_sprite(-83, 42);
  bn::sprite_ptr pigeon = bn::sprite_items::pigeon.create_sprite(46, -71);
  bn::sprite_ptr pigeon2 = bn::sprite_items::pigeon2.create_sprite(60, -71);
  pigeon2.set_horizontal_flip(true);
  bn::sprite_ptr till = bn::sprite_items::till.create_sprite(-65, 3);
  bn::sprite_ptr cash_sprite = bn::sprite_items::cash.create_sprite(-65, -7);
  cash_sprite.set_visible(false);
  bn::sprite_ptr twinkle = bn::sprite_items::twinkle.create_sprite(-65, -7);
  twinkle.set_visible(false);

  // TODO: Add Swallow mascot sprite and animation as ambient character effect.

  bn::sprite_ptr reflect1 = bn::sprite_items::reflect.create_sprite(5, 20);
  reflect1.set_blending_enabled(true);
  reflect1.set_z_order(-500);
  // TODO: Add second reflection visual effect (reflect2) for extra window
  // polish.

  bn::sprite_ptr blocker = bn::sprite_items::blocker.create_sprite(58, 42);
  blocker.set_z_order(-40);

  int timer = 40;
  bn::random rng = bn::random();

  // animation action
  bn::sprite_animate_action<7> steamAction =
      bn::create_sprite_animate_action_once(
          steam, 5, bn::sprite_items::steam.tiles_item(), 6, 6, 6, 6, 6, 6, 6);
  steam.set_visible(false);
  bn::sprite_animate_action<5> drinkerAction =
      bn::create_sprite_animate_action_once(
          drinker, 15, bn::sprite_items::drinker.tiles_item(), 0, 0, 0, 0, 0);
  bn::sprite_animate_action<10> sylvesterAction =
      bn::create_sprite_animate_action_forever(
          upgrades.at(wishlist::KITTY), 18,
          bn::sprite_items::sylvester.tiles_item(), 0, 1, 2, 3, 4, 5, 6, 7, 8,
          9);

  bn::sprite_animate_action<5> pigeonAction =
      bn::create_sprite_animate_action_once(
          pigeon, 18, bn::sprite_items::pigeon.tiles_item(), 0, 1, 0, 1, 0);
  bn::sprite_animate_action<5> pigeon2Action =
      bn::create_sprite_animate_action_once(
          pigeon2, 18, bn::sprite_items::pigeon2.tiles_item(), 0, 1, 0, 1, 0);

  bn::sprite_animate_action<15> reflectAction1 =
      bn::create_sprite_animate_action_once(
          reflect1, 4, bn::sprite_items::reflect.tiles_item(), 0, 1, 2, 3, 4, 5,
          6, 7, 8, 9, 10, 11, 12, 13, 14);

  bn::sprite_animate_action<11> twinkle_action =
      bn::create_sprite_animate_action_once(
          twinkle, 6, bn::sprite_items::twinkle.tiles_item(), 0, 1, 2, 3, 4, 5,
          6, 7, 8, 9, 10);

  // TODO: Implement reflectAction2 for a possible extra animation layer.

  // TODO: Add swallow mascot animation loop here if feature is enabled.

  bn::sprite_animate_action<2> typistAction =
      bn::create_sprite_animate_action_forever(
          upgrades.at(wishlist::WIFI), 8, bn::sprite_items::typist.tiles_item(),
          0, 1);

  bn::sprite_animate_action<12> clockAction =
      bn::create_sprite_animate_action_forever(
          upgrades.at(wishlist::CLOCK), 300,
          bn::sprite_items::clock.tiles_item(), 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
          10, 11);

  bn::deque<int, 8> order_queue = {};
  bool waiting_spot = false;
  bool purchased_this_frame = false;

  bn::vector<ti::Person, 16> people;
  // Pointer to the currently focused person (if any)
  ti::Person *focused_person = nullptr;
  bn::vector<int, 16> available_types;
  initialize_people(people);

  bn::vector<bn::sprite_ptr, 8> focus_summary_sprites;
  while (true) {
    wishlist_menu.update(purchased);

    if (wishlist_menu.is_focused()) {
      if (bn::keypad::up_pressed()) {
        wishlist_menu.move_cursor(-1, purchased);
      }
      if (bn::keypad::down_pressed()) {
        wishlist_menu.move_cursor(+1, purchased);
      }

      const int cursor_index = wishlist_menu.cursor_index_value();

      // Cursor shake effect
      if (cursor_shake_frames_remaining > 0) {
        wishlist_menu.set_cursor_x_offset(cursor_shake_direction * 2);
        cursor_shake_frames_remaining--;
        cursor_shake_direction *= -1;
        if (cursor_shake_frames_remaining == 0) {
          wishlist_menu.set_cursor_x_offset(0);
        }
      } else {
        wishlist_menu.set_cursor_x_offset(0);
      }

      if (bn::keypad::a_pressed() && cursor_index >= 0) {
        const bool is_purchased = purchased.at(cursor_index);
        const int base_price = wishlist::base_price(cursor_index);
        const wishlist::logic::PurchaseAttempt purchase_attempt =
            wishlist::logic::attempt_purchase(is_purchased, base_price, cash,
                                              popularity_level);

        if (purchase_attempt.outcome ==
            wishlist::logic::PurchaseOutcome::purchased) {
          cash = purchase_attempt.cash_after;
          upgrades.at(cursor_index)
              .set_visible(!upgrades.at(cursor_index).visible());
          purchased.at(cursor_index) = purchase_attempt.purchased_after;
          wishlist_menu.refresh_text(purchased);
          popularity_level = purchase_attempt.popularity_after;
          popularity_bar.set_item(bn::sprite_items::popularity_bar,
                                  popularity_level);
          if (purchase_attempt.should_close_menu) {
            wishlist_menu.hide();
          }
          twinkle.set_position(upgrades.at(cursor_index).position());
          twinkle.set_visible(true);
          bn::sound_items::sparkle.play(0.8);
          twinkle_action = bn::create_sprite_animate_action_once(
              twinkle, 6, bn::sprite_items::twinkle.tiles_item(), 0, 1, 2, 3, 4,
              5, 6, 7, 8, 9, 10);
        } else if (purchase_attempt.should_start_cursor_shake) {
          cursor_shake_frames_remaining = 10;
          cursor_shake_direction = 1;
          bn::sound_items::cancel.play(1.0);
        }
      }
    }

    else {
      if (bn::keypad::a_pressed()) {
        wishlist_menu.show(purchased);
      }
    }

    if (bn::keypad::b_pressed()) {
      wishlist_menu.hide();
    }

    cash_text_sprites.clear();
    text_generator.set_palette_item(
        bn::sprite_palette_items::white_text_palette);
    text_generator.set_right_alignment();
    text_generator.generate(-21, -71, "$" + bn::to_string<8>(cash),
                            cash_text_sprites);
    text_generator.set_palette_item(
        bn::sprite_palette_items::black_text_palette);

    if (bustle_timer > 60 * 29) {
      bustle_timer = 0;
      bn::sound_items::bustle.play(0.1 + bn::fixed(popularity_level) / 20);
    } else {
      bustle_timer = bustle_timer + 1;
    }
    timer = timer - 1;
    if (timer < 0) {
      if (chance(rng, 39)) {
        barista.set_item(bn::sprite_items::barista, rng.get_int(5));
      }
      if (chance(rng, 39)) {
        till.set_item(bn::sprite_items::till, rng.get_int(3));
      }
      if (chance(rng, 7)) {
        if (steamAction.done()) {
          bn::sound_items::steam.play(0.6);
          steamAction = bn::create_sprite_animate_action_once(
              steam, 5, bn::sprite_items::steam.tiles_item(), 0, 1, 2, 3, 4, 5,
              6);
          steam.set_visible(true);
        }
      }
      if (chance(rng, 9)) {
        if (drinkerAction.done()) {
          drinkerAction = bn::create_sprite_animate_action_once(
              drinker, 15, bn::sprite_items::drinker.tiles_item(), 0, 1, 2, 1,
              0);
        }
      }
      if (chance(rng, 90)) {
        talkative.set_item(bn::sprite_items::talkative, rng.get_int(4));
      }

      if (typistAction.done()) {
        if (chance(rng, 19)) {
          typistAction = bn::create_sprite_animate_action_forever(
              upgrades.at(wishlist::WIFI), 8,
              bn::sprite_items::typist.tiles_item(), 0, 1);
        }
      } else {
        if (chance(rng, 19)) {
          typistAction = bn::create_sprite_animate_action_once(
              upgrades.at(wishlist::WIFI), 8,
              bn::sprite_items::typist.tiles_item(), 2, 2);
        }
      }

      if (chance(rng, 19)) {
        if (pigeonAction.done()) {
          pigeonAction = bn::create_sprite_animate_action_once(
              pigeon, 15, bn::sprite_items::pigeon.tiles_item(), 0, 1, 0, 1, 0);
        }
      }
      if (chance(rng, 20)) {
        if (pigeon2Action.done()) {
          pigeon2Action = bn::create_sprite_animate_action_once(
              pigeon2, 15, bn::sprite_items::pigeon2.tiles_item(), 0, 1, 0, 1,
              0);
        }
      }
      // TODO: Swallow mascot random jump logic (uncomment if swallow is
      // re-enabled)

      timer = 30;
    }

    // TODO: Swallow mascot movement logic (uncomment if swallow is re-enabled)

    update_if_running(twinkle_action);
    update_if_running(steamAction);
    update_if_running(drinkerAction);
    if (!reflectAction1.done()) {
      reflectAction1.update();
      // TODO: Update reflectAction2 animation if feature is added.
    } else {
      if (chance(rng, 1, 1000)) {
        reflectAction1 = bn::create_sprite_animate_action_once(
            reflect1, 4, bn::sprite_items::reflect.tiles_item(), 0, 1, 2, 3, 4,
            5, 6, 7, 8, 9, 10, 11, 12, 13, 14);
        // TODO: (Optional) Restart reflectAction2 as part of polish animation.
      }
    }
    update_if_running(pigeonAction);
    update_if_running(pigeon2Action);
    // TODO: Update swallow animation if mascot feature enabled.
    sylvesterAction.update();
    if (!typistAction.done()) {
      typistAction.update();
    }
    rebuild_available_types(people, popularity_level, available_types);
    // Handle L+R input for focus selection/deselection
    bool lr_pressed = bn::keypad::l_pressed() && bn::keypad::r_pressed();
    if (lr_pressed) {
      if (focused_person && focused_person->is_focused()) {
        // Deselect current focus
        focused_person->set_focused(false);
        focused_person = nullptr;
      } else {
        // Select first visible customer
        for (int i = 0; i < people.size(); i++) {
          if (popularity_level > i && people.at(i).is_visible()) {
            focused_person = &people.at(i);
            focused_person->set_focused(true);
            break;
          }
        }
      }
    }

    // Handle L/R cycling when focused
    if (focused_person && focused_person->is_focused()) {
      int focused_index = -1;
      for (int i = 0; i < people.size(); i++) {
        if (&people.at(i) == focused_person) {
          focused_index = i;
          break;
        }
      }
      // Find all visible customers
      bn::vector<int, 16> visible_indices;
      for (int i = 0; i < people.size(); i++) {
        if (popularity_level > i && people.at(i).is_visible()) {
          visible_indices.push_back(i);
        }
      }
      if (!visible_indices.empty()) {
        int current_visible = -1;
        for (int i = 0; i < visible_indices.size(); i++) {
          if (visible_indices.at(i) == focused_index) {
            current_visible = i;
            break;
          }
        }
        // L cycles left, R cycles right
        if (bn::keypad::l_pressed() && !bn::keypad::r_pressed()) {
          int prev = (current_visible - 1 + visible_indices.size()) %
                     visible_indices.size();
          if (prev != current_visible) {
            focused_person->set_focused(false);
            focused_person = &people.at(visible_indices.at(prev));
            focused_person->set_focused(true);
          }
        } else if (bn::keypad::r_pressed() && !bn::keypad::l_pressed()) {
          int next = (current_visible + 1) % visible_indices.size();
          if (next != current_visible) {
            focused_person->set_focused(false);
            focused_person = &people.at(visible_indices.at(next));
            focused_person->set_focused(true);
          }
        }
      }
    }

    for (int i = 0; i < people.size(); i++) {
      if (popularity_level > i) {
        people.at(i).update(order_queue, waiting_spot, purchased_this_frame,
                            available_types, people);
        // If we have a focused person pointer, check if it is still valid
        if (focused_person && (!focused_person->is_focused())) {
          focused_person = nullptr;
        }
      }
    }
    clockAction.update();

    if (cash_sprite.visible()) {
      cash_sprite.set_y(cash_sprite.y() - 0.2);
      if (cash_sprite.y() < -14) {
        cash_sprite.set_visible(false);
        cash_sprite.set_y(-7);
      }
    }

    if (purchased_this_frame) {
      cash += rng.get_int(3) + 3;
      cash_sprite.set_visible(true);
      bn::sound_items::cash.play(0.8);
      purchased_this_frame = false;
    }
    // Render focused customer summary at bottom of screen
    focus_summary_sprites.clear();
    if (focused_person && focused_person->is_focused()) {
      const char *state_str =
          ti::Person::state_to_string(focused_person->get_state());
      text_generator.set_left_alignment();
      text_generator.set_palette_item(
          bn::sprite_palette_items::white_text_palette);
      text_generator.generate(-110, 68, bn::string<32>("State: ") + state_str,
                              focus_summary_sprites);
    }
    bn::core::update();
    (void)rng.get();
  }
}
