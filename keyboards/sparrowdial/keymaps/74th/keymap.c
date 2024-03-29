// Copyright 2023 QMK
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H

enum layer_number {
    _MAC_BASE_L = 0,
    _MAC_RAISE_L,
    _LINUX_BASE_L,
    _LINUX_RAISE_L,
    _SPECIAL_L,
    _DEMO_L,
};

enum custom_keycodes {
    SCROLL = SAFE_RANGE,
    ESC_ENm,
    ESC_ENl,
    USE_MAC,
    USE_LINUX,
    USE_DEMO,
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

#define CTL_TAB  LCTL_T(KC_TAB)
#define CMD_EN   LGUI_T(EISUm)
#define OPT_JA   LALT_T(KANAm)
#define CTL_JA   RCTL_T(KANAm)
#define CTL_EN   LCTL_T(EISUl)
#define CTL_ESCm LCTL_T(KC_ESC)
#define OPT_ESCm LALT_T(KC_ESC)
#define GUI_JA   RGUI_T(KANAl)
#define SFT_SP   LSFT_T(KC_SPC)
#define SFT_Z    LSFT_T(KC_Z)
#define CTL_A    LCTL_T(KC_A)
#define CTL_QUOT RCTL_T(KC_QUOT)
#define SFT_TAB  RSFT_T(KC_TAB)

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
                layer_on(_MAC_BASE_L);
                layer_off(_LINUX_BASE_L);
                layer_off(_DEMO_L);
                return false;
            case USE_LINUX:
                layer_on(_LINUX_BASE_L);
                layer_off(_MAC_BASE_L);
                layer_off(_DEMO_L);
                return false;
            case USE_DEMO:
                layer_on(_DEMO_L);
                layer_off(_MAC_BASE_L);
                layer_off(_LINUX_BASE_L);
                return false;
            case KC_ESC:
                if(IS_LAYER_ON(_LINUX_BASE_L)){
                    tap_code16(EISUl);
                }else{
                    tap_code16(EISUm);
                }
                return true;
        }
    }
    return true;
}

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [_MAC_BASE_L] = LAYOUT(
        KC_1,     KC_2,     KC_3,     KC_4,     KC_5,                         KC_7,     KC_8,     KC_9,     KC_0,     KC_MINS,  KC_BSLS,
        KC_Q,     KC_W,     KC_E,     KC_R,     KC_T,     KC_6,               KC_Y,     KC_U,     KC_I,     KC_O,     KC_P,     KC_EQL,
        CTL_A,    KC_S,     KC_D,     KC_F,     KC_G,     KC_LBRC,            KC_H,     KC_J,     KC_K,     KC_L,     KC_SCLN,  CTL_QUOT,
        SFT_Z,    KC_X,     KC_C,     KC_V,     KC_B,     KC_RBRC,            KC_N,     KC_M,     KC_COMM,  KC_DOT,   KC_SLSH,  SFT_TAB,
                            SCROLL,   OPT_ESCm, CMD_EN,   SFT_SP,   KC_BTN1,  RS_ENTm,  CTL_JA,   KC_BSPC,  SPECIAL
    ),
    [_MAC_RAISE_L] = LAYOUT(
        G(KC_F1), G(KC_F2), G(KC_F3), G(KC_F4), G(KC_F5),                     G(KC_F6), G(KC_F7), G(KC_F8), G(KC_F10),_______,  _______,
        KC_F1,    KC_F2,    KC_F3,    KC_F4,    KC_F5,    KC_F6,              KC_F7,    KC_F8,    KC_F9,    KC_F10,   KC_F11,   KC_F12,
        _______,  KC_GRV,   S(KC_GRV),_______,  _______,  _______,            _______,  KC_PGDN,  KC_PGUP,  _______,  _______,  _______,
        _______,  KC_TAB,   PR_WINm,  PR_SCRm,  KC_F12,   _______,            KC_LEFT,  KC_DOWN,  KC_UP,    KC_RIGHT, KC_HOME,  KC_END,
                            _______,  _______,  _______,  _______,  _______,  _______,  _______,  KC_DEL,   _______
    ),
    [_LINUX_BASE_L] = LAYOUT(
        KC_1,     KC_2,     KC_3,     KC_4,     KC_5,                         KC_7,     KC_8,     KC_9,     KC_0,     KC_MINS,  KC_BSLS,
        KC_Q,     KC_W,     KC_E,     KC_R,     KC_T,     KC_6,               KC_Y,     KC_U,     KC_I,     KC_O,     KC_P,     KC_EQL,
        CTL_A,    KC_S,     KC_D,     KC_F,     KC_G,     KC_LBRC,            KC_H,     KC_J,     KC_K,     KC_L,     KC_SCLN,  CTL_QUOT,
        SFT_Z,    KC_X,     KC_C,     KC_V,     KC_B,     KC_RBRC,            KC_N,     KC_M,     KC_COMM,  KC_DOT,   KC_SLSH,  SFT_TAB,
                            SCROLL,  ESC_ENl,  CTL_EN,   SFT_SP,   KC_LSFT,  RS_ENTm,  GUI_JA,   KC_BSPC,  SPECIAL
    ),
    [_LINUX_RAISE_L] = LAYOUT(
        G(KC_F1), G(KC_F2), G(KC_F3), G(KC_F4), G(KC_F5),                     G(KC_F6), G(KC_F7), G(KC_F8), G(KC_F10),_______,  _______,
        KC_F1,    KC_F2,    KC_F3,    KC_F4,    KC_F5,    KC_F6,              KC_F7,    KC_F8,    KC_F9,    KC_F10,   KC_F11,   KC_F12,
        _______,  KC_GRV,   S(KC_GRV),_______,  _______,  _______,            _______,  KC_PGDN,  KC_PGUP,  _______,  _______,  _______,
        _______,  KC_TAB,   PR_WINm,  PR_SCRm,  KC_F12,   _______,            KC_LEFT,  KC_DOWN,  KC_UP,    KC_RIGHT, KC_HOME,  KC_END,
                            _______,  _______,  _______,  _______,  _______,  _______,  _______,  KC_DEL,  _______
    ),
    [_SPECIAL_L] = LAYOUT(
        _______,  USE_MAC,  USE_LINUX,USE_DEMO, _______,                      _______,  _______,  _______,  _______,  _______,  _______,
        _______,  _______,  _______,  _______,  _______,  _______,            _______,  _______,  _______,  _______,  _______,  _______,
        KC_CAPS,  _______,  _______,  _______,  _______,  _______,            _______,  _______,  _______,  _______,  _______,  _______,
        _______,  _______,  _______,  _______,  _______,  _______,            _______,  _______,  _______,  _______,  _______,  _______,
                            _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______
    ),
    [_DEMO_L] = LAYOUT(
        KC_1,     KC_2,     KC_3,     KC_4,     KC_5,                         KC_7,     KC_8,     KC_9,     KC_0,     KC_MINS,  KC_EQL,
        KC_Q,     KC_W,     KC_E,     KC_R,     KC_T,     KC_6,               KC_Y,     KC_U,     KC_I,     KC_O,     KC_P,     KC_BSLS,
        KC_A,     KC_S,     KC_D,     KC_F,     KC_G,     KC_LBRC,            KC_H,     KC_J,     KC_K,     KC_L,     KC_SCLN,  KC_QUOT,
        KC_Z,     KC_X,     KC_C,     KC_V,     KC_B,     KC_RBRC,            KC_N,     KC_M,     KC_COMM,  KC_DOT,   KC_SLSH,  KC_RSFT,
                            KC_ESC,   KC_LALT,  KC_LCTL,  KC_SPC,   KC_BTN1,  KC_ENT,   KC_BSPC,  MO(_MAC_RAISE_L),KC_MS_BTN1
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
        case _DEMO_L:
            rgblight_sethsv(85, 0xFF, 0x20);
            break;
    }

    return state;
}

layer_state_t default_layer_state_set_user(layer_state_t state) {
    rgblight_setrgb(0x00, 0x00, 0x01);
    return state;
}

void keyboard_post_init_user(void) {
#if POINTING_DEVICE_DEBUG
    debug_enable=true;
#endif
    // debug_matrix=true;
    // debug_keyboard=true;
    // debug_mouse=true;
    rgblight_sethsv(168, 0xFF, 0x20);
}
