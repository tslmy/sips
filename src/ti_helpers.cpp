/**
 * @file ti_helpers.cpp
 * @brief Implementation of helpers for misc math/game logic (see ti_helpers.h)
 */

#include "ti_helpers.h"

namespace ti {

/**
 * Which point to move from "from" toward "to" by up to "speed" units?
 * If within 2 units, snap to "to".
 */
bn::fixed_point get_next_step(const bn::fixed_point &from,
                              const bn::fixed_point &to, bn::fixed speed) {
  const bn::fixed snap_distance(2);

  // This implementation avoids trigonometric functions (and sqrt) for
  // performance. This is sufficient for GBA.
  const bn::fixed delta_x = to.x() - from.x();
  const bn::fixed delta_y = to.y() - from.y();
  const bn::fixed abs_delta_x = delta_x >= 0 ? delta_x : -delta_x;
  const bn::fixed abs_delta_y = delta_y >= 0 ? delta_y : -delta_y;

  if (abs_delta_x > snap_distance || abs_delta_y > snap_distance) {
    const bn::fixed dominant =
        abs_delta_x > abs_delta_y ? abs_delta_x : abs_delta_y;
    const bn::fixed step_x = (delta_x * speed) / dominant;
    const bn::fixed step_y = (delta_y * speed) / dominant;

    return bn::fixed_point(from.x() + step_x, from.y() + step_y);
  }

  return to;
}

}  // namespace ti
