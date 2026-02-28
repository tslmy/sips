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

// ------ Copy-paste or #include the logic from your ti_helpers.cpp here:
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
TEST_CASE("get_next_step moves towards target", "[helpers]") {
  bn::fixed_point from{0, 0};
  bn::fixed_point to{10, 0};
  bn::fixed speed = 1.0f;
  auto next = ti::get_next_step(from, to, speed);
  // Test that we move closer to 'to'
  REQUIRE(next.x() != from.x());
}
