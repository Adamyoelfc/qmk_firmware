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
#include "transactions.h"

enum custom_keycodes {
    ANIM_NEXT = SAFE_RANGE,
};

typedef struct { uint8_t anim_idx; } user_sync_t;

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
        KC_LALT,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,         KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_BSPC,
        KC_LCTL, LCSG(KC_1), LCSG(KC_2), LCSG(KC_W), LCSG(KC_D), LCSG(KC_T), KC_PDOT, KC_DOWN, KC_UP,   KC_LEFT, KC_RGHT, KC_PGUP,
        KC_LSFT, LCSG(KC_0), LCSG(KC_9), LCSG(KC_6), LCSG(KC_7), LCSG(KC_8), KC_PCMM, LCSG(KC_F), LCSG(KC_H), LCSG(KC_N), LCSG(KC_S), KC_PGDN,
                                            KC_LGUI, KC_TRNS, KC_SPC,  KC_ENT,  MO(3),   KC_RALT
    ),

    // Symbols layer
    [2] = LAYOUT_split_3x6_3(
        KC_TAB,  KC_EXLM, KC_AT,   KC_HASH, KC_DLR,  KC_PERC,      KC_CIRC, KC_AMPR, KC_ASTR, KC_LPRN, KC_RPRN, KC_DEL,
        KC_LCTL, LCG(KC_SPC), LCSG(KC_I), LCSG(KC_O), LCSG(KC_P), LSG(KC_5), KC_PSLS, KC_PEQL, LSFT_T(KC_LBRC), KC_RBRC, KC_BSLS, KC_GRV,
        KC_LSFT, KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,        KC_QUES, KC_PLUS, KC_LCBR, KC_RCBR, KC_PIPE, KC_TILD,
                                            KC_LGUI, MO(3),  KC_SPC,  KC_ENT,  KC_TRNS, KC_RALT
    ),

    // Function and media layer
    [3] = LAYOUT_split_3x6_3(
        KC_TAB,  KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,        KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,
        KC_NO,   KC_BRIU, KC_MNXT, KC_MPLY, KC_VOLU, KC_NO,        ANIM_NEXT, KC_NO, KC_NO,   KC_NO,   KC_NO,   KC_NO,
        KC_NO,   RM_VALD, KC_MPRV, KC_MUTE, KC_VOLD, KC_NO,        RM_TOGG, RM_NEXT, RM_HUEU, RM_SPDU, KC_NO,   KC_NO,
                                            KC_LGUI, KC_TRNS, KC_SPC,  KC_ENT,  KC_TRNS, KC_RALT
    )
};

#ifdef OLED_ENABLE
extern const unsigned char font[] PROGMEM;

static uint16_t last_frame   = 0;
static uint8_t  frame_count  = 0;
static uint16_t last_kc      = 0;
static uint16_t key_count    = 0;

static void put_px(int16_t x, int16_t y) {
    if (x < 0 || x >= 128 || y < 0 || y >= 32) return;
    oled_write_pixel((uint8_t)x, (uint8_t)y, true);
}

static void clear_px(int16_t x, int16_t y) {
    if (x < 0 || x >= 128 || y < 0 || y >= 32) return;
    oled_write_pixel((uint8_t)x, (uint8_t)y, false);
}

static void hline(int16_t x0, int16_t x1, int16_t y) {
    for (int16_t x = x0; x <= x1; x++) put_px(x, y);
}

static void vline(int16_t x, int16_t y0, int16_t y1) {
    for (int16_t y = y0; y <= y1; y++) put_px(x, y);
}

// ---- Matrix-style code rain on the slave OLED (full screen) ----
#define RAIN_COLS 18
static int8_t  rain_y[RAIN_COLS];
static uint8_t rain_speed[RAIN_COLS];
static uint8_t rain_chars[RAIN_COLS][3];
static bool    rain_init_done = false;
static uint16_t rain_lfsr = 0xACE1u;

static const char rain_palette[] = "0123456789ABCDEF{}<>;+=()*/";
#define RAIN_PALETTE_LEN (sizeof(rain_palette) - 1)

static uint8_t rain_rand(void) {
    uint16_t bit = (rain_lfsr ^ (rain_lfsr >> 2) ^ (rain_lfsr >> 3) ^ (rain_lfsr >> 5)) & 1;
    rain_lfsr = (rain_lfsr >> 1) | (bit << 15);
    return rain_lfsr & 0xFF;
}

static uint8_t rain_pick_char(void) {
    return (uint8_t)rain_palette[rain_rand() % RAIN_PALETTE_LEN];
}

static void draw_glyph(uint8_t c, int16_t x, int16_t y, uint8_t scale);

static void rain_init(void) {
    for (uint8_t i = 0; i < RAIN_COLS; i++) {
        rain_y[i]     = -(int8_t)(rain_rand() & 0x1F);
        rain_speed[i] = 1 + (rain_rand() & 1);
        for (uint8_t j = 0; j < 3; j++) {
            rain_chars[i][j] = rain_pick_char();
        }
    }
    rain_init_done = true;
}

static void rain_step_and_draw(void) {
    if (!rain_init_done) rain_init();

    for (uint8_t i = 0; i < RAIN_COLS; i++) {
        rain_y[i] += rain_speed[i];

        // shimmer: occasionally swap a char in the trail
        if ((rain_rand() & 0x0F) == 0) {
            rain_chars[i][rain_rand() % 3] = rain_pick_char();
        }

        // recycle when the whole 3-char trail has fallen off
        if (rain_y[i] - 16 > 32) {
            rain_y[i]     = -16 - (int8_t)(rain_rand() & 0x0F);
            rain_speed[i] = 1 + (rain_rand() & 1);
            for (uint8_t j = 0; j < 3; j++) {
                rain_chars[i][j] = rain_pick_char();
            }
        }

        int16_t x  = i * 7;
        int16_t hy = rain_y[i];

        // 3 stacked chars: head at hy, two trail chars above
        for (uint8_t j = 0; j < 3; j++) {
            int16_t y = hy - j * 8;
            draw_glyph(rain_chars[i][j], x, y, 1);
        }

        // bright underline at the head
        if (hy + 7 >= 0 && hy + 7 < 32) {
            hline(x, x + 4, hy + 7);
        }
    }
}

// ---- Animation 2: coffee mug with steam ----
static void draw_coffee_mug(int16_t cx, uint8_t phase) {
    const int16_t xl = cx - 22, xr = cx + 22;
    const int16_t yt = 7, yb = 33;
    for (int8_t s = 0; s < 3; s++) {
        int16_t base_x = cx - 14 + s * 14;
        for (int16_t y = -10; y <= yt - 3; y++) {
            int16_t t   = (int16_t)phase - y * 2 + s * 9;
            int8_t  wob = ((t >> 1) & 3) - 1;
            if ((t & 3) != 0) put_px(base_x + wob, y);
        }
    }
    vline(xl, yt + 1, yb - 1);     vline(xl + 1, yt + 1, yb - 1);
    vline(xr, yt + 1, yb - 1);     vline(xr - 1, yt + 1, yb - 1);
    hline(xl + 2, xr - 2, yb);     hline(xl + 1, xr - 1, yb - 1);
    const int16_t hx = xr + 7, hy = 18;
    for (int16_t dy = -9; dy <= 9; dy++) {
        for (int16_t dx = -2; dx <= 9; dx++) {
            int32_t d2 = (int32_t)dx * dx + (int32_t)dy * dy;
            if (d2 <= 81 && d2 >= 36) put_px(hx + dx, hy + dy);
        }
    }
    for (int16_t x = xl + 2; x <= xr - 2; x++) {
        int8_t  wave_t = (int8_t)(((x + phase) >> 1) & 3);
        int16_t top    = yt + 1 + (wave_t == 0 || wave_t == 3 ? 0 : 1);
        for (int16_t y = top; y <= yt + 5; y++) put_px(x, y);
    }
    for (int8_t i = 0; i < 2; i++) {
        int16_t gx = xl + 6 + i * 16 + ((phase >> 1) % 6);
        if (gx > xl + 2 && gx < xr - 2) { clear_px(gx, yt + 3); clear_px(gx + 1, yt + 3); }
    }
    oled_set_cursor(6, 2);
    oled_write_P(PSTR("COFFEE"), false);
}

static void anim_coffee_step(void) {
    int16_t sway = ((frame_count >> 2) & 3);
    int16_t cx   = 56 + (sway == 1 ? 1 : sway == 3 ? -1 : 0);
    draw_coffee_mug(cx, frame_count);
}

// ---- Animation 3: oscilloscope wave ----
static void anim_wave_step(void) {
    for (int16_t x = 0; x < 128; x += 2) {
        uint8_t p = (x + frame_count * 3) & 31;
        int8_t tri = p < 16 ? p : 31 - p;
        int16_t y = 16 + tri - 8;
        put_px(x, y);
        put_px(x + 1, y);
        if ((x & 15) == 0) vline(x, 16, y);
    }
    hline(0, 127, 16);
}

// ---- Animation 4: bouncing ladybug ----
static int8_t lady_x = 50, lady_y = 15, lady_vx = 2, lady_vy = 1;

static void clear_spot(int16_t x, int16_t y) {
    clear_px(x, y - 2);
    clear_px(x - 1, y - 1); clear_px(x, y - 1); clear_px(x + 1, y - 1);
    clear_px(x - 2, y);     clear_px(x - 1, y); clear_px(x, y); clear_px(x + 1, y); clear_px(x + 2, y);
    clear_px(x - 1, y + 1); clear_px(x, y + 1); clear_px(x + 1, y + 1);
    clear_px(x, y + 2);
}

static void anim_bug_step(void) {
    lady_x += lady_vx;
    lady_y += lady_vy;
    if (lady_x <= 34) { lady_x = 34; lady_vx = 2; }
    if (lady_x >= 96) { lady_x = 96; lady_vx = -2; }
    if (lady_y <= 13) { lady_y = 13; lady_vy = 1; }
    if (lady_y >= 18) { lady_y = 18; lady_vy = -1; }

    int16_t x = lady_x;
    int16_t y = lady_y;
    int8_t  leg = (frame_count & 4) ? 3 : -3;

    hline(x - 18, x + 12, y - 11);
    hline(x - 23, x + 17, y - 9);
    hline(x - 27, x + 21, y - 7);
    hline(x - 29, x + 23, y - 5);
    hline(x - 30, x + 24, y - 3);
    hline(x - 31, x + 25, y - 1);
    hline(x - 31, x + 25, y + 1);
    hline(x - 30, x + 24, y + 3);
    hline(x - 29, x + 23, y + 5);
    hline(x - 27, x + 21, y + 7);
    hline(x - 23, x + 17, y + 9);
    hline(x - 18, x + 12, y + 11);

    for (int8_t yy = -10; yy <= 10; yy++) clear_px(x - 3, y + yy);
    clear_spot(x - 18, y - 5);
    clear_spot(x - 16, y + 5);
    clear_spot(x - 3, y - 6);
    clear_spot(x + 5, y + 5);
    clear_spot(x + 14, y - 3);

    hline(x + 25, x + 34, y - 5);
    hline(x + 26, x + 36, y - 3);
    hline(x + 27, x + 37, y - 1);
    hline(x + 27, x + 37, y + 1);
    hline(x + 26, x + 36, y + 3);
    hline(x + 25, x + 34, y + 5);
    clear_px(x + 33, y - 2);
    clear_px(x + 33, y + 2);
    hline(x + 33, x + 41, y - 8);
    hline(x + 33, x + 41, y + 8);

    hline(x - 35, x - 22, y - 8); put_px(x - 36 - leg, y - 12);
    hline(x - 39, x - 27, y);     put_px(x - 40 + leg, y);
    hline(x - 35, x - 22, y + 8); put_px(x - 36 - leg, y + 12);
    hline(x + 8, x + 21, y - 9);  put_px(x + 22 + leg, y - 13);
    hline(x + 11, x + 24, y);     put_px(x + 25 - leg, y);
    hline(x + 8, x + 21, y + 9);  put_px(x + 22 + leg, y + 13);
}

// ---- Animation dispatcher (with EEPROM persistence) ----
#define ANIM_COUNT 4

typedef union {
    uint32_t raw;
    struct { uint8_t anim_idx; } s;
} user_config_t;
static user_config_t user_config;

static uint8_t anim_idx = 0;
static bool    initial_anim_sync_done = false;

static void user_sync_anim_slave_handler(uint8_t in_buflen, const void *in_data, uint8_t out_buflen, void *out_data) {
    if (in_buflen == sizeof(user_sync_t)) {
        const user_sync_t *sync = (const user_sync_t *)in_data;
        if (sync->anim_idx < ANIM_COUNT) {
            anim_idx = sync->anim_idx;
        }
    }
}

static void sync_anim_to_slave(void) {
    user_sync_t sync = {.anim_idx = anim_idx};
    transaction_rpc_send(USER_SYNC_ANIM, sizeof(sync), &sync);
}

static void run_current_anim(void) {
    switch (anim_idx) {
        case 0: rain_step_and_draw(); break;
        case 1: anim_coffee_step();   break;
        case 2: anim_wave_step();     break;
        case 3: anim_bug_step();      break;
    }
}

// ---- Big-glyph rendering on the master OLED ----

static void draw_glyph(uint8_t c, int16_t x, int16_t y, uint8_t scale) {
    if (c == 0) return;
    for (uint8_t col = 0; col < 5; col++) {
        uint8_t byte = pgm_read_byte(&font[(uint16_t)c * 6 + col]);
        for (uint8_t row = 0; row < 8; row++) {
            if (byte & (1 << row)) {
                for (uint8_t sx = 0; sx < scale; sx++) {
                    for (uint8_t sy = 0; sy < scale; sy++) {
                        put_px(x + col * scale + sx, y + row * scale + sy);
                    }
                }
            }
        }
    }
}

static void draw_string_scaled(const char *s, int16_t x, int16_t y, uint8_t scale) {
    while (*s) {
        draw_glyph((uint8_t)*s, x, y, scale);
        x += 6 * scale;
        s++;
    }
}

static char kc_to_char(uint16_t kc) {
    if (kc >= KC_A && kc <= KC_Z) return 'A' + (kc - KC_A);
    if (kc >= KC_1 && kc <= KC_9) return '1' + (kc - KC_1);
    switch (kc) {
        case KC_0:    return '0';
        case KC_MINS: return '-';
        case KC_EQL:  return '=';
        case KC_LBRC: return '[';
        case KC_RBRC: return ']';
        case KC_BSLS: return '\\';
        case KC_SCLN: return ';';
        case KC_QUOT: return '\'';
        case KC_GRV:  return '`';
        case KC_COMM: return ',';
        case KC_DOT:  return '.';
        case KC_SLSH: return '/';
    }
    return 0;
}

static const char *kc_to_label(uint16_t kc) {
    switch (kc) {
        case KC_ENT:  return "RET";
        case KC_BSPC: return "BSP";
        case KC_TAB:  return "TAB";
        case KC_SPC:  return "SPC";
        case KC_ESC:  return "ESC";
        case KC_DEL:  return "DEL";
        case KC_LSFT: case KC_RSFT: return "SFT";
        case KC_LCTL: case KC_RCTL: return "CTL";
        case KC_LALT: case KC_RALT: return "ALT";
        case KC_LGUI: case KC_RGUI: return "GUI";
        case KC_CAPS: return "CAP";
        case KC_PGUP: return "PUP";
        case KC_PGDN: return "PDN";
        case KC_HOME: return "HOM";
        case KC_END:  return "END";
        case KC_F1:   return "F1";   case KC_F2:  return "F2";
        case KC_F3:   return "F3";   case KC_F4:  return "F4";
        case KC_F5:   return "F5";   case KC_F6:  return "F6";
        case KC_F7:   return "F7";   case KC_F8:  return "F8";
        case KC_F9:   return "F9";   case KC_F10: return "F10";
        case KC_F11:  return "F11";  case KC_F12: return "F12";
        case KC_VOLU: return "VL+";
        case KC_VOLD: return "VL-";
        case KC_MUTE: return "MUT";
        case KC_MNXT: return ">>";
        case KC_MPRV: return "<<";
        case KC_MPLY: return "PLY";
    }
    return NULL;
}

static int8_t kc_to_arrow(uint16_t kc) {
    switch (kc) {
        case KC_UP:    return 0;
        case KC_DOWN:  return 1;
        case KC_LEFT:  return 2;
        case KC_RGHT:  return 3;
    }
    return -1;
}

static void draw_arrow_icon(int16_t cx, int16_t cy, uint8_t dir) {
    if (dir <= 1) {
        int16_t y0 = (dir == 0) ? cy - 4  : cy - 12;
        int16_t y1 = (dir == 0) ? cy + 12 : cy + 4;
        for (int16_t y = y0; y <= y1; y++) hline(cx - 3, cx + 3, y);
        for (int8_t i = 0; i < 11; i++) {
            int16_t y = (dir == 0) ? cy - 12 + i : cy + 12 - i;
            int16_t w = 10 - i;
            if (w < 0) break;
            hline(cx - w, cx + w, y);
        }
    } else {
        int16_t x0 = (dir == 2) ? cx - 4  : cx - 12;
        int16_t x1 = (dir == 2) ? cx + 12 : cx + 4;
        for (int16_t x = x0; x <= x1; x++) vline(x, cy - 3, cy + 3);
        for (int8_t i = 0; i < 11; i++) {
            int16_t x = (dir == 2) ? cx - 12 + i : cx + 12 - i;
            int16_t h = 10 - i;
            if (h < 0) break;
            vline(x, cy - h, cy + h);
        }
    }
}

static void render_big_key(uint16_t kc) {
    // Right zone: x ~ 38..127 (about 90 px wide), full height 0..31
    int8_t arrow = kc_to_arrow(kc);
    if (arrow >= 0) {
        draw_arrow_icon(82, 16, arrow);
        return;
    }
    char ch = kc_to_char(kc);
    if (ch) {
        // single char, scale 4 → 20w x 32h
        draw_glyph((uint8_t)ch, 72, 0, 4);
        return;
    }
    const char *lbl = kc_to_label(kc);
    if (lbl) {
        // scale 3 → each char 15w x 24h, +3 spacing
        uint8_t n = 0;
        for (const char *p = lbl; *p; p++) n++;
        int16_t w = n * 6 * 3 - 3;
        int16_t x = 38 + (90 - w) / 2;
        if (x < 38) x = 38;
        draw_string_scaled(lbl, x, 4, 3);
    }
}

static void draw_master(void) {
    oled_clear();

    uint8_t layer = get_highest_layer(layer_state);
    uint8_t mods  = get_mods();

    // Row 0: layer name (default font)
    oled_set_cursor(0, 0);
    switch (layer) {
        case 0: oled_write_P(PSTR("BASE"), false); break;
        case 1: oled_write_P(PSTR("NUM "), false); break;
        case 2: oled_write_P(PSTR("SYM "), false); break;
        case 3: oled_write_P(PSTR("FN  "), false); break;
        default: oled_write_P(PSTR("???"), false);
    }

    // Row 1: WPM-ish little decoration
    oled_set_cursor(0, 1);
    oled_write_P(PSTR("L"), false);
    oled_write_char('0' + layer, false);

    // Row 2: mods, active inverted
    oled_set_cursor(0, 2);
    oled_write_char('S', mods & MOD_MASK_SHIFT);
    oled_write_char(' ', false);
    oled_write_char('C', mods & MOD_MASK_CTRL);
    oled_write_char(' ', false);
    oled_write_char('A', mods & MOD_MASK_ALT);
    oled_write_char(' ', false);
    oled_write_char('G', mods & MOD_MASK_GUI);

    // Row 3: keystroke counter
    oled_set_cursor(0, 3);
    oled_write_char('#', false);
    oled_write(get_u16_str(key_count, ' '), false);

    // Right side: BIG last key
    render_big_key(last_kc);

    // Vertical separator between left status and right glyph
    for (int16_t y = 0; y < 32; y++) {
        if ((y & 1) == 0) put_px(34, y);
    }
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (keycode == ANIM_NEXT) {
        if (record->event.pressed) {
            anim_idx = (anim_idx + 1) % ANIM_COUNT;
            user_config.s.anim_idx = anim_idx;
            eeconfig_update_user(user_config.raw);
            sync_anim_to_slave();
        }
        return false;
    }
    if (record->event.pressed) {
        uint16_t kc = keycode;
        if (IS_QK_MOD_TAP(kc)) {
            kc = record->tap.count ? QK_MOD_TAP_GET_TAP_KEYCODE(kc) : 0;
        } else if (IS_QK_LAYER_TAP(kc)) {
            kc = record->tap.count ? QK_LAYER_TAP_GET_TAP_KEYCODE(kc) : 0;
        }
        if (IS_QK_MODS(kc)) kc = QK_MODS_GET_BASIC_KEYCODE(kc);
        if (kc != 0 && kc != KC_NO) {
            last_kc = kc;
            key_count++;
        }
    }
    return true;
}

bool oled_task_user(void) {
    if (is_keyboard_master()) {
        draw_master();
        return false;
    }
    if (timer_elapsed(last_frame) > 50) {
        last_frame = timer_read();
        frame_count++;
        oled_clear();
        run_current_anim();
    }
    return false;
}

void keyboard_post_init_user(void) {
    transaction_register_rpc(USER_SYNC_ANIM, user_sync_anim_slave_handler);
    user_config.raw = eeconfig_read_user();
    anim_idx = user_config.s.anim_idx;
    if (anim_idx >= ANIM_COUNT) anim_idx = 0;
}

void housekeeping_task_user(void) {
    if (is_keyboard_master() && !initial_anim_sync_done && timer_read32() > 1000) {
        initial_anim_sync_done = true;
        sync_anim_to_slave();
    }
}

void eeconfig_init_user(void) {
    user_config.raw       = 0;
    user_config.s.anim_idx = 0;
    eeconfig_update_user(user_config.raw);
}
#endif

#ifdef ENCODER_MAP_ENABLE
const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][NUM_DIRECTIONS] = {
  [0] = { ENCODER_CCW_CW(KC_VOLD, KC_VOLU), ENCODER_CCW_CW(KC_MPRV, KC_MNXT), ENCODER_CCW_CW(RM_VALD, RM_VALU), ENCODER_CCW_CW(KC_RGHT, KC_LEFT), },
  [1] = { ENCODER_CCW_CW(KC_VOLD, KC_VOLU), ENCODER_CCW_CW(KC_MPRV, KC_MNXT), ENCODER_CCW_CW(RM_VALD, RM_VALU), ENCODER_CCW_CW(KC_RGHT, KC_LEFT), },
  [2] = { ENCODER_CCW_CW(KC_VOLD, KC_VOLU), ENCODER_CCW_CW(KC_MPRV, KC_MNXT), ENCODER_CCW_CW(RM_VALD, RM_VALU), ENCODER_CCW_CW(KC_RGHT, KC_LEFT), },
  [3] = { ENCODER_CCW_CW(KC_VOLD, KC_VOLU), ENCODER_CCW_CW(KC_MPRV, KC_MNXT), ENCODER_CCW_CW(RM_VALD, RM_VALU), ENCODER_CCW_CW(KC_RGHT, KC_LEFT), },
};
#endif
