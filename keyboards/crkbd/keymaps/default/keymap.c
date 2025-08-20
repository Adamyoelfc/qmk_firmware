/*
Copyright 2019 @foostan
Copyright 2020 Drashna Jaelre <@drashna>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.


command to compile:
qmk compile -kb crkbd/r2g -km default
*/

#include QMK_KEYBOARD_H

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    // Base layer - QWERTY
    [0] = LAYOUT_split_3x6_3(
    // Left Hand (Columns 0-5)                                                                  // Right Hand (Columns 6-11)
        KC_TAB,  KC_QUOT, KC_COMM, KC_DOT,  KC_P,    KC_Y,         KC_F,    KC_G,    KC_C,    KC_R,    KC_L,    KC_BSPC,
        KC_LCTL, KC_A,    KC_O,    KC_E,    KC_U,    KC_I,         KC_D,    KC_H,    KC_T,    KC_N,    KC_S,    KC_MINS,
        KC_LSFT, KC_SCLN, KC_Q,    KC_J,    KC_K,    KC_X,         KC_B,    KC_M,    KC_W,    KC_V,    KC_Z,    KC_ESC,
                                            KC_LGUI, MO(1),  KC_SPC,  KC_ENT,  MO(2),   KC_RALT
    ),

    // Numbers and navigation layer
    [1] = LAYOUT_split_3x6_3(
        KC_TAB,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,         KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_BSPC,
        KC_DEL, LCSG(KC_1), LCSG(KC_2), LCSG(KC_W), LCSG(KC_D), LCSG(KC_T), KC_PDOT, KC_DOWN, KC_UP,   KC_LEFT, KC_RGHT, KC_PGUP,
        KC_LSFT, LCSG(KC_0), LCSG(KC_9), LCSG(KC_6), LCSG(KC_7), LCSG(KC_8), KC_PCMM, LCSG(KC_F), LCSG(KC_H), LCSG(KC_N), LCSG(KC_S), KC_PGDN,
                                            KC_LGUI, KC_TRNS, KC_SPC,  KC_ENT,  MO(3),   KC_RALT
    ),

    // Symbols layer
    [2] = LAYOUT_split_3x6_3(
        KC_TAB,  KC_EXLM, KC_AT,   KC_HASH, KC_DLR,  KC_PERC,      KC_CIRC, KC_AMPR, KC_ASTR, KC_LPRN, KC_RPRN, KC_BSPC,
        KC_LCTL, LCG(KC_SPC), LCSG(KC_I), LCSG(KC_O), LCSG(KC_P), LSG(KC_5), KC_PSLS, KC_PEQL, LSFT_T(KC_LBRC), KC_RBRC, KC_BSLS, KC_GRV,
        KC_LSFT, KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,        KC_QUES, KC_PLUS, KC_LCBR, KC_RCBR, KC_PIPE, KC_TILD,
                                            KC_LGUI, MO(3),  KC_SPC,  KC_ENT,  KC_TRNS, KC_RALT
    ),

    // Function and media layer
    [3] = LAYOUT_split_3x6_3(
        KC_TAB,  KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,        KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,
        KC_NO,   KC_BRIU, KC_MNXT, KC_MPLY, KC_VOLU, KC_NO,        KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,
        KC_NO,   RM_VALD, KC_MPRV, KC_MUTE, KC_VOLD, KC_NO,        BL_TOGG, LM_TOGG, UG_TOGG, RM_TOGG, KC_NO,   KC_NO,
                                            KC_LGUI, KC_TRNS, KC_SPC,  KC_ENT,  KC_TRNS, KC_RALT
    )
};

#ifdef ENCODER_MAP_ENABLE
const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][NUM_DIRECTIONS] = {
  [0] = { ENCODER_CCW_CW(KC_VOLD, KC_VOLU), ENCODER_CCW_CW(KC_MPRV, KC_MNXT), ENCODER_CCW_CW(RM_VALD, RM_VALU), ENCODER_CCW_CW(KC_RGHT, KC_LEFT), },
  [1] = { ENCODER_CCW_CW(KC_VOLD, KC_VOLU), ENCODER_CCW_CW(KC_MPRV, KC_MNXT), ENCODER_CCW_CW(RM_VALD, RM_VALU), ENCODER_CCW_CW(KC_RGHT, KC_LEFT), },
  [2] = { ENCODER_CCW_CW(KC_VOLD, KC_VOLU), ENCODER_CCW_CW(KC_MPRV, KC_MNXT), ENCODER_CCW_CW(RM_VALD, RM_VALU), ENCODER_CCW_CW(KC_RGHT, KC_LEFT), },
  [3] = { ENCODER_CCW_CW(KC_VOLD, KC_VOLU), ENCODER_CCW_CW(KC_MPRV, KC_MNXT), ENCODER_CCW_CW(RM_VALD, RM_VALU), ENCODER_CCW_CW(KC_RGHT, KC_LEFT), },
};
#endif
