// Copyright 2023 QMK
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H

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
     *  ┌───┬───┬───┬───┬───┬───┐       ┌───┬───┬───┬───┬───┬───┐
     *  │Esc│ 1 │ 2 │ 3 │ 4 │ 5 │       │ 7 │ 8 │ 9 │ 0 │ - │ = │
     * ┌┴───┼───┼───┼───┼───┼───┼───┐   ├───┼───┼───┼───┼───┼───┤
     * │ Tab│ Q │ W │ E │ R │ T │ 6 │   │ Y │ U │ I │ O │ P │ \ │
     * ├────┼───┼───┼───┼───┼───┼───┤ O ├───┼───┼───┼───┼───┼───┤
     * │Ctrl│ A │ S │ D │ F │ G │ [ │   │ H │ J │ K │ L │ ; │ ' │
     * ├────┼───┼───┼───┼───┼───┼───┤   ├───┼───┼───┼───┼───┼───┤
     * │Shft│ Z │ X │ C │ V │ B │ ] │   │ N │ M │ , │ . │ / │Sft│
     * └────┴───┼───┼───┴┬──┴─┬─┴──┬┴───┼───┴┬──┴─┬─┴──┬┴──┬┴───┘
     *          │SCR│LGUI│LALT│SPC │CLIK│Ent │RCTL│Basc│RCl│
     *          └───┴────┴────┴────┴────┴────┴────┴────┴───┘
     */
    [0] = LAYOUT(
        KC_ESC,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,             KC_7,    KC_8,    KC_9,    KC_0,    KC_MINS, KC_EQL,
        KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_6,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_BSLS,
        KC_LCTL,  KC_A,   KC_S,    KC_D,    KC_F,    KC_G,    KC_LBRC, KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT,
        KC_LSFT, KC_NUBS, KC_Z,    KC_X,    KC_C,    KC_V,    KC_RBRC, KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, KC_RSFT,
                          SCROLL,  KC_LGUI, KC_LALT, KC_SPC,  KC_BTN1, KC_ENT,  KC_RCTL, KC_BSPC, KC_BTN2
    )
};
