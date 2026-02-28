// test_helpers.cpp
// Unit tests for ti_helpers using Catch2, with mocks for Butano types.

#define CATCH_CONFIG_MAIN
#include <cmath>  // for std::abs

#include "catch2/catch.hpp"

// Minimal mocks for Butano types
typedef float fixed_t;

struct fixed_point {
  fixed_t _x, _y;
  fixed_point(fixed_t x = 0, fixed_t y = 0) : _x(x), _y(y) {}
  fixed_t x() const { return _x; }
  fixed_t y() const { return _y; }
};

namespace bn {
using fixed = fixed_t;
using fixed_point = ::fixed_point;
inline fixed abs(fixed v) { return std::abs(v); }

inline fixed degrees_atan2(int y, int x) {
  // Return angle in degrees
  return std::atan2((float)y, (float)x) * (180.0f / 3.1415926f);
}
inline std::pair<fixed, fixed> degrees_sin_and_cos(fixed deg) {
  float rad = deg * 3.1415926f / 180.0f;
  return {std::sin(rad), std::cos(rad)};
}
}  // namespace bn

// ------ Actual helper logic
namespace ti {
bn::fixed_point get_next_step(const bn::fixed_point& from,
                              const bn::fixed_point& to, bn::fixed speed) {
  bn::fixed diff_x = from.x() - to.x();
  bn::fixed diff_y = from.y() - to.y();
  if (bn::abs(diff_x) > 2 || bn::abs(diff_y) > 2) {
    bn::fixed angle = bn::degrees_atan2(diff_y, diff_x);
    auto xy = bn::degrees_sin_and_cos(angle);
    return bn::fixed_point(from.x() - speed * xy.second,
                           from.y() - speed * xy.first);
  } else {
    return to;
  }
}
}  // namespace ti

// ----------- Unit Tests --------------
TEST_CASE("get_next_step: normal movement toward target", "[helpers]") {
  bn::fixed_point from{10, 10};
  bn::fixed_point to{20, 20};
  bn::fixed speed = 3;
  auto result = ti::get_next_step(from, to, speed);
  // Should not snap; should move closer but not reach "to"
  REQUIRE((std::abs(result.x() - to.x()) < std::abs(from.x() - to.x())));
  REQUIRE((std::abs(result.y() - to.y()) < std::abs(from.y() - to.y())));
  bool at_x = (result.x() == to.x());
  bool at_y = (result.y() == to.y());
  bool both_at_target = at_x && at_y;
  REQUIRE_FALSE(both_at_target);
}

TEST_CASE("get_next_step: already at target", "[helpers]") {
  bn::fixed_point from{20, 20};
  bn::fixed_point to{20, 20};
  bn::fixed speed = 3;
  auto result = ti::get_next_step(from, to, speed);
  REQUIRE(result.x() == 20);
  REQUIRE(result.y() == 20);
}

TEST_CASE("get_next_step: snap to when within 2 units", "[helpers]") {
  bn::fixed_point from{21, 20};
  bn::fixed_point to{20, 20};
  bn::fixed speed = 3;
  auto result = ti::get_next_step(from, to, speed);
  // Should snap exactly
  REQUIRE(result.x() == 20);
  REQUIRE(result.y() == 20);
}
