/**
 * @file ti_helpers.cpp
 * @brief Implementation of helpers for misc math/game logic (see ti_helpers.h)
 */

#include "ti_helpers.h"

#if TI_HOST_TEST_MATH
#include <cmath>
#else
#include "bn_math.h"
#endif

namespace ti {

bn::fixed_point get_next_step(const bn::fixed_point& from,
                              const bn::fixed_point& to, bn::fixed speed) {
  bn::fixed diff_x = from.x() - to.x();
  bn::fixed diff_y = from.y() - to.y();
  bn::fixed abs_diff_x = diff_x >= 0 ? diff_x : -diff_x;
  bn::fixed abs_diff_y = diff_y >= 0 ? diff_y : -diff_y;

  if (abs_diff_x > 2 || abs_diff_y > 2) {
#if TI_HOST_TEST_MATH
    const double angle =
        std::atan2(double(diff_y.integer()), double(diff_x.integer()));
    const bn::fixed sin_value = bn::fixed(std::sin(angle));
    const bn::fixed cos_value = bn::fixed(std::cos(angle));

    return bn::fixed_point(from.x() - speed * cos_value,
                           from.y() - speed * sin_value);
#else
    bn::fixed angle = bn::degrees_atan2(diff_y.integer(), diff_x.integer());
    bn::pair<bn::fixed, bn::fixed> xy = bn::degrees_sin_and_cos(angle);

    return bn::fixed_point(from.x() - speed * xy.second,
                           from.y() - speed * xy.first);
#endif
  }

  return to;
}

}  // namespace ti
