#include "people_manager.h"

#include "bn_algorithm.h"
#include "ti_person.h"

namespace people_manager {

void initialize_people(bn::vector<ti::Person, 16> &people) {
  people.clear();
  for (int i = 0; i < 10; ++i) {
    const ti::START start = i % 2 == 0 ? ti::START::RIGHT : ti::START::LEFT;
    people.push_back(ti::Person(start, ti::TYPE::GREEN_SHIRT, i));
  }
}

void rebuild_available_types(const bn::vector<ti::Person, 16> &people,
                             int popularity_level, bn::vector<int, 16> &types) {
  types.clear();
  for (int i = 0; i < 14; ++i) {
    types.push_back(i);
  }

  const int active_people = bn::min(popularity_level, people.size());
  for (int i = 0; i < active_people; ++i) {
    const int used_type = int(people.at(i).get_type());
    for (int j = 0; j < types.size(); ++j) {
      if (types.at(j) == used_type) {
        types.erase(types.begin() + j);
        break;
      }
    }
  }
}

ti::Person *find_next_person(bn::vector<ti::Person, 16> &people,
                             ti::Person *focused_person, bn::fixed current_x,
                             bool going_left, int popularity_level) {
  ti::Person *next_person = nullptr;
  bn::fixed closest_x = going_left ? bn::fixed(-32767) : bn::fixed(32767);

  for (int i = 0; i < people.size(); i++) {
    if (popularity_level > i && people.at(i).is_visible() &&
        &people.at(i) != focused_person) {
      bn::fixed person_x = people.at(i).get_shadow_position().x();
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

}  // namespace people_manager
