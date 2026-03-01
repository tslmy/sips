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

TEST_CASE("wishlist_logic::first_unpurchased_index", "[wishlist][logic]") {
  using Vec = std::vector<bool>;

  SECTION("returns first available entry") {
    Vec purchased = {true, true, false, true};
    REQUIRE(wishlist::logic::first_unpurchased_index(purchased) == 2);
  }

  SECTION("falls back to -1 when all are purchased") {
    Vec purchased = {true, true, true};
    REQUIRE(wishlist::logic::first_unpurchased_index(purchased) == -1);
  }
}

TEST_CASE("wishlist_logic::attempt_purchase branches", "[wishlist][logic]") {
  SECTION("successful purchase updates state") {
    const auto result = wishlist::logic::attempt_purchase(false, 40, 100, 3);
    REQUIRE(result.outcome == wishlist::logic::PurchaseOutcome::purchased);
    REQUIRE(result.cash_after == 60);
    REQUIRE(result.popularity_after == 4);
    REQUIRE(result.purchased_after);
    REQUIRE(result.should_close_menu);
    REQUIRE_FALSE(result.should_start_cursor_shake);
  }

  SECTION("already purchased keeps state unchanged") {
    const auto result = wishlist::logic::attempt_purchase(true, 40, 100, 3);
    REQUIRE(result.outcome ==
            wishlist::logic::PurchaseOutcome::already_purchased);
    REQUIRE(result.cash_after == 100);
    REQUIRE(result.popularity_after == 3);
    REQUIRE(result.purchased_after);
    REQUIRE_FALSE(result.should_close_menu);
    REQUIRE_FALSE(result.should_start_cursor_shake);
  }

  SECTION("insufficient funds triggers shake") {
    const auto result = wishlist::logic::attempt_purchase(false, 40, 39, 3);
    REQUIRE(result.outcome ==
            wishlist::logic::PurchaseOutcome::insufficient_funds);
    REQUIRE(result.cash_after == 39);
    REQUIRE(result.popularity_after == 3);
    REQUIRE_FALSE(result.purchased_after);
    REQUIRE_FALSE(result.should_close_menu);
    REQUIRE(result.should_start_cursor_shake);
  }

  SECTION("invalid price is rejected without shake") {
    const auto result = wishlist::logic::attempt_purchase(false, 0, 100, 3);
    REQUIRE(result.outcome == wishlist::logic::PurchaseOutcome::invalid_price);
    REQUIRE(result.cash_after == 100);
    REQUIRE(result.popularity_after == 3);
    REQUIRE_FALSE(result.purchased_after);
    REQUIRE_FALSE(result.should_close_menu);
    REQUIRE_FALSE(result.should_start_cursor_shake);
  }
}
