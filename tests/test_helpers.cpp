// test_helpers.cpp
// Unit tests for ti_helpers using Catch2, with mocks for Butano types.

#include <catch2/catch_all.hpp>
#include <cmath>  // for std::abs
#include <vector>

#include "ti_helpers.h"

TEST_CASE("get_next_step: normal movement toward target", "[helpers]") {
  bn::fixed_point from(10, 10);
  bn::fixed_point to(20, 20);
  bn::fixed speed(3);
  bn::fixed_point result = ti::get_next_step(from, to, speed);

  REQUIRE(bn::abs(result.x() - to.x()) < bn::abs(from.x() - to.x()));
  REQUIRE(bn::abs(result.y() - to.y()) < bn::abs(from.y() - to.y()));
  REQUIRE(!(result.x() == to.x() && result.y() == to.y()));
}

TEST_CASE("get_next_step: already at target", "[helpers]") {
  bn::fixed_point from(20, 20);
  bn::fixed_point to(20, 20);
  bn::fixed speed(3);
  bn::fixed_point result = ti::get_next_step(from, to, speed);

  REQUIRE(result.x() == 20);
  REQUIRE(result.y() == 20);
}

TEST_CASE("get_next_step: snap when close", "[helpers]") {
  bn::fixed_point from(21, 20);
  bn::fixed_point to(20, 20);
  bn::fixed speed(3);
  bn::fixed_point result = ti::get_next_step(from, to, speed);

  REQUIRE(result.x() == 20);
  REQUIRE(result.y() == 20);
}

// Place this after the get_next_step tests.
TEST_CASE("move_cursor_available: skips purchased items and respects bounds",
          "[cursor][helpers]") {
  using Vec = std::vector<bool>;
  // Unpurchased everywhere
  {
    Vec purchased = {false, false, false, false};
    REQUIRE(ti::move_cursor_available(1, +1, purchased) == 2);
    REQUIRE(ti::move_cursor_available(2, -1, purchased) == 1);
  }
  // Item at index 2 is purchased
  {
    Vec purchased = {false, false, true, false};
    REQUIRE(ti::move_cursor_available(1, +1, purchased) == 3);
    REQUIRE(ti::move_cursor_available(3, -1, purchased) == 1);
  }
  // Multiple purchased in a row
  {
    Vec purchased = {false, true, true, false};
    REQUIRE(ti::move_cursor_available(0, +1, purchased) == 3);
    REQUIRE(ti::move_cursor_available(3, -1, purchased) == 0);
  }
  // All in direction purchased: should stay
  {
    Vec purchased = {false, true, true, true};
    REQUIRE(ti::move_cursor_available(0, +1, purchased) == 0);
    REQUIRE(ti::move_cursor_available(3, -1, purchased) == 0);
  }
  // At bounds, cannot move further
  {
    Vec purchased = {false, false, false};
    REQUIRE(ti::move_cursor_available(0, -1, purchased) == 0);
    REQUIRE(ti::move_cursor_available(2, +1, purchased) == 2);
  }
}
