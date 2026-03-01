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
int locate_in_queue(const bn::deque<int, 8>& order_queue, int id) {
  for (int i = 0; i < order_queue.size(); ++i) {
    if (order_queue.at(i) == id) return i;
  }
  return -1;
}
}  // namespace

int Person::_active_loiterers = 0;

bool Person::_street_faces_left(STREET_ROLE role) const {
  return role == STREET_ROLE::ENTER_FROM_RIGHT ||
         role == STREET_ROLE::PASS_RIGHT_TO_LEFT ||
         role == STREET_ROLE::EXIT_LEFT_WITH_COFFEE;
}

bn::fixed_point Person::_street_target() const {
  switch (_street_role) {
    case STREET_ROLE::ENTER_FROM_LEFT:
    case STREET_ROLE::ENTER_FROM_RIGHT:
      return OUTSIDE;
    case STREET_ROLE::PASS_LEFT_TO_RIGHT:
    case STREET_ROLE::EXIT_RIGHT_WITH_COFFEE:
      return RIGHT;
    case STREET_ROLE::PASS_RIGHT_TO_LEFT:
    case STREET_ROLE::EXIT_LEFT_WITH_COFFEE:
      return LEFT;
    default:
      return OUTSIDE;
  }
}

bool Person::_street_allows_loiter(STREET_ROLE) const { return true; }

void Person::_set_street_role(STREET_ROLE role) {
  _street_role = role;
  _state = STATE::STREET;
  _sprite.value().set_horizontal_flip(_street_faces_left(role));
  _action = bn::create_sprite_animate_action_forever(
      _sprite.value(), 12, _sprite_item.value().tiles_item(), 0, 1, 2, 3, 4, 5,
      6, 7);
}

void Person::_respawn_new_customer(START start, bn::vector<int, 16>& types) {
  if (types.empty()) {
    return;
  }
  int type_index = _random.get_int(types.size());
  int next_type = types.at(type_index);
  types.erase(types.begin() + type_index);
  setStyle(static_cast<TYPE>(next_type), start, _sprite.value().position());
}

void Person::_handle_street_arrival(bn::vector<int, 16>& types) {
  switch (_street_role) {
    case STREET_ROLE::ENTER_FROM_LEFT:
      if (_should_walk_by()) {
        _set_street_role(STREET_ROLE::PASS_LEFT_TO_RIGHT);
        return;
      }
      _state = STATE::ENTERING;
      _sprite.value().set_horizontal_flip(true);
      break;
    case STREET_ROLE::ENTER_FROM_RIGHT:
      if (_should_walk_by()) {
        _set_street_role(STREET_ROLE::PASS_RIGHT_TO_LEFT);
        return;
      }
      _state = STATE::ENTERING;
      _sprite.value().set_horizontal_flip(true);
      break;
    case STREET_ROLE::PASS_LEFT_TO_RIGHT:
      _set_street_role(STREET_ROLE::ENTER_FROM_RIGHT);
      break;
    case STREET_ROLE::PASS_RIGHT_TO_LEFT:
      _set_street_role(STREET_ROLE::ENTER_FROM_LEFT);
      break;
    case STREET_ROLE::EXIT_LEFT_WITH_COFFEE:
      _respawn_new_customer(START::LEFT, types);
      _set_street_role(STREET_ROLE::ENTER_FROM_LEFT);
      break;
    case STREET_ROLE::EXIT_RIGHT_WITH_COFFEE:
      _respawn_new_customer(START::RIGHT, types);
      _set_street_role(STREET_ROLE::ENTER_FROM_RIGHT);
      break;
  }
}

void Person::_update_street(bn::vector<int, 16>& types) {
  bn::fixed_point target = _street_target();
  bn::fixed_point next_step =
      ti::get_next_step(_sprite.value().position(), target, _speed);
  _sprite.value().set_position(next_step);

  bool reached_target =
      target.x() == next_step.x() && target.y() == next_step.y();
  if (!reached_target && _street_allows_loiter(_street_role)) {
    _try_start_loitering(_street_role);
    if (_state == STATE::LOITERING) {
      return;
    }
  }

  if (reached_target) {
    _handle_street_arrival(types);
  }
}

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
  STREET_ROLE initial_role = STREET_ROLE::EXIT_LEFT_WITH_COFFEE;
  if (start == START::RIGHT) {
    pos.set_x(160);
    initial_role = STREET_ROLE::EXIT_RIGHT_WITH_COFFEE;
  } else if (start == START::COUNTER) {
    pos = bn::fixed_point(-86, 14);
  }

  setStyle(type, start, pos);

  if (start == START::COUNTER) {
    _state = STATE::WAITING_AT_COUNTER;
    _sprite.value().set_horizontal_flip(true);
    _sprite.value().set_z_order(-100);
    _action = bn::create_sprite_animate_action_forever(
        _sprite.value(), 20, _sprite_item.value().tiles_item(), 16, 16, 16, 16,
        17, 17, 17, 17);
  } else {
    _set_street_role(initial_role);
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
  _loiter_time = 0;
  _loiter_duration_frames = 0;
  _loiter_in_position = false;
  _loiter_target_position = pos;
  _action = bn::create_sprite_animate_action_forever(
      _sprite.value(), 12, _sprite_item.value().tiles_item(), 0, 1, 2, 3, 4, 5,
      6, 7);
}

TYPE Person::get_type() { return _type; }

bn::fixed_point Person::_random_street_loiter_point(STREET_ROLE resume_role) {
  bn::fixed_point current_pos = _sprite.value().position();
  bn::fixed target_x = current_pos.x();

  switch (resume_role) {
    case STREET_ROLE::ENTER_FROM_LEFT:
    case STREET_ROLE::ENTER_FROM_RIGHT:
      target_x = OUTSIDE.x();
      break;
    case STREET_ROLE::PASS_LEFT_TO_RIGHT:
    case STREET_ROLE::EXIT_RIGHT_WITH_COFFEE:
      target_x = RIGHT.x();
      break;
    case STREET_ROLE::PASS_RIGHT_TO_LEFT:
    case STREET_ROLE::EXIT_LEFT_WITH_COFFEE:
      target_x = LEFT.x();
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

bool Person::_try_start_loitering(STREET_ROLE resume_role) {
  if (_has_loitered || _state == STATE::LOITERING ||
      _active_loiterers >= _max_loiterers) {
    return false;
  }

  constexpr int LOITER_CHANCE_FRAMES = 360;  // ~6 seconds average between tries
  if (_random.get_int(LOITER_CHANCE_FRAMES) == 0) {
    _begin_loitering(resume_role);
    return true;
  }

  return false;
}

void Person::_begin_loitering(STREET_ROLE resume_role) {
  _has_loitered = true;
  _state = STATE::LOITERING;
  _loiter_resume_state = STATE::STREET;
  _loiter_resume_street_role = resume_role;
  _loiter_time = 0;
  _loiter_duration_frames = (_random.get_int(9) + 2) * 60;
  _loiter_target_position = _random_street_loiter_point(resume_role);
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
  _loiter_time = 0;
  _loiter_duration_frames = 0;
  _loiter_in_position = false;
  if (_loiter_resume_state == STATE::STREET) {
    _set_street_role(_loiter_resume_street_role);
  } else {
    _state = _loiter_resume_state;
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
  switch (_state) {
    case STATE::WALKING_TO_COUNTER: {
      bn::fixed_point counter = COUNTER1;
      if (waiting_spot) {
        counter = COUNTER2;
      }
      bn::fixed_point next_step =
          ti::get_next_step(_sprite.value().position(), counter, _speed);
      _sprite.value().set_position(next_step);

      if (counter.x() == next_step.x() && counter.y() == next_step.y()) {
        // At the counter, now wait
        _state = STATE::WAITING_AT_COUNTER;
        waiting_spot = !waiting_spot;
        _action = bn::create_sprite_animate_action_forever(
            _sprite.value(), 20, _sprite_item.value().tiles_item(), 16, 16, 16,
            16, 17, 17, 17, 17);
        _sprite.value().set_horizontal_flip(true);
      }
      break;
      case STATE::WALKING_TO_ORDER: {
        bn::fixed_point next_step =
            ti::get_next_step(_sprite.value().position(), QUEUE_START, _speed);
        _wait_time = _wait_time += 1;
        if (_wait_time > _wait_max + 60) {
          _wait_time = 0;
          _state = STATE::WALKING_TO_DOOR;
          _action = bn::create_sprite_animate_action_forever(
              _sprite.value(), 12, _sprite_item.value().tiles_item(), 8, 9, 10,
              11, 12, 13, 14, 15);
          _sprite.value().set_horizontal_flip(false);
        }
        bn::fixed_point next_step =
            ti::get_next_step(_sprite.value().position(), TILL, _speed);

        // check if in queue
        if (!_loiter_in_position) {
          bn::fixed_point next_step = ti::get_next_step(
              _sprite.value().position(), _loiter_target_position, _speed);
          _sprite.value().set_position(next_step);
          if (next_step.x() == _loiter_target_position.x() &&
              next_step.y() == _loiter_target_position.y()) {
            _loiter_in_position = true;
            _action = bn::create_sprite_animate_action_forever(
                _sprite.value(), 20, _sprite_item.value().tiles_item(), 16, 16,
                16, 16, 17, 17, 17, 17);
          }
        } else {
          _loiter_time += 1;
          if (_loiter_duration_frames > 0 &&
              _loiter_time >= _loiter_duration_frames) {
            _stop_loitering();
          }
        }
        if (_sprite.value().position().x() == next_step.x() &&
            _sprite.value().position().y() == next_step.y()) {
          _state = STATE::WAITING_TO_ORDER;
          _action = bn::create_sprite_animate_action_forever(
              _sprite.value(), 20, _sprite_item.value().tiles_item(), 16, 16,
              16, 16, 17, 17, 17, 17);
          _sprite.value().set_horizontal_flip(true);
        }

        _sprite.value().set_position(next_step);
        break;
      }

      case STATE::WAITING_TO_ORDER: {
        // get position in queue
        int index = locate_in_queue(order_queue, _id);

        // try to move up
        bn::fixed_point next_step = ti::get_next_step(
            _sprite.value().position(), LOCATIONS.at(index), _speed);

        // if next start ordering
        if (_sprite.value().position().x() == next_step.x() &&
            _sprite.value().position().y() == next_step.y()) {
          if (index == 0) {
            _state = STATE::ORDERING;
          }
          _action = bn::create_sprite_animate_action_forever(
              _sprite.value(), 20, _sprite_item.value().tiles_item(), 16, 16,
              16, 16, 17, 17, 17, 17);
        } else {
          _action = bn::create_sprite_animate_action_forever(
              _sprite.value(), 12, _sprite_item.value().tiles_item(), 0, 1, 2,
              3, 4, 5, 6, 7);
        }
        _sprite.value().set_position(next_step);
        break;
      }

      case STATE::ORDERING: {
        _wait_time = _wait_time += 1;
        if (_wait_time > _wait_max) {
          purchased_this_frame = true;
          _wait_time = 0;
          _state = STATE::WALKING_TO_COUNTER;
          order_queue.pop_front();
          _action = bn::create_sprite_animate_action_forever(
              _sprite.value(), 12, _sprite_item.value().tiles_item(), 0, 1, 2,
              3, 4, 5, 6, 7);
          _sprite.value().set_horizontal_flip(true);
        }
        break;
      }

      case STATE::WALKING_TO_COUNTER: {
        bn::fixed_point counter = COUNTER1;
        if (waiting_spot) {
          counter = COUNTER2;
        }
        bn::fixed_point next_step =
            ti::get_next_step(_sprite.value().position(), counter, _speed);
        _sprite.value().set_position(next_step);

        if (counter.x() == next_step.x() && counter.y() == next_step.y()) {
          // At the counter, now wait
          _state = STATE::WAITING;
          waiting_spot = !waiting_spot;
          _action = bn::create_sprite_animate_action_forever(
              _sprite.value(), 20, _sprite_item.value().tiles_item(), 16, 16,
              16, 16, 17, 17, 17, 17);
          _sprite.value().set_horizontal_flip(true);
        }
        break;
      }

      case STATE::WAITING_AT_COUNTER: {
        _wait_time = _wait_time += 1;
        if (_wait_time > _wait_max + 60) {
          _wait_time = 0;
          _state = STATE::WALKING_TO_DOOR;
          _action = bn::create_sprite_animate_action_forever(
              _sprite.value(), 12, _sprite_item.value().tiles_item(), 8, 9, 10,
              11, 12, 13, 14, 15);
          _sprite.value().set_horizontal_flip(false);
        }
        break;
      }

      case STATE::LOITERING: {
        if (!_loiter_in_position) {
          bn::fixed_point next_step = ti::get_next_step(
              _sprite.value().position(), _loiter_target_position, _speed);
          _sprite.value().set_position(next_step);
          if (next_step.x() == _loiter_target_position.x() &&
              next_step.y() == _loiter_target_position.y()) {
            _loiter_in_position = true;
            _action = bn::create_sprite_animate_action_forever(
                _sprite.value(), 20, _sprite_item.value().tiles_item(), 16, 16,
                16, 16, 17, 17, 17, 17);
          }
        } else {
          _loiter_time += 1;
          if (_loiter_duration_frames > 0 &&
              _loiter_time >= _loiter_duration_frames) {
            _stop_loitering();
            break;
          }

          case STATE::WALKING_TO_DOOR: {
          }
        }
        else if (_state == STATE::WALKING_TO_DOOR) {
          bn::fixed_point next_step =
              ti::get_next_step(_sprite.value().position(), DOOR, _speed);
          _sprite.value().set_position(next_step);

          if (DOOR.x() == next_step.x() && DOOR.y() == next_step.y()) {
            _state = STATE::EXITING;
            break;
          }

          case STATE::EXITING: {
          }
        }
        else if (_state == STATE::EXITING) {
          bn::fixed_point next_step =
              ti::get_next_step(_sprite.value().position(), OUTSIDE, _speed);
          _sprite.value().set_position(next_step);
          _set_street_role(is_left ? STREET_ROLE::EXIT_LEFT_WITH_COFFEE
                                   : STREET_ROLE::EXIT_RIGHT_WITH_COFFEE);
          _sprite.value().set_horizontal_flip(false);
        }
        break;
      }
    }
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