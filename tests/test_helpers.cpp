// test_helpers.cpp
// Unit tests for ti_helpers using Catch2.

#include <catch2/catch_all.hpp>
#include <cmath>  // for std::abs
#include <vector>

#include "ti_helpers.h"
#include "wishlist_logic.h"

TEST_CASE("get_next_step: normal movement toward target", "[helpers]") {
  bn::fixed_point from(10, 10);
  bn::fixed_point to(20, 20);
  bn::fixed speed(3);
  bn::fixed_point result = ti::get_next_step(from, to, speed);

  REQUIRE(std::abs((result.x() - to.x()).integer()) <
          std::abs((from.x() - to.x()).integer()));
  REQUIRE(std::abs((result.y() - to.y()).integer()) <
          std::abs((from.y() - to.y()).integer()));
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

TEST_CASE("wishlist_logic::first_unpurchased_index", "[wishlist][logic]") {
  using Vec = std::vector<bool>;

  SECTION("returns first available entry") {
    Vec purchased = {true, true, false, true};
    REQUIRE(wishlist::logic::first_unpurchased_index(purchased) == 2);
  }

  SECTION("falls back to 0 when all are purchased") {
    Vec purchased = {true, true, true};
    REQUIRE(wishlist::logic::first_unpurchased_index(purchased) == 0);
  }
}

TEST_CASE("wishlist_logic purchase checks", "[wishlist][logic]") {
  SECTION("selected price is zero when purchased") {
    REQUIRE(wishlist::logic::selected_price(true, 55) == 0);
    REQUIRE(wishlist::logic::selected_price(false, 55) == 55);
  }

  SECTION("can_purchase enforces purchase and cash constraints") {
    REQUIRE(wishlist::logic::can_purchase(false, 30, 30));
    REQUIRE(wishlist::logic::can_purchase(false, 30, 50));
    REQUIRE_FALSE(wishlist::logic::can_purchase(false, 30, 29));
    REQUIRE_FALSE(wishlist::logic::can_purchase(true, 30, 999));
    REQUIRE_FALSE(wishlist::logic::can_purchase(false, 0, 999));
  }
}
