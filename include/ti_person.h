/**
 * @file ti_person.h
 * @brief Declares the Person class and types that control characters
 * (customers) in the cafe game.
 *
 * STATE: Enum for character state machine (walking, ordering, etc).
 * TYPE: Visual/style enum for sprite appearance variants.
 * START: Enum for entry/exit position options.
 *
 * Person: Represents a single customer in the game, their movement, animation,
 * and logic.
 *
 * Usage: Used by main game logic to simulate customer flow and interactions.
 */
#ifndef TI_PERSON_H
#define TI_PERSON_H

#include "bn_blending.h"
#include "bn_deque.h"
#include "bn_fixed_point.h"
#include "bn_optional.h"
#include "bn_random.h"
#include "bn_sprite_animate_actions.h"
#include "bn_sprite_item.h"
#include "bn_sprite_ptr.h"

namespace ti {

/**
 * @brief Represents the different states a customer can be in during their
 * lifecycle in the game.
 */
enum class STATE {
  WALKING_LEFT = 1,
  WALKING_LEFT_W_COFFEE = 2,
  WALKING_RIGHT = 3,
  WALKING_RIGHT_W_COFFEE = 4,
  ENTERING = 5,
  WALKING_TO_ORDER = 6,
  WAITING_TO_ORDER = 7,
  ORDERING = 8,
  WALKING_TO_COUNTER = 9,
  WAITING = 10,
  WALKING_TO_DOOR = 11,
  EXITING = 12,
  JOINING_QUEUE = 13,
  LOITERING = 14,
};

/**
 * @brief Enumerates all possible character sprite styles/types.
 */
enum class TYPE {
  GREEN_SHIRT = 0,
  RED_SHIRT = 1,
  BLUE_SHIRT = 2,
  RED_SINGLET = 3,
  DWIGHT = 4,
  GIRL1 = 5,
  GIRL2 = 6,
  PALE_GREEN_SHIRT = 7,
  GIRL3 = 8,
  PERSON1 = 9,
  PERSON2 = 10,
  PERSON3 = 11,
  PERSON4 = 12,
  PERSON5 = 13
};

/**
 * @brief Entry locations for a Person: left side, right side, or at the
 * counter.
 */
enum class START { LEFT, RIGHT, COUNTER };

}  // namespace ti

namespace ti {

/**
 * @class Person
 * @brief Models a customer character, handling movement, animation, queueing,
 * and ordering logic in the cafe.
 *
 * Typical usage: Instantiated by the main game loop, repeatedly updated, and
 * rendered to the screen with a sprite and shadow.
 */
class Person {
 private:
  bn::optional<bn::sprite_ptr> _sprite;
  bn::optional<bn::sprite_animate_action<8>> _action;
  bn::sprite_ptr _shadow;
  bn::optional<bn::sprite_item> _sprite_item;
  bn::fixed _speed = 0.3;
  bn::random _random = bn::random();
  TYPE _type = TYPE::GREEN_SHIRT;
  int _wait_max = 320;
  int _wait_time = 0;
  STATE _state = STATE::WAITING;
  void setStyle(TYPE type, START start, bn::fixed_point pos);
  int _id;
  bool _has_loitered = false;
  int _loiter_time = 0;
  int _loiter_duration_frames = 0;
  STATE _loiter_resume_state = STATE::WALKING_RIGHT;
  bn::fixed_point _loiter_target_position = bn::fixed_point(0, 0);
  bool _loiter_in_position = false;
  static int _active_loiterers;
  static constexpr int _max_loiterers = 3;
  bool _try_start_loitering(STATE resume_state);
  void _begin_loitering(STATE resume_state);
  void _stop_loitering();
  bn::fixed_point _random_street_loiter_point(STATE resume_state);

 public:
  /**
   * @brief Constructs a Person with the given starting location, type, and
   * unique id.
   * @param start Entry position (LEFT, RIGHT, COUNTER)
   * @param type Visual/style type enum
   * @param id Unique person id (used throughout the game's queue and logic
   * flows)
   */
  Person(START start, TYPE type, int id);

  /**
   * @brief Per-frame update for this character: controls position, state
   * machine, ordering, and animation logic.
   * @param order_queue Reference to the global queue of customer IDs
   * @param waiting_spot Whether the alternate waiting spot is occupied
   * @param purchased_this_frame Flag flipped when the character completes a
   * purchase
   * @param types List of available style types for recycling when respawning
   */
  void update(bn::deque<int, 8>& order_queue, bool& waiting_spot,
              bool& purchased_this_frame, bn::vector<int, 16>& types);

  int get_id();
  TYPE get_type();
  bn::fixed_point TILL = bn::fixed_point(-66, 14);
  bn::fixed_point COUNTER1 = bn::fixed_point(-100, 16);
  bn::fixed_point COUNTER2 = bn::fixed_point(-86, 14);
  bn::fixed_point DOOR = bn::fixed_point(88, 36);
  bn::fixed_point OUTSIDE = bn::fixed_point(100, 60);
  bn::fixed_point LEFT = bn::fixed_point(-140, 60);
  bn::fixed_point RIGHT = bn::fixed_point(180, 60);
  bn::fixed_point QUEUE_START = bn::fixed_point(0, 24);

  bn::vector<bn::fixed_point, 5> LOCATIONS;
};
}  // namespace ti

#endif