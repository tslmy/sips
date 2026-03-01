// test_helpers.cpp
// Unit tests for ti_helpers using Catch2, with mocks for Butano types.

#include <catch2/catch_all.hpp>
#include <cmath>  // for std::abs
#include <vector>

#include "cursor_helpers.h"

// Place this after the get_next_step tests.
TEST_CASE("move_cursor: skips purchased items and respects bounds",
          "[cursor][helpers]") {
  using Vec = std::vector<int>;
  // Unpurchased everywhere
  {
    Vec prices = {10, 20, 30, 40};
    REQUIRE(ti::move_cursor(1, +1, prices) == 2);
    REQUIRE(ti::move_cursor(2, -1, prices) == 1);
  }
  // Item at index 2 is purchased
  {
    Vec prices = {10, 20, 0, 40};
    REQUIRE(ti::move_cursor(1, +1, prices) == 3);  // should skip 2
    REQUIRE(ti::move_cursor(3, -1, prices) == 1);  // should skip 2
  }
  // Multiple purchased in a row
  {
    Vec prices = {10, 0, 0, 40};
    REQUIRE(ti::move_cursor(0, +1, prices) == 3);
    REQUIRE(ti::move_cursor(3, -1, prices) == 0);
  }
  // All in direction purchased: should stay
  {
    Vec prices = {10, 0, 0, 0};
    REQUIRE(ti::move_cursor(0, +1, prices) == 0);
    REQUIRE(ti::move_cursor(3, -1, prices) == 0);
  }
  // At bounds, cannot move further
  {
    Vec prices = {10, 20, 30};
    REQUIRE(ti::move_cursor(0, -1, prices) == 0);
    REQUIRE(ti::move_cursor(2, +1, prices) == 2);
  }
}
