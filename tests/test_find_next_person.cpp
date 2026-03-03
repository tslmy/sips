#include <catch2/catch_all.hpp>
#include <vector>

#include "find_next_person.h"

namespace {

// Minimal types matching the expected interface
struct Fixed {
  int v;
  Fixed(int x = 0) : v(x) {}
  bool operator<(const Fixed &o) const { return v < o.v; }
  bool operator>(const Fixed &o) const { return v > o.v; }
};

struct Pos {
  Fixed x_val, y_val;
  Pos(Fixed x = Fixed(0), Fixed y = Fixed(0)) : x_val(x), y_val(y) {}
  Fixed x() const { return x_val; }
};

struct Person {
  Pos pos;
  bool visible = true;
  int id;
  Person(int x, int y, int id_) : pos(Fixed(x), Fixed(y)), id(id_) {}
  Pos get_shadow_position() const { return pos; }
  bool is_visible() const { return visible; }
};

}  // namespace

TEST_CASE("find_next_person: find closest to the left", "[find_next_person]") {
  std::vector<Person> people = {
      Person(-100, 0, 0), Person(-50, 0, 1), Person(0, 0, 2),
      Person(50, 0, 3),   Person(100, 0, 4),
  };
  Person *focused = &people[2];
  Person *result = find_next_person(people, focused, Fixed(0), true, 5);
  REQUIRE(result != nullptr);
  REQUIRE(result->id == 1);
}

TEST_CASE("find_next_person: find closest to the right", "[find_next_person]") {
  std::vector<Person> people = {
      Person(-100, 0, 0), Person(-50, 0, 1), Person(0, 0, 2),
      Person(50, 0, 3),   Person(100, 0, 4),
  };
  Person *focused = &people[2];
  Person *result = find_next_person(people, focused, Fixed(0), false, 5);
  REQUIRE(result != nullptr);
  REQUIRE(result->id == 3);
}

TEST_CASE("find_next_person: no one to the left returns nullptr",
          "[find_next_person]") {
  std::vector<Person> people = {Person(0, 0, 0), Person(50, 0, 1),
                                Person(100, 0, 2)};
  Person *focused = &people[0];
  Person *result = find_next_person(people, focused, Fixed(0), true, 3);
  REQUIRE(result == nullptr);
}

TEST_CASE("find_next_person: no one to the right returns nullptr",
          "[find_next_person]") {
  std::vector<Person> people = {Person(-100, 0, 0), Person(-50, 0, 1),
                                Person(0, 0, 2)};
  Person *focused = &people[2];
  Person *result = find_next_person(people, focused, Fixed(0), false, 3);
  REQUIRE(result == nullptr);
}

TEST_CASE("find_next_person: respects popularity level", "[find_next_person]") {
  std::vector<Person> people = {Person(-50, 0, 0), Person(0, 0, 1),
                                Person(50, 0, 2), Person(100, 0, 3)};
  Person *focused = &people[1];
  Person *result = find_next_person(people, focused, Fixed(0), false, 2);
  REQUIRE(result == nullptr);
}

TEST_CASE("find_next_person: skips invisible people", "[find_next_person]") {
  std::vector<Person> people = {Person(-100, 0, 0), Person(-50, 0, 1),
                                Person(0, 0, 2)};
  people[1].visible = false;
  Person *focused = &people[2];
  Person *result = find_next_person(people, focused, Fixed(0), true, 3);
  REQUIRE(result != nullptr);
  REQUIRE(result->id == 0);
}

TEST_CASE("find_next_person: empty vector returns nullptr",
          "[find_next_person]") {
  std::vector<Person> people;
  Person *result = find_next_person(people, nullptr, Fixed(0), true, 5);
  REQUIRE(result == nullptr);
}

TEST_CASE("find_next_person: single focused person returns nullptr",
          "[find_next_person]") {
  std::vector<Person> people = {Person(0, 0, 0)};
  Person *focused = &people[0];
  Person *result_left = find_next_person(people, focused, Fixed(0), true, 1);
  Person *result_right = find_next_person(people, focused, Fixed(0), false, 1);
  REQUIRE(result_left == nullptr);
  REQUIRE(result_right == nullptr);
}
