#include "people.h"

#include "ti_person.h"

namespace people_manager {

void rebuild_available_types(const bn::vector<ti::Person, 16> &people,
                             int popularity_level, bn::vector<int, 16> &types) {
  types.clear();
  for (int i = 0; i < 14; ++i) {
    types.push_back(i);
  }

  const int active_people =
      popularity_level < int(people.size()) ? popularity_level : people.size();

  for (int i = 0; i < active_people; ++i) {
    const int used_type = int(people.at(i).get_type());
    for (int j = 0; j < int(types.size()); ++j) {
      if (types.at(j) == used_type) {
        types.erase(types.begin() + j);
        break;
      }
    }
  }
}

}  // namespace people_manager
