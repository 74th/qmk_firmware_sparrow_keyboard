// Copyright 2023 QMK
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H

enum layer_number {
    _BASE = 0,
    _RAISE,
    _LAYER_2,
    _LAYER_3,
};

enum custom_keycodes {
    SCROLL = SAFE_RANGE,
};
bool set_scrolling = false;

#define SCROLL_DIVISOR_H 32.0
#define SCROLL_DIVISOR_V 32.0

float scroll_accumulated_h = 0;
float scroll_accumulated_v = 0;

report_mouse_t pointing_device_task_user(report_mouse_t mouse_report) {
    if (set_scrolling) {
        scroll_accumulated_h += (float)mouse_report.x / SCROLL_DIVISOR_H;
        scroll_accumulated_v += (float)mouse_report.y / SCROLL_DIVISOR_V;

        mouse_report.h = (int8_t)scroll_accumulated_h;
        mouse_report.v = (int8_t)scroll_accumulated_v;

        scroll_accumulated_h -= (int8_t)scroll_accumulated_h;
        scroll_accumulated_v -= (int8_t)scroll_accumulated_v;

        mouse_report.x = 0;
        mouse_report.y = 0;
    }
    return mouse_report;
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case SCROLL:
            set_scrolling = record->event.pressed;
            break;
        default:
            break;
    }
    return true;
}

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    /*
     * ┌───┬───┬───┬───┬───┐         ┌───┬───┬───┬───┬───┬───┐
     * │ 1 │ 2 │ 3 │ 4 │ 5 │         │ 7 │ 8 │ 9 │ 0 │ - │ = │
     * ├───┼───┼───┼───┼───┼───┐     ├───┼───┼───┼───┼───┼───┤
     * │ Q │ W │ E │ R │ T │ 6 │     │ Y │ U │ I │ O │ P │ \ │
     * ├───┼───┼───┼───┼───┼───┤ O   ├───┼───┼───┼───┼───┼───┤
     * │ A │ S │ D │ F │ G │ [ │     │ H │ J │ K │ L │ ; │ ' │
     * ├───┼───┼───┼───┼───┼───┤     ├───┼───┼───┼───┼───┼───┤
     * │ Z │ X │ C │ V │ B │ ] │     │ N │ M │ , │ . │ / │Sft│
     * └───┼───┼───┴┬──┴─┬─┴──┬┴───┐ ├───┴┬──┴─┬─┴──┬┴──┬┴───┘
     *     │ESC│LALT│LCTL│SPC │LGUI│ │Ent │Basc│RAIS│MBL│
     *     └───┴────┴────┴────┴────┘ └────┴────┴────┴───┘
     */
    [_BASE] = LAYOUT(
        KC_1,    KC_2,    KC_3,    KC_4,    KC_5,                      KC_7,    KC_8,    KC_9,    KC_0,    KC_MINS, KC_EQL,
        KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_6,             KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_BSLS,
        KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_LBRC,          KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT,
        KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_RBRC,          KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, KC_RSFT,
                          KC_ESC,  KC_LALT, KC_LCTL, KC_SPC,  KC_LGUI, KC_ENT,  KC_BSPC, MO(1),   KC_MS_BTN1
    ),
    /*
     * ┌───┬───┬───┬───┬───┐         ┌───┬───┬───┬───┬───┬───┐
     * │F1 │F2 │F3 │F4 │F5 │         │F7 │F8 │F9 │F10│F11│F12│
     * ├───┼───┼───┼───┼───┼───┐     ├───┼───┼───┼───┼───┼───┤
     * │   │   │   │   │   │   │     │   │   │   │   │   │   │
     * ├───┼───┼───┼───┼───┼───┤ O   ├───┼───┼───┼───┼───┼───┤
     * │   │   │   │   │   │   │     │Tab│PGD│PGU│   │   │   │
     * ├───┼───┼───┼───┼───┼───┤     ├───┼───┼───┼───┼───┼───┤
     * │   │   │   │   │   │   │     │ ← │ ↓ │ ↑ │ → │HOM│END│
     * └───┼───┼───┴┬──┴─┬─┴──┬┴───┐ ├───┴┬──┴─┬─┴──┬┴──┬┴───┘
     *     │   │    │    │    │    │ │    │    │    │   │
     *     └───┴────┴────┴────┴────┘ └────┴────┴────┴───┘
     */
    [_RAISE] = LAYOUT(
        KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,                     KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12,
        _______, _______, _______, _______, _______, KC_F6,            _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______,          KC_TAB,  KC_PGDN, KC_PGUP, _______, _______, _______,
        _______, _______, _______, _______, _______, _______,          KC_LEFT, KC_DOWN, KC_UP,   KC_RIGHT,KC_HOME, KC_END,
                          _______, _______, _______, _______, _______, _______, _______, _______, _______
    ),
    [_LAYER_2] = LAYOUT(
        _______, _______, _______, _______, _______,                   _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______,          _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______,          _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______,          _______, _______, _______, _______, _______, _______,
                          _______, _______, _______, _______, _______, _______, _______, _______, _______
    ),
    [_LAYER_3] = LAYOUT(
        _______, _______, _______, _______, _______,                   _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______,          _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______,          _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______,          _______, _______, _______, _______, _______, _______,
                          _______, _______, _______, _______, _______, _______, _______, _______, _______
    )
};

layer_state_t layer_state_set_user(layer_state_t state) {

    switch (get_highest_layer(state)) {
        case _BASE:
            rgblight_sethsv(HSV_BLUE);
            break;
        case _RAISE:
            rgblight_sethsv(HSV_CYAN);
            break;
        case _LAYER_2:
            rgblight_sethsv(HSV_ORANGE);
            break;
        case _LAYER_3:
            rgblight_sethsv(HSV_GREEN);
            break;
    }

    return state;
}

layer_state_t default_layer_state_set_user(layer_state_t state) {
    rgblight_setrgb(0x00, 0x00, 0x01);
    return state;
}

void keyboard_post_init_user(void) {
  // Customise these values to desired behaviour
#if POINTING_DEVICE_DEBUG
  debug_enable=true;
#endif
  debug_matrix=true;
  //debug_keyboard=true;
  //debug_mouse=true;
}
