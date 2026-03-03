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

}  // namespace people_manager
