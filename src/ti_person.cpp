/**
 * @file ti_person.cpp
 * @brief Implements the Person class state/behavior for customers in the jam
 * cafe game.
 *
 * Contains core state machine logic, sprite animation switching, queue/ordering
 * workflow, and all per-customer lifecycle management for the main gameplay
 * flow.
 *
 * Agent/Contributor notes:
 *   - This file is tightly coupled with the game balance and overall tempo.
 *   - Magic numbers and queue-related quirks are a legacy from the rapid game
 * jam build.
 *   - If making changes, test thoroughly in-emulator, as small tweaks have
 * game-wide effects!
 */

#include "ti_person.h"

#include "bn_log.h"
#include "bn_math.h"
#include "bn_sprite_builder.h"
#include "bn_sprite_items_shadow.h"
#include "bn_sprite_items_walk1.h"
#include "bn_sprite_items_walk10.h"
#include "bn_sprite_items_walk11.h"
#include "bn_sprite_items_walk12.h"
#include "bn_sprite_items_walk13.h"
#include "bn_sprite_items_walk14.h"
#include "bn_sprite_items_walk2.h"
#include "bn_sprite_items_walk3.h"
#include "bn_sprite_items_walk4.h"
#include "bn_sprite_items_walk5.h"
#include "bn_sprite_items_walk6.h"
#include "bn_sprite_items_walk7.h"
#include "bn_sprite_items_walk8.h"
#include "bn_sprite_items_walk9.h"
#include "ti_helpers.h"

/**
 * @brief Anonymous namespace: low-level helpers for sprite and movement.
 * - _create_sprite: Utility for building person sprites with z/horizontal
 * config.
 * - _create_shadow: Utility for shadow sprites with blending.
 * - get_next_step: See ti_helpers.h.
 */
namespace {
bn::sprite_ptr _create_sprite(bn::fixed_point position, bool is_left,
                              bn::sprite_item sprite) {
  bn::sprite_builder builder(sprite);
  builder.set_position(position);
  builder.set_z_order(-300);
  builder.set_horizontal_flip(is_left);
  return builder.release_build();
}

bn::sprite_ptr _create_shadow(bn::fixed_point position) {
  bn::sprite_builder builder(bn::sprite_items::shadow);
  builder.set_position(position);
  builder.set_z_order(-5);
  builder.set_blending_enabled(true);
  bn::blending::set_transparency_alpha(0.5);
  return builder.release_build();
}

}  // namespace

namespace ti {

namespace {
constexpr const bn::sprite_item* TYPE_TO_SPRITE[] = {
    &bn::sprite_items::walk1,   // GREEN_SHIRT = 0
    &bn::sprite_items::walk2,   // RED_SHIRT = 1
    &bn::sprite_items::walk3,   // BLUE_SHIRT = 2
    &bn::sprite_items::walk4,   // RED_SINGLET = 3
    &bn::sprite_items::walk6,   // DWIGHT = 4
    &bn::sprite_items::walk8,   // GIRL1 = 5
    &bn::sprite_items::walk7,   // GIRL2 = 6
    &bn::sprite_items::walk5,   // PALE_GREEN_SHIRT = 7
    &bn::sprite_items::walk9,   // GIRL3 = 8
    &bn::sprite_items::walk10,  // PERSON1 = 9
    &bn::sprite_items::walk11,  // PERSON2 = 10
    &bn::sprite_items::walk12,  // PERSON3 = 11
    &bn::sprite_items::walk13,  // PERSON4 = 12
    &bn::sprite_items::walk14,  // PERSON5 = 13
};
}  // namespace

// This array must match the STATE enum order exactly.
const Person::StateHandler Person::_state_handlers[] = {
    &Person::_handle_walking_left,
    &Person::_handle_walking_left_with_coffee,
    &Person::_handle_walking_right,
    &Person::_handle_walking_right_with_coffee,
    &Person::_handle_entering,
    &Person::_handle_walking_to_order,
    &Person::_handle_waiting_to_order,
    &Person::_handle_ordering,
    &Person::_handle_walking_to_counter,
    &Person::_handle_waiting,
    &Person::_handle_walking_to_door,
    &Person::_handle_exiting,
    &Person::_handle_joining_queue,
    &Person::_handle_walking_left_passer,
    &Person::_handle_walking_right_passer,
};

constexpr bool _ti_verify_state_handler_table() {
  static_assert(
      sizeof(Person::_state_handlers) / sizeof(Person::StateHandler) == 15,
      "State handler table must match STATE enum.");
  return true;
}

constexpr bool _ti_state_handler_table_verified =
    _ti_verify_state_handler_table();

namespace {

int locate_in_queue(const bn::deque<int, 8>& order_queue, int id) {
  for (int i = 0; i < order_queue.size(); ++i) {
    if (order_queue.at(i) == id) return i;
  }
  return -1;
}
}  // namespace

int Person::_active_loiterers = 0;

Person::Person(START start, TYPE type, int id)
    : _shadow(bn::sprite_items::shadow.create_sprite(0, 0)), _id(id) {
  LOCATIONS.push_back(bn::fixed_point(-60, 12));
  LOCATIONS.push_back(bn::fixed_point(-55, 15));
  LOCATIONS.push_back(bn::fixed_point(-50, 18));
  LOCATIONS.push_back(bn::fixed_point(-45, 21));
  LOCATIONS.push_back(bn::fixed_point(-40, 24));

  bn::random rng = bn::random();
  for (int i = 0; i < _id; i++) {
    rng.get();
  }
  _speed += rng.get_fixed(0.2) - 0.1;

  bn::fixed_point pos = bn::fixed_point(-160, 60);
  _state = STATE::WALKING_LEFT_W_COFFEE;
  if (start == START::RIGHT) {
    pos.set_x(160);
    _state = STATE::WALKING_RIGHT_W_COFFEE;
  } else if (start == START::COUNTER) {
    pos = bn::fixed_point(-86, 14);
    _state = STATE::WAITING;
  }

  setStyle(type, start, pos);

  if (start == START::RIGHT) {
    _sprite.value().set_horizontal_flip(true);
  } else if (start == START::COUNTER) {
    _sprite.value().set_horizontal_flip(true);
    _sprite.value().set_z_order(-100);
    _action = bn::create_sprite_animate_action_forever(
        _sprite.value(), 20, _sprite_item.value().tiles_item(), 16, 16, 16, 16,
        17, 17, 17, 17);
  } else {
    _sprite.value().set_horizontal_flip(false);
  }

  bn::fixed_point shadow_pos = _sprite.value().position();
  shadow_pos.set_y(shadow_pos.y() + 15);
  _shadow = _create_shadow(shadow_pos);
}

void Person::setStyle(TYPE type, START start, bn::fixed_point pos) {
  if (start != START::COUNTER) {
    pos.set_y(_randomized_street_y(pos.y()));
  }
  _type = type;
  _sprite_item = *TYPE_TO_SPRITE[static_cast<int>(type)];
  _sprite = _create_sprite(pos, start != START::RIGHT, _sprite_item.value());
  _has_loitered = false;
  _is_loitering = false;
  _loiter_time = 0;
  _loiter_duration_frames = 0;
  _loiter_in_position = false;
  _loiter_target_position = pos;
  _action = bn::create_sprite_animate_action_forever(
      _sprite.value(), 12, _sprite_item.value().tiles_item(), 0, 1, 2, 3, 4, 5,
      6, 7);
}

TYPE Person::get_type() { return _type; }

bn::fixed_point Person::_random_street_loiter_point() {
  bn::fixed_point current_pos = _sprite.value().position();
  bn::fixed target_x = current_pos.x();

  switch (_state) {
    case STATE::WALKING_LEFT:
      target_x = OUTSIDE.x();
      break;
    case STATE::WALKING_RIGHT:
      target_x = OUTSIDE.x();
      break;
    case STATE::WALKING_LEFT_W_COFFEE:
      target_x = LEFT.x();
      break;
    case STATE::WALKING_RIGHT_W_COFFEE:
      target_x = RIGHT.x();
      break;
    case STATE::WALKING_LEFT_PASSER:
      target_x = LEFT.x();
      break;
    case STATE::WALKING_RIGHT_PASSER:
      target_x = RIGHT.x();
      break;
    default:
      break;
  }

  bn::fixed min_x = target_x;
  bn::fixed max_x = current_pos.x();
  if (min_x > max_x) {
    bn::fixed temp = min_x;
    min_x = max_x;
    max_x = temp;
  }

  bn::fixed range = max_x - min_x;
  if (range <= 0) {
    return bn::fixed_point(current_pos.x(),
                           _randomized_street_y(current_pos.y()));
  }

  bn::fixed random_offset = _random.get_fixed(range);
  bn::fixed random_x = min_x + random_offset;
  return bn::fixed_point(random_x, _randomized_street_y(current_pos.y()));
}

bn::fixed Person::_randomized_street_y(bn::fixed base_y) {
  int offset = _random.get_int(21) - 10;
  return base_y + bn::fixed(offset);
}

bool Person::_should_walk_by() {
  if (_walk_by_chance <= 0) {
    return false;
  }
  return _random.get_int(_walk_by_chance) == 0;
}

bool Person::_try_start_loitering() {
  if (_has_loitered || _is_loitering || _active_loiterers >= _max_loiterers) {
    return false;
  }

  constexpr int LOITER_CHANCE_FRAMES = 360;  // ~6 seconds average between tries
  if (_random.get_int(LOITER_CHANCE_FRAMES) == 0) {
    _begin_loitering();
    return true;
  }

  return false;
}

void Person::_begin_loitering() {
  _has_loitered = true;
  _is_loitering = true;
  _loiter_time = 0;
  _loiter_duration_frames = (_random.get_int(9) + 2) * 60;
  _loiter_target_position = _random_street_loiter_point();
  _loiter_in_position = false;
  _active_loiterers++;
  bool already_at_target =
      _sprite.value().position().x() == _loiter_target_position.x() &&
      _sprite.value().position().y() == _loiter_target_position.y();
  if (already_at_target) {
    _loiter_in_position = true;
    _action = bn::create_sprite_animate_action_forever(
        _sprite.value(), 20, _sprite_item.value().tiles_item(), 16, 16, 16, 16,
        17, 17, 17, 17);
  } else {
    _sprite.value().set_horizontal_flip(_loiter_target_position.x() <
                                        _sprite.value().x());
    _action = bn::create_sprite_animate_action_forever(
        _sprite.value(), 12, _sprite_item.value().tiles_item(), 0, 1, 2, 3, 4,
        5, 6, 7);
  }
}

void Person::_stop_loitering() {
  if (_active_loiterers > 0) {
    --_active_loiterers;
  }
  _is_loitering = false;
  _loiter_time = 0;
  _loiter_duration_frames = 0;
  _loiter_in_position = false;
  bool face_left = _state == STATE::WALKING_LEFT ||
                   _state == STATE::WALKING_LEFT_W_COFFEE ||
                   _state == STATE::WALKING_LEFT_PASSER;
  _sprite.value().set_horizontal_flip(face_left);
  _action = bn::create_sprite_animate_action_forever(
      _sprite.value(), 12, _sprite_item.value().tiles_item(), 0, 1, 2, 3, 4, 5,
      6, 7);
}

bool Person::_update_loiter_overlay() {
  if (!_is_loitering) {
    return false;
  }

  if (_loiter_in_position) {
    _loiter_time += 1;
    if (_loiter_duration_frames > 0 &&
        _loiter_time >= _loiter_duration_frames) {
      _stop_loitering();
    }
  } else {
    bn::fixed_point next_step = ti::get_next_step(
        _sprite.value().position(), _loiter_target_position, _speed);
    _sprite.value().set_position(next_step);
    if (next_step.x() == _loiter_target_position.x() &&
        next_step.y() == _loiter_target_position.y()) {
      _loiter_in_position = true;
      _action = bn::create_sprite_animate_action_forever(
          _sprite.value(), 20, _sprite_item.value().tiles_item(), 16, 16, 16,
          16, 17, 17, 17, 17);
    }
  }
  return _is_loitering;
}

int Person::_state_index(STATE state) {
  int idx = static_cast<int>(state) - 1;
  return idx < 0 ? 0 : idx;
}

void Person::_handle_walking_right(bn::deque<int, 8>&, bool&, bool&,
                                   bn::vector<int, 16>&) {
  bn::fixed_point next_step =
      ti::get_next_step(_sprite.value().position(), OUTSIDE, _speed);
  _sprite.value().set_position(next_step);
  bool started_loitering = _try_start_loitering();
  if (!started_loitering && OUTSIDE.x() == next_step.x() &&
      OUTSIDE.y() == next_step.y()) {
    if (_should_walk_by()) {
      _state = STATE::WALKING_RIGHT_PASSER;
      _sprite.value().set_horizontal_flip(false);
      _action = bn::create_sprite_animate_action_forever(
          _sprite.value(), 12, _sprite_item.value().tiles_item(), 0, 1, 2, 3, 4,
          5, 6, 7);
    } else {
      _state = STATE::ENTERING;
      _sprite.value().set_horizontal_flip(true);
    }
  }
}

void Person::_handle_walking_left(bn::deque<int, 8>&, bool&, bool&,
                                  bn::vector<int, 16>&) {
  bn::fixed_point next_step =
      ti::get_next_step(_sprite.value().position(), OUTSIDE, _speed);
  _sprite.value().set_position(next_step);
  bool started_loitering = _try_start_loitering();
  if (!started_loitering && OUTSIDE.x() == next_step.x() &&
      OUTSIDE.y() == next_step.y()) {
    if (_should_walk_by()) {
      _state = STATE::WALKING_LEFT_PASSER;
      _sprite.value().set_horizontal_flip(true);
      _action = bn::create_sprite_animate_action_forever(
          _sprite.value(), 12, _sprite_item.value().tiles_item(), 0, 1, 2, 3, 4,
          5, 6, 7);
    } else {
      _state = STATE::ENTERING;
      _sprite.value().set_horizontal_flip(true);
    }
  }
}

void Person::_handle_entering(bn::deque<int, 8>&, bool&, bool&,
                              bn::vector<int, 16>&) {
  bn::fixed_point next_step =
      ti::get_next_step(_sprite.value().position(), DOOR, _speed);
  _sprite.value().set_position(next_step);
  if (DOOR.x() == next_step.x() && DOOR.y() == next_step.y()) {
    _state = STATE::WALKING_TO_ORDER;
    _sprite.value().set_horizontal_flip(true);
  }
}

void Person::_handle_walking_to_order(bn::deque<int, 8>&, bool&, bool&,
                                      bn::vector<int, 16>&) {
  bn::fixed_point next_step =
      ti::get_next_step(_sprite.value().position(), QUEUE_START, _speed);
  _sprite.value().set_position(next_step);
  if (QUEUE_START.x() == next_step.x() && QUEUE_START.y() == next_step.y()) {
    _state = STATE::JOINING_QUEUE;
  }
}

void Person::_handle_joining_queue(bn::deque<int, 8>& order_queue,
                                   bool& waiting_spot,
                                   bool& purchased_this_frame,
                                   bn::vector<int, 16>& types) {
  (void)waiting_spot;
  (void)purchased_this_frame;
  (void)types;
  bn::fixed_point next_step =
      ti::get_next_step(_sprite.value().position(), TILL, _speed);
  int index = locate_in_queue(order_queue, _id);
  if (index == -1) {
    if (order_queue.size() >= 5) {
      _state = STATE::WALKING_TO_DOOR;
      _sprite.value().set_horizontal_flip(false);
      return;
    }
    order_queue.push_back(_id);
  } else {
    next_step = ti::get_next_step(_sprite.value().position(),
                                  LOCATIONS.at(index), _speed);
  }

  if (_sprite.value().position().x() == next_step.x() &&
      _sprite.value().position().y() == next_step.y()) {
    _state = STATE::WAITING_TO_ORDER;
    _action = bn::create_sprite_animate_action_forever(
        _sprite.value(), 20, _sprite_item.value().tiles_item(), 16, 16, 16, 16,
        17, 17, 17, 17);
    _sprite.value().set_horizontal_flip(true);
  }

  _sprite.value().set_position(next_step);
}

void Person::_handle_waiting_to_order(bn::deque<int, 8>& order_queue, bool&,
                                      bool&, bn::vector<int, 16>&) {
  int index = locate_in_queue(order_queue, _id);
  bn::fixed_point next_step = ti::get_next_step(_sprite.value().position(),
                                                LOCATIONS.at(index), _speed);

  if (_sprite.value().position().x() == next_step.x() &&
      _sprite.value().position().y() == next_step.y()) {
    if (index == 0) {
      _state = STATE::ORDERING;
    }
    _action = bn::create_sprite_animate_action_forever(
        _sprite.value(), 20, _sprite_item.value().tiles_item(), 16, 16, 16, 16,
        17, 17, 17, 17);
  } else {
    _action = bn::create_sprite_animate_action_forever(
        _sprite.value(), 12, _sprite_item.value().tiles_item(), 0, 1, 2, 3, 4,
        5, 6, 7);
  }
  _sprite.value().set_position(next_step);
}

void Person::_handle_ordering(bn::deque<int, 8>& order_queue, bool&,
                              bool& purchased_this_frame,
                              bn::vector<int, 16>&) {
  _wait_time = _wait_time += 1;
  if (_wait_time > _wait_max) {
    purchased_this_frame = true;
    _wait_time = 0;
    _state = STATE::WALKING_TO_COUNTER;
    order_queue.pop_front();
    _action = bn::create_sprite_animate_action_forever(
        _sprite.value(), 12, _sprite_item.value().tiles_item(), 0, 1, 2, 3, 4,
        5, 6, 7);
    _sprite.value().set_horizontal_flip(true);
  }
}

void Person::_handle_walking_to_counter(bn::deque<int, 8>&, bool& waiting_spot,
                                        bool&, bn::vector<int, 16>&) {
  bn::fixed_point counter = waiting_spot ? COUNTER2 : COUNTER1;
  bn::fixed_point next_step =
      ti::get_next_step(_sprite.value().position(), counter, _speed);
  _sprite.value().set_position(next_step);

  if (counter.x() == next_step.x() && counter.y() == next_step.y()) {
    _state = STATE::WAITING;
    waiting_spot = !waiting_spot;
    _action = bn::create_sprite_animate_action_forever(
        _sprite.value(), 20, _sprite_item.value().tiles_item(), 16, 16, 16, 16,
        17, 17, 17, 17);
    _sprite.value().set_horizontal_flip(true);
  }
}

void Person::_handle_waiting(bn::deque<int, 8>&, bool&, bool&,
                             bn::vector<int, 16>&) {
  _wait_time = _wait_time += 1;
  if (_wait_time > _wait_max + 60) {
    _wait_time = 0;
    _state = STATE::WALKING_TO_DOOR;
    _action = bn::create_sprite_animate_action_forever(
        _sprite.value(), 12, _sprite_item.value().tiles_item(), 8, 9, 10, 11,
        12, 13, 14, 15);
    _sprite.value().set_horizontal_flip(false);
  }
}

void Person::_handle_walking_to_door(bn::deque<int, 8>&, bool&, bool&,
                                     bn::vector<int, 16>&) {
  bn::fixed_point next_step =
      ti::get_next_step(_sprite.value().position(), DOOR, _speed);
  _sprite.value().set_position(next_step);

  if (DOOR.x() == next_step.x() && DOOR.y() == next_step.y()) {
    _state = STATE::EXITING;
    _sprite.value().set_horizontal_flip(false);
  }
}

void Person::_handle_exiting(bn::deque<int, 8>&, bool&, bool&,
                             bn::vector<int, 16>&) {
  bn::fixed_point next_step =
      ti::get_next_step(_sprite.value().position(), OUTSIDE, _speed);
  _sprite.value().set_position(next_step);
  if (OUTSIDE.x() == next_step.x() && OUTSIDE.y() == next_step.y()) {
    bool is_left = _random.get_int(10) > 5;
    if (is_left) {
      _state = STATE::WALKING_LEFT_W_COFFEE;
      _sprite.value().set_horizontal_flip(true);
    } else {
      _state = STATE::WALKING_RIGHT_W_COFFEE;
      _sprite.value().set_horizontal_flip(false);
    }
  }
  _sprite.value().set_z_order(-300);
}

void Person::_handle_walking_right_passer(bn::deque<int, 8>&, bool&, bool&,
                                          bn::vector<int, 16>&) {
  bn::fixed_point next_step =
      ti::get_next_step(_sprite.value().position(), RIGHT, _speed);
  _sprite.value().set_position(next_step);
  bool started_loitering = _try_start_loitering();
  if (!started_loitering && RIGHT.x() == next_step.x() &&
      RIGHT.y() == next_step.y()) {
    _state = STATE::WALKING_LEFT;
    _sprite.value().set_horizontal_flip(true);
  }
}

void Person::_handle_walking_left_passer(bn::deque<int, 8>&, bool&, bool&,
                                         bn::vector<int, 16>&) {
  bn::fixed_point next_step =
      ti::get_next_step(_sprite.value().position(), LEFT, _speed);
  _sprite.value().set_position(next_step);
  bool started_loitering = _try_start_loitering();
  if (!started_loitering && LEFT.x() == next_step.x() &&
      LEFT.y() == next_step.y()) {
    _state = STATE::WALKING_RIGHT;
    _sprite.value().set_horizontal_flip(false);
  }
}

void Person::_handle_walking_left_with_coffee(bn::deque<int, 8>&, bool&, bool&,
                                              bn::vector<int, 16>& types) {
  bn::fixed_point next_step =
      ti::get_next_step(_sprite.value().position(), LEFT, _speed);
  _sprite.value().set_position(next_step);
  bool started_loitering = _try_start_loitering();

  if (!started_loitering && LEFT.x() == next_step.x() &&
      LEFT.y() == next_step.y()) {
    _state = STATE::WALKING_RIGHT;
    int type_index = _random.get_int(types.size());
    int next_type = types.at(type_index);
    types.erase(types.begin() + type_index);
    setStyle(static_cast<TYPE>(next_type), START::LEFT,
             _sprite.value().position());
    _sprite.value().set_horizontal_flip(false);
  }
}

void Person::_handle_walking_right_with_coffee(bn::deque<int, 8>&, bool&, bool&,
                                               bn::vector<int, 16>& types) {
  bn::fixed_point next_step =
      ti::get_next_step(_sprite.value().position(), RIGHT, _speed);
  _sprite.value().set_position(next_step);
  bool started_loitering = _try_start_loitering();

  if (!started_loitering && RIGHT.x() == next_step.x() &&
      RIGHT.y() == next_step.y()) {
    _state = STATE::WALKING_LEFT;
    int type_index = _random.get_int(types.size());
    int next_type = types.at(type_index);
    types.erase(types.begin() + type_index);
    setStyle(static_cast<TYPE>(next_type), START::RIGHT,
             _sprite.value().position());
    _sprite.value().set_horizontal_flip(true);
  }
}

/**
 * @brief Main state machine update for the Person (customer) object.
 *
 * Handles all movement, queuing, ordering, waiting, leaving logic per frame.
 * WARNING: Core to game balance—subtle changes deeply affect flow/feel!
 *
 * - order_queue: Global cafe customer queue by id
 * - waiting_spot: Reference flag for counter queue position
 * - purchased_this_frame: Set true if this customer buys during the update
 * - types: Pool of available style/type indices for respawning
 */
void Person::update(bn::deque<int, 8>& order_queue, bool& waiting_spot,
                    bool& purchased_this_frame, bn::vector<int, 16>& types) {
  if (!_update_loiter_overlay()) {
    const StateHandler handler = _state_handlers[_state_index(_state)];
    (this->*handler)(order_queue, waiting_spot, purchased_this_frame, types);
  }

  _sprite.value().set_z_order(-_sprite.value().y().integer());

  _shadow.set_x(_sprite.value().x());
  _shadow.set_y(_sprite.value().y() + 15);

  if (_action.has_value() && !_action.value().done()) {
    _action.value().update();
  }
  _random.get();
}
}  // namespace ti