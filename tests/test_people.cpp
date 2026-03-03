// test_people.cpp
// Unit tests for people management using Catch2.

#include <catch2/catch_all.hpp>
#include <vector>

#include "people.h"

namespace {

using people_manager::find_next_person;

// Test types for find_next_person
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

struct PersonSpatial {
  Pos pos;
  bool visible = true;
  int id;
  PersonSpatial(int x, int y, int id_) : pos(Fixed(x), Fixed(y)), id(id_) {}
  Pos get_shadow_position() const { return pos; }
  bool is_visible() const { return visible; }
};

// Test types for rebuild_available_types
struct Type {
  int value;
  Type(int v) : value(v) {}
  bool operator==(const Type &other) const { return value == other.value; }
};

struct PersonType {
  Type type;
  int id;
  PersonType(Type t, int id_) : type(t), id(id_) {}
  Type get_type() const { return type; }
};

}  // namespace

// ============================================================================
// Tests for find_next_person
// ============================================================================

TEST_CASE("find_next_person: find closest to the left", "[people]") {
  std::vector<PersonSpatial> people = {
      PersonSpatial(-100, 0, 0), PersonSpatial(-50, 0, 1),
      PersonSpatial(0, 0, 2),    PersonSpatial(50, 0, 3),
      PersonSpatial(100, 0, 4),
  };
  PersonSpatial *focused = &people[2];
  PersonSpatial *result = find_next_person(people, focused, Fixed(0), true, 5);
  REQUIRE(result != nullptr);
  REQUIRE(result->id == 1);
}

TEST_CASE("find_next_person: find closest to the right", "[people]") {
  std::vector<PersonSpatial> people = {
      PersonSpatial(-100, 0, 0), PersonSpatial(-50, 0, 1),
      PersonSpatial(0, 0, 2),    PersonSpatial(50, 0, 3),
      PersonSpatial(100, 0, 4),
  };
  PersonSpatial *focused = &people[2];
  PersonSpatial *result = find_next_person(people, focused, Fixed(0), false, 5);
  REQUIRE(result != nullptr);
  REQUIRE(result->id == 3);
}

TEST_CASE("find_next_person: no one to the left returns nullptr", "[people]") {
  std::vector<PersonSpatial> people = {PersonSpatial(0, 0, 0),
                                       PersonSpatial(50, 0, 1),
                                       PersonSpatial(100, 0, 2)};
  PersonSpatial *focused = &people[0];
  PersonSpatial *result = find_next_person(people, focused, Fixed(0), true, 3);
  REQUIRE(result == nullptr);
}

TEST_CASE("find_next_person: no one to the right returns nullptr", "[people]") {
  std::vector<PersonSpatial> people = {PersonSpatial(-100, 0, 0),
                                       PersonSpatial(-50, 0, 1),
                                       PersonSpatial(0, 0, 2)};
  PersonSpatial *focused = &people[2];
  PersonSpatial *result = find_next_person(people, focused, Fixed(0), false, 3);
  REQUIRE(result == nullptr);
}

TEST_CASE("find_next_person: respects popularity level", "[people]") {
  std::vector<PersonSpatial> people = {
      PersonSpatial(-50, 0, 0), PersonSpatial(0, 0, 1), PersonSpatial(50, 0, 2),
      PersonSpatial(100, 0, 3)};
  PersonSpatial *focused = &people[1];
  PersonSpatial *result = find_next_person(people, focused, Fixed(0), false, 2);
  REQUIRE(result == nullptr);
}

TEST_CASE("find_next_person: skips invisible people", "[people]") {
  std::vector<PersonSpatial> people = {PersonSpatial(-100, 0, 0),
                                       PersonSpatial(-50, 0, 1),
                                       PersonSpatial(0, 0, 2)};
  people[1].visible = false;
  PersonSpatial *focused = &people[2];
  PersonSpatial *result = find_next_person(people, focused, Fixed(0), true, 3);
  REQUIRE(result != nullptr);
  REQUIRE(result->id == 0);
}

TEST_CASE("find_next_person: empty vector returns nullptr", "[people]") {
  std::vector<PersonSpatial> people;
  PersonSpatial *result = find_next_person(people, nullptr, Fixed(0), true, 5);
  REQUIRE(result == nullptr);
}

TEST_CASE("find_next_person: single focused person returns nullptr",
          "[people]") {
  std::vector<PersonSpatial> people = {PersonSpatial(0, 0, 0)};
  PersonSpatial *focused = &people[0];
  PersonSpatial *result_left =
      find_next_person(people, focused, Fixed(0), true, 1);
  PersonSpatial *result_right =
      find_next_person(people, focused, Fixed(0), false, 1);
  REQUIRE(result_left == nullptr);
  REQUIRE(result_right == nullptr);
}

// ============================================================================
// Tests for rebuild_available_types
// ============================================================================

TEST_CASE("rebuild_available_types: removes used types", "[people]") {
  std::vector<PersonType> people = {PersonType(Type(1), 0),
                                    PersonType(Type(3), 1)};
  std::vector<Type> types = {Type(0), Type(1), Type(2), Type(3), Type(4)};

  // Use the generic template for testing
  ::people_manager::rebuild_available_types_generic(people, 2, types);

  REQUIRE(types.size() == 3);
  REQUIRE(types[0].value == 0);
  REQUIRE(types[1].value == 2);
  REQUIRE(types[2].value == 4);
}

TEST_CASE("rebuild_available_types: respects popularity level", "[people]") {
  std::vector<PersonType> people = {
      PersonType(Type(0), 0), PersonType(Type(1), 1), PersonType(Type(2), 2)};
  std::vector<Type> types = {Type(0), Type(1), Type(2), Type(3)};

  // Only check first 2 people
  ::people_manager::rebuild_available_types_generic(people, 2, types);

  REQUIRE(types.size() == 2);
  REQUIRE(types[0].value == 2);
  REQUIRE(types[1].value == 3);
}

TEST_CASE("rebuild_available_types: handles empty people", "[people]") {
  std::vector<PersonType> people;
  std::vector<Type> types = {Type(0), Type(1), Type(2)};

  ::people_manager::rebuild_available_types_generic(people, 0, types);

  REQUIRE(types.size() == 3);
}

TEST_CASE("rebuild_available_types: handles empty types", "[people]") {
  std::vector<PersonType> people = {PersonType(Type(1), 0)};
  std::vector<Type> types;

  ::people_manager::rebuild_available_types_generic(people, 1, types);

  REQUIRE(types.size() == 0);
}

TEST_CASE("rebuild_available_types: removes all used types", "[people]") {
  std::vector<PersonType> people = {
      PersonType(Type(0), 0), PersonType(Type(1), 1), PersonType(Type(2), 2)};
  std::vector<Type> types = {Type(0), Type(1), Type(2)};

  ::people_manager::rebuild_available_types_generic(people, 3, types);

  REQUIRE(types.size() == 0);
}
