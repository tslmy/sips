#ifndef PEOPLE_MANAGER_H
#define PEOPLE_MANAGER_H

#include "bn_fixed.h"
#include "bn_vector.h"
#include "find_next_person.h"
#include "ti_person.h"

namespace people_manager {

/**
 * @brief Initializes the people vector with 10 default customers.
 */
void initialize_people(bn::vector<ti::Person, 16> &people);

/**
 * @brief Rebuilds the list of available character types based on current
 * popularity level and active people.
 */
void rebuild_available_types(const bn::vector<ti::Person, 16> &people,
                             int popularity_level, bn::vector<int, 16> &types);

// Template is defined in find_next_person.h and available as find_next_person
using ::find_next_person;

}  // namespace people_manager

#endif
