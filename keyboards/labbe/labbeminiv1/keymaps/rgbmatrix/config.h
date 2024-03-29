/*
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

// ws2812 options
#define WS2812_DI_PIN C7 // pin the DI on the ws2812 is hooked-up to
#define RGBLIGHT_LED_COUNT 4 // number of LEDs
#define RGB_MATRIX_LED_COUNT RGBLIGHT_LED_COUNT

#define RGB_MATRIX_KEYPRESSES

#define ENABLE_RGB_MATRIX_MULTISPLASH
#define RGB_MATRIX_DEFAULT_MODE RGB_MATRIX_MULTISPLASH