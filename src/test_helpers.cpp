/**
 * @file test_helpers.cpp
 * @brief Minimal test harness for ti_helpers logic (in-game log view).
 */

#include "bn_core.h"
#include "bn_fixed.h"
#include "bn_fixed_point.h"
#include "bn_log.h"
#include "bn_string.h"
#include "ti_helpers.h"

namespace {

void run_get_next_step_tests() {
  bn::log(bn::string<32>("ti::get_next_step unit tests:"));

  // Test 1: far away, should step toward
  bn::fixed_point from(10, 10);
  bn::fixed_point to(20, 20);
  bn::fixed speed = 3;
  bn::fixed_point result = ti::get_next_step(from, to, speed);
  bn::string<64> msg;
  msg = "1: from=(10,10) to=(20,20) speed=3 -> (";
  msg.append(bn::to_string<8>(result.x().integer()));
  msg.append(",");
  msg.append(bn::to_string<8>(result.y().integer()));
  msg.append(")");
  bn::log(msg);

  // Test 2: already at target
  from = bn::fixed_point(20, 20);
  to = bn::fixed_point(20, 20);
  result = ti::get_next_step(from, to, speed);
  msg = "2: from=(20,20) to=(20,20) speed=3 -> (";
  msg.append(bn::to_string<8>(result.x().integer()));
  msg.append(",");
  msg.append(bn::to_string<8>(result.y().integer()));
  msg.append(")");
  bn::log(msg);

  // Test 3: within 2 units in x, should snap exactly
  from = bn::fixed_point(21, 20);
  to = bn::fixed_point(20, 20);
  result = ti::get_next_step(from, to, speed);
  msg = "3: from=(21,20) to=(20,20) speed=3 -> (";
  msg.append(bn::to_string<8>(result.x().integer()));
  msg.append(",");
  msg.append(bn::to_string<8>(result.y().integer()));
  msg.append(")");
  bn::log(msg);
}

}  // namespace

int main() {
  run_get_next_step_tests();
  while (true) {
    // Keep GBA program alive for log viewing
  }
  return 0;
}
