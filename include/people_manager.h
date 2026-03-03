#ifndef PEOPLE_MANAGER_H
#define PEOPLE_MANAGER_H

#include "bn_fixed.h"
#include "bn_vector.h"
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

/**
 * @brief Finds the next person in a specific direction (left or right).
 * @param people The vector of all people.
 * @param focused_person The currently focused person.
 * @param current_x The x position to search from.
 * @param going_left True to search left, false to search right.
 * @param popularity_level The current popularity level (determines active
 * people).
 * @return Pointer to the next visible person in the specified direction,
 *         or nullptr if no one is found.
 */
ti::Person *find_next_person(bn::vector<ti::Person, 16> &people,
                             ti::Person *focused_person, bn::fixed current_x,
                             bool going_left, int popularity_level);

}  // namespace people_manager

#endif
