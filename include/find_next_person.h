#ifndef FIND_NEXT_PERSON_H
#define FIND_NEXT_PERSON_H

/**
 * @brief Generic template to find the next person in a specific direction.
 *
 * This template works with any container and element type that:
 * - Has a get_shadow_position() method returning something with x() method
 * - Has an is_visible() method
 *
 * @tparam Container Container type (like bn::vector<T, N> or std::vector<T>)
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

#endif
