/**
 * @file ti_helpers.cpp
 * @brief Implementation of helpers for misc math/game logic (see ti_helpers.h)
 */

#include "ti_helpers.h"

#include "bn_math.h"
#include "bn_vector.h"

namespace ti {

bn::fixed_point get_next_step(const bn::fixed_point& from,
                              const bn::fixed_point& to, bn::fixed speed) {
  bn::fixed diff_x = from.x() - to.x();
  bn::fixed diff_y = from.y() - to.y();

  if (bn::abs(diff_x) > 2 || bn::abs(diff_y) > 2) {
    bn::fixed angle = bn::degrees_atan2(diff_y.integer(), diff_x.integer());
    bn::pair<bn::fixed, bn::fixed> xy = bn::degrees_sin_and_cos(angle);

    return bn::fixed_point(from.x() - speed * xy.second,
                           from.y() - speed * xy.first);
  }

  return to;
}

}  // namespace ti
