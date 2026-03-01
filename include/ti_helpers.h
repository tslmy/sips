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

/**
 * Finds the next cursor index when skipping unavailable items.
 *
 * @param current_index Current cursor position
 * @param delta +1 for downward, -1 for upward movement
 * @param purchased Purchased flags where true means unavailable
 * @return The next valid cursor index, or current_index if none exists in
 * direction
 */
template <typename PurchasedFlags>
int move_cursor_available(int current_index, int delta,
                          const PurchasedFlags& purchased) {
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
}  // namespace ti

#endif
