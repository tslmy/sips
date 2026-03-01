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
  STREET = 1,
  ENTERING = 2,
  WALKING_TO_ORDER = 3,
  JOINING_QUEUE = 4,
  WAITING_TO_ORDER = 5,
  ORDERING = 6,
  WALKING_TO_COUNTER = 7,
  WAITING_AT_COUNTER = 8,
  WALKING_TO_DOOR = 9,
  EXITING = 10,
  LOITERING = 11,
};

enum class STREET_ROLE {
  ENTER_FROM_LEFT = 0,
  ENTER_FROM_RIGHT = 1,
  PASS_LEFT_TO_RIGHT = 2,
  PASS_RIGHT_TO_LEFT = 3,
  EXIT_LEFT_WITH_COFFEE = 4,
  EXIT_RIGHT_WITH_COFFEE = 5,
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
  STATE _state = STATE::STREET;
  void setStyle(TYPE type, START start, bn::fixed_point pos);
  int _id;
  bool _has_loitered = false;
  int _loiter_time = 0;
  int _loiter_duration_frames = 0;
  STATE _loiter_resume_state = STATE::STREET;
  STREET_ROLE _loiter_resume_street_role = STREET_ROLE::ENTER_FROM_LEFT;
  STREET_ROLE _street_role = STREET_ROLE::ENTER_FROM_LEFT;
  bn::fixed_point _loiter_target_position = bn::fixed_point(0, 0);
  bool _loiter_in_position = false;
  static int _active_loiterers;
  static constexpr int _max_loiterers = 3;
  static constexpr int _walk_by_chance = 4;  // 1 in 4 chance to skip entering
  bool _try_start_loitering(STREET_ROLE resume_role);
  void _begin_loitering(STREET_ROLE resume_role);
  void _stop_loitering();
  bn::fixed_point _random_street_loiter_point(STREET_ROLE resume_role);
  bn::fixed _randomized_street_y(bn::fixed base_y);
  bool _should_walk_by();
  void _set_street_role(STREET_ROLE role);
  void _update_street(bn::vector<int, 16>& types);
  bn::fixed_point _street_target() const;
  void _handle_street_arrival(bn::vector<int, 16>& types);
  bool _street_faces_left(STREET_ROLE role) const;
  void _respawn_new_customer(START start, bn::vector<int, 16>& types);
  bool _street_allows_loiter(STREET_ROLE role) const;

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