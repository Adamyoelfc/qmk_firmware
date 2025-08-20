/*
Copyright 2019 @foostan
Copyright 2020 Drashna Jaelre <@drashna>
Copyright 2021 Elliot Powell @e11i0t23

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
*/
#include "quantum.h"

#ifdef OLED_ENABLE
void oled_render_logo(void) {
    static const char PROGMEM mb_logo[] = {
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,128,224,248,252,252,254,254,252,252,252,240,  0,  0,  0,  0,  0,254,254,254,254,254,254,254,254,254,254,254,254,252,252,220, 24, 56, 96,192,128,  0,  0,  0,  0,  0,  0,  0,  0,  0,128,240,254,254,254,254,  6,  6,  6, 14, 56,224,128,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,192,240,248, 12,  4,  4,  4,132,164, 36,136, 88,176,192,  0,  0,  0,  0,192, 64, 96, 48, 16, 16, 24,  8,  8,  8, 12,  4,228,228,140, 24,112,192,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,192,248,255,255,255,255, 31,  7,127,255,255,255,255,192,  0,  0,  0,  0,255,255,255,255,255,255,  1,  1,  1,  1,  1,  3,255,255,255,254,120,  0,  0,255, 24,  0,  0,  0,  0,  0,  0,224,252,255,255,255,255,255,255,  7, 30,124,224,224,  0,  7, 60,224,  0,  0,  0,  0,  0,  0,  0,  0,224,255, 63,  0,  0,  0,  0,  0,  0, 15, 52, 25,191,201, 58,255,240,112, 30,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1, 15,127,254,240,129,  7, 60,224,  0,  0,  0,  0,  0,  0,  0,  0, 
        0,  0,  0,  0,  0,  0,  0,  0,240,254,255,255,127, 31, 15, 12, 12, 12, 12,255,255,255,255,255,  0,  0,  0,  0,255,255,255,255,255,255,254,254,254,254,254,254,223,207,199,227,224, 48, 30,  3,  0,  0,  0,  0,  0,240,254,255,255,255,255,255,255,255,  3,  2,  2,  2, 15,255,255,224,128,129,254,  0,  0,  0,  0,  0,  0,224,255, 63,  0,  0,  0,  0,  0,  0,224, 96,  0,241,128,  0,  2,  0,  1,  0,  0,  0,  0,128,248,  0,224,240,  0,  0,  0,  0,  0,  0,  0,  0, 15,255,255,248,  0,  3, 60,192,  0,  0,  0,  0,  0,  0, 
        0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  0,  0,  0,  0,  0,  0,  0,  0,  1,  3,  3,  3,  1,  0,  0,  0,  0,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  1,  0,  0,  0,  0,  0,  0,  0,  0,  3,  7,  7,  7,  7,  7,  7,  7,  3,  3,  0,  0,  0,  0,  0,  1,  3,  3,  3,  3,  3,  0,  0,  0,  0,  0, 30, 31, 19, 16, 16, 16, 16, 20, 22, 19,  9,  8, 14,  3,  1,  3,  6,  4,  4,  4,  4,  4,  4,  5,  4,  7,  1,  1, 15, 24, 48, 32, 32, 32, 32, 32, 32, 32, 49, 19, 16, 16, 24, 15,  0,  0,  0,  0,  0,  0,
};
    oled_write_raw_P(mb_logo, sizeof(mb_logo));
    //oled_set_cursor(oled_max_chars()/2,oled_max_lines()/2);
    //oled_write_P(PSTR("R2G"), false);
}
#endif
