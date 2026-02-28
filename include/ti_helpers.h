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
}  // namespace ti

#endif
