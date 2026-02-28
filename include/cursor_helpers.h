/**
 * @file cursor_helpers.h
 * @brief Cursor movement and skipping-purchased-item logic, no dependencies.
 *
 * This header is standalone/test-friendly and designed for use in both
 * host-side unit tests and embedded game builds. It is dependency-free and
 * STL-compatible.
 */

#ifndef CURSOR_HELPERS_H
#define CURSOR_HELPERS_H

namespace ti {
/**
 * Finds the next cursor index when skipping purchased items.
 *
 * @param current_index The current cursor position
 * @param delta +1 for downward, -1 for upward movement
 * @param prices The vector of item prices (0 = purchased)
 * @return The new valid cursor position, or current_index if none found
 * in direction
 */
template <typename Prices>
int move_cursor(int current_index, int delta, const Prices& prices) {
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

#endif  // CURSOR_HELPERS_H
