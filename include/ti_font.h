/**
 * @file ti_font.h
 * @brief Defines the variable-width 8x8 sprite font and associated character
 * metadata for in-game text rendering.
 *
 * This module declares the character set, per-character widths, and a sprite
 * font object for use with Butano's sprite text system. Supports accented,
 * special, and Greek characters.
 *
 * Usage: Used throughout the game for dialogue, UI text, and numbers rendered
 * as sprites. Font asset source: see graphics/font.png and asset pipeline.
 *
 * Copyright (c) 2020-2021 Gustavo Valiente gustavo.valiente@protonmail.com
 * zlib License, see LICENSE file.
 */

#ifndef TI_VARIABLE_8x8_SPRITE_FONT_H
#define TI_VARIABLE_8x8_SPRITE_FONT_H

#include "bn_sprite_font.h"
#include "bn_sprite_items_font.h"
#include "bn_utf8_characters_map.h"

namespace ti {

/**
 * @brief Supported UTF-8 character overrides for the font.
 * Use this for special accented and Greek characters with unique tiles.
 */
constexpr bn::utf8_character variable_8x8_sprite_font_utf8_characters[] = {
    "á", "é", "í", "ó", "ú", "ü", "ñ", "¡", "¿", "α", "β"};

/**
 * @brief Per-character pixel widths for main font.
 * Width order matches font character mapping. Used for variable-width sprite
 * text rendering.
 */
constexpr int8_t variable_8x8_sprite_font_character_widths[] = {
    8, 3, 5, 7, 7, 8, 7, 3, 6, 6, 7, 7, 4, 7, 3, 6, 8, 7, 8, 8, 8, 8,
    8, 8, 8, 8, 4, 4, 5, 7, 5, 7, 8, 7, 7, 7, 7, 7, 7, 7, 7, 3, 7, 7,
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 4, 6, 4, 7, 7, 3, 7,
    7, 7, 7, 7, 6, 7, 7, 3, 5, 7, 4, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
    7, 7, 7, 5, 3, 5, 7, 7, 7, 4, 7, 7, 7, 7, 3, 7, 8, 8,
};

constexpr bn::span<const bn::utf8_character>
    variable_8x8_sprite_font_utf8_characters_span(
        variable_8x8_sprite_font_utf8_characters);

constexpr auto variable_8x8_sprite_font_utf8_characters_map =
    bn::utf8_characters_map<variable_8x8_sprite_font_utf8_characters_span>();

/**
 * @brief The actual Butano sprite font object for this project.
 * Pass to BN APIs to use variable-width in-game text.
 */
constexpr bn::sprite_font variable_8x8_sprite_font(
    bn::sprite_items::font,
    variable_8x8_sprite_font_utf8_characters_map.reference(),
    variable_8x8_sprite_font_character_widths);
}  // namespace ti

#endif