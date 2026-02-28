/**
 * @file ti_helpers.h
 * @brief Miscellaneous helper functions for game math and logic.
 *
 * This header defines reusable small utilities, math helpers, and functions
 * suitable for unit testing.
 */

#ifndef TI_HELPERS_H
#define TI_HELPERS_H

#include "bn_fixed.h"
#include "bn_fixed_point.h"
#include "bn_vector.h"

namespace ti {
/**
 * @brief Move "from" toward "to" by up to "speed" units. If within 2 units,
 * snap to "to".
 *
 * Useful for simple tile/sprite chasing logic and variable speed step.
 * @param from Starting position
 * @param to Target position
 * @param speed Max amount to move this frame
 * @return Next step (clamped to "to" position if close enough)
 */
bn::fixed_point get_next_step(const bn::fixed_point& from,
                              const bn::fixed_point& to, bn::fixed speed);
}  // namespace ti

/**
 * Finds the next cursor index when skipping purchased items.
 *
 * @param current_index The current cursor position
 * @param delta +1 for downward, -1 for upward movement
 * @param prices The vector of item prices (0 = purchased)
 * @return The new valid cursor position, or current_index if none found in
 * direction
 */
namespace ti {
template <int N>
int move_cursor(int current_index, int delta,
                const bn::vector<int, N>& prices) {
  int original_index = current_index;
  int idx = current_index + delta;
  while (idx >= 0 && idx < int(prices.size()) && prices[idx] == 0) {
    idx += delta;
  }
  if (idx < 0 || idx >= int(prices.size())) {
    // No valid item found in direction, stay at original
    return original_index;
  }
  return idx;
}
}  // namespace ti

#endif
