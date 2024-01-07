// Copyright 2023 QMK
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H

enum layer_number {
    _MAC_BASE_L = 0,
    _MAC_RAISE_L,
    _LINUX_BASE_L,
    _LINUX_RAISE_L,
    _SPECIAL_L,
};

enum custom_keycodes {
    SCROLL = SAFE_RANGE,
    ESC_ENm,
    ESC_ENl,
    USE_MAC,
    USE_LINUX,
};

#define RS_ENTm LT(_MAC_RAISE_L, KC_ENT)
#define RS_ENTl LT(_LINUX_RAISE_L, KC_ENT)
#define SPECIAL MO(_SPECIAL_L)

#define TO_MAC DF(_MAC_BASE_L)
#define TO_LINUX DF(_LINUX_BASE_L)

#define EISUm KC_LNG2
#define KANAm KC_LNG1
#define PR_WINm LSG(KC_3)
#define PR_SCRm LSG(KC_4)

#define EISUl KC_INT5
#define KANAl KC_INT4
#define PR_WINl A(KC_PSCR)
#define PR_SCRl S(KC_PSCR)

#define CTL_TAB LCTL_T(KC_TAB)
#define CMD_EN LGUI_T(EISUm)
#define OPT_JA LALT_T(KANAm)
#define CTL_EN LCTL_T(EISUl)
#define GUI_JA LGUI_T(KANAl)

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
    }

    if (record->event.pressed) {
        switch (keycode) {
            case ESC_ENm:
                tap_code(EISUm);
                tap_code(KC_ESC);
                return false;
            case ESC_ENl:
                tap_code(EISUl);
                tap_code(KC_ESC);
                return false;
            case USE_MAC:
                layer_off(_LINUX_BASE_L);
                layer_on(_MAC_BASE_L);
                return false;
            case USE_LINUX:
                layer_on(_LINUX_BASE_L);
                layer_off(_MAC_BASE_L);
                return false;
        }
    }
    return true;
}

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [_MAC_BASE_L] = LAYOUT(
        SPECIAL,   KC_1,     KC_2,     KC_3,     KC_4,     KC_5,                         KC_7,     KC_8,     KC_9,     KC_0,     KC_MINS,  KC_EQL,
        KC_GRV,    KC_Q,     KC_W,     KC_E,     KC_R,     KC_T,     KC_6,               KC_Y,     KC_U,     KC_I,     KC_O,     KC_P,     KC_BSLS,
        CTL_TAB,   KC_A,     KC_S,     KC_D,     KC_F,     KC_G,     KC_LBRC,            KC_H,     KC_J,     KC_K,     KC_L,     KC_SCLN,  KC_QUOT,
        KC_LSFT,   KC_Z,     KC_X,     KC_C,     KC_V,     KC_B,     KC_RBRC,            KC_N,     KC_M,     KC_COMM,  KC_DOT,   KC_SLSH,  KC_RSFT,
                                       SCROLL,   ESC_ENm,  CMD_EN,   KC_SPC,   KC_BTN1,  RS_ENTm,  OPT_JA,   KC_BSPC,  KC_BTN2
    ),
    [_MAC_RAISE_L] = LAYOUT(
        _______,   G(KC_F1), G(KC_F2), G(KC_F3), G(KC_F4), G(KC_F5),                     G(KC_F6), G(KC_F7), G(KC_F8), G(KC_F10),_______,  _______,
        KC_ESC,    KC_F1,    KC_F2,    KC_F3,    KC_F4,    KC_F5,    KC_F6,              KC_F7,    KC_F8,    KC_F9,    KC_F10,   KC_F11,   KC_F12,
        KC_LCTL,   S(KC_1),  S(KC_2),  S(KC_3),  S(KC_4),  S(KC_5),  S(KC_6),            S(KC_7),  KC_PGDN,  KC_PGUP,  S(KC_0),  KC_UNDS,  KC_EQL,
        _______,   _______,  _______,  PR_WINm,  PR_SCRm,  KC_F12,   _______,            KC_LEFT,  KC_DOWN,  KC_UP,    KC_RIGHT, KC_HOME,  KC_END,
                                       _______,  _______,  _______,  _______,  _______,  _______,  _______,  KC_DEL,   _______
    ),
    [_LINUX_BASE_L] = LAYOUT(
        SPECIAL,   KC_1,     KC_2,     KC_3,     KC_4,     KC_5,                         KC_7,     KC_8,     KC_9,     KC_0,     KC_MINS,  KC_EQL,
        KC_GRV,    KC_Q,     KC_W,     KC_E,     KC_R,     KC_T,     KC_6,               KC_Y,     KC_U,     KC_I,     KC_O,     KC_P,     KC_BSLS,
        CTL_TAB,   KC_A,     KC_S,     KC_D,     KC_F,     KC_G,     KC_LBRC,            KC_H,     KC_J,     KC_K,     KC_L,     KC_SCLN,  KC_QUOT,
        KC_LSFT,   KC_Z,     KC_X,     KC_C,     KC_V,     KC_B,     KC_RBRC,            KC_N,     KC_M,     KC_COMM,  KC_DOT,   KC_SLSH,  KC_RSFT,
                                       SCROLL,   ESC_ENl,  CTL_EN,   KC_SPC,   KC_BTN1,  RS_ENTl,  GUI_JA,   KC_BSPC,  KC_BTN2
    ),
    [_LINUX_RAISE_L] = LAYOUT(
        _______,   G(KC_F1), G(KC_F2), G(KC_F3), G(KC_F4), G(KC_F5),                     G(KC_F6), G(KC_F7), G(KC_F8), G(KC_F10),_______,  _______,
        KC_ESC,    KC_F1,    KC_F2,    KC_F3,    KC_F4,    KC_F5,    KC_F6,              KC_F7,    KC_F8,    KC_F9,    KC_F10,   KC_F11,   KC_F12,
        KC_LCTL,   S(KC_1),  S(KC_2),  S(KC_3),  S(KC_4),  S(KC_5),  S(KC_6),            S(KC_7),  KC_PGDN,  KC_PGUP,  S(KC_0),  KC_UNDS,  KC_EQL,
        _______,   _______,  _______,  PR_WINm,  PR_SCRm,  KC_F12,   _______,            KC_LEFT,  KC_DOWN,  KC_UP,    KC_RIGHT, KC_HOME,  KC_END,
                                       _______,  _______,  _______,  _______,  _______,  _______,  _______,  KC_DEL,   _______
    ),
    [_SPECIAL_L] = LAYOUT(
        _______,   USE_MAC,  USE_LINUX,_______,  _______,  _______,                      _______,  _______,  _______,  _______,  _______,  _______,
        _______,   _______,  _______,  _______,  _______,  _______,  _______,            _______,  _______,  _______,  _______,  _______,  _______,
        KC_CAPS,   _______,  _______,  _______,  _______,  _______,  _______,            _______,  _______,  _______,  _______,  _______,  _______,
        _______,   _______,  _______,  _______,  _______,  _______,  _______,            _______,  _______,  _______,  _______,  _______,  _______,
                                       _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______
    )
};

layer_state_t layer_state_set_user(layer_state_t state) {

    switch (get_highest_layer(state)) {
        case _MAC_BASE_L:
            rgblight_sethsv(168, 0xFF, 0x20);
            break;
        case _MAC_RAISE_L:
            rgblight_sethsv(168, 0xFF, 0x40);
            break;
        case _LINUX_BASE_L:
            rgblight_sethsv(21, 0xFF, 0x20);
            break;
        case _LINUX_RAISE_L:
            rgblight_sethsv(21, 0xFF, 0x40);
            break;
        case _SPECIAL_L:
            rgblight_sethsv(85, 0xFF, 0x20);
            break;
    }

    return state;
}

layer_state_t default_layer_state_set_user(layer_state_t state) {
    rgblight_sethsv(168, 0xFF, 0x10);
    return state;
}
