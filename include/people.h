#ifndef PEOPLE_H
#define PEOPLE_H

#include "bn_vector.h"

// Forward declarations
namespace ti {
class Person;
}

namespace people_manager {

/**
 * @brief Finds the next person in a specific direction (left or right).
 *
 * Template function that works with any container and element type that:
 * - Has a get_shadow_position() method returning something with x() method
 * - Has an is_visible() method
 *
 * @tparam Container Container type (like bn::vector<T, N>)
 * @param people The container of all people.
 * @param focused_person Pointer to the currently focused person.
 * @param current_x The x position to search from.
 * @param going_left True to search left, false to search right.
 * @param popularity_level The current popularity level (determines active
 * people).
 * @return Pointer to the next visible person in the specified direction,
 *         or nullptr if no one is found.
 */
template <typename Container>
typename Container::value_type *find_next_person(
    Container &people, typename Container::value_type *focused_person,
    auto current_x, bool going_left, int popularity_level) {
  using PersonType = typename Container::value_type;
  PersonType *next_person = nullptr;
  auto closest_x =
      going_left ? decltype(current_x)(-32767) : decltype(current_x)(32767);

  for (int i = 0; i < int(people.size()); i++) {
    if (popularity_level > i && people.at(i).is_visible() &&
        &people.at(i) != focused_person) {
      auto person_x = people.at(i).get_shadow_position().x();
      if (going_left) {
        if (person_x < current_x && person_x > closest_x) {
          closest_x = person_x;
          next_person = &people.at(i);
        }
      } else {
        if (person_x > current_x && person_x < closest_x) {
          closest_x = person_x;
          next_person = &people.at(i);
        }
      }
    }
  }
  return next_person;
}

/**
 * @brief Rebuilds the list of available character types based on current
 * popularity level and active people.
 */
void rebuild_available_types(const bn::vector<ti::Person, 16> &people,
                             int popularity_level, bn::vector<int, 16> &types);

/**
 * @brief Generic template to remove used types from available types.
 *
 * Given a container of people and a container of available types,
 * removes any types that are currently in use by active people.
 *
 * Requires:
 * - PeopleContainer::value_type has get_type() method
 * - TypesContainer has erase(iterator) and comparable value_type
 * - Types must be comparable with ==
 *
 * @tparam PeopleContainer Container of people (e.g., bn::vector<Person, N>)
 * @tparam TypesContainer Container of available types (e.g., bn::vector<int,
 * M>)
 * @param people The container of all people.
 * @param popularity_level The number of active people to check.
 * @param types The container of available types (modified in-place).
 */
template <typename PeopleContainer, typename TypesContainer>
void rebuild_available_types_generic(const PeopleContainer &people,
                                     int popularity_level,
                                     TypesContainer &types) {
  const int active_people =
      popularity_level < int(people.size()) ? popularity_level : people.size();

  for (int i = 0; i < active_people; ++i) {
    const auto used_type = people.at(i).get_type();
    for (int j = 0; j < int(types.size()); ++j) {
      if (types.at(j) == used_type) {
        types.erase(types.begin() + j);
        break;
      }
    }
  }
}

}  // namespace people_manager

#endif
