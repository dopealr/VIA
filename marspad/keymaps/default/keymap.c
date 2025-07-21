// Copyright 2023 QMK
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H

// For using OLED UI change function from oled.c
void oled_display_mode_step(void);

enum custom_keycodes {
    LAYER_NEXT = SAFE_RANGE,
};

// State for layer key timing and OLED cycling
static uint16_t layer_key_timer = 0;
static bool layer_key_longpress = false;
static uint16_t oled_last_step_time = 0;
static bool layer_key_held = false;

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    // Layer 0: Alphabets
    [0] = LAYOUT(
        KC_A,                        KC_B,
        LAYER_NEXT,  KC_C,  KC_D,    KC_E,
        KC_F,         KC_G,  KC_H,   KC_I,
        KC_J,         KC_K,  KC_L,   KC_M
    ),
    // Layer 1: Numbers
    [1] = LAYOUT(
        KC_1,                        KC_2,
        LAYER_NEXT,  KC_3,   KC_4,   KC_5,
        KC_6,         KC_7,  KC_8,   KC_9,
        KC_0,         KC_MINUS, KC_EQUAL, KC_BSPC
    ),
    // Layer 2: Symbols
    [2] = LAYOUT(
        KC_EXLM,                     KC_AT,
        LAYER_NEXT,  KC_HASH, KC_DLR, KC_PERC,
        KC_CIRC,      KC_AMPR, KC_ASTR, KC_LPRN,
        KC_RPRN,      KC_LEFT, KC_DOWN, KC_UP
    ),
    // Layer 3: Media & Navigation
    [3] = LAYOUT(
        KC_MPLY,                     KC_MUTE,
        LAYER_NEXT,  KC_VOLD, KC_VOLU, KC_MNXT,
        KC_MPRV,      KC_HOME, KC_END, KC_DEL,
        KC_PGUP,      KC_PGDN, KC_TAB, KC_ENT
    )
};

// This runs every scan cycle (about 1ms) and handles repeated OLED UI cycling on long hold.
void matrix_scan_user(void) {
    if (layer_key_held && layer_key_longpress) {
        if (timer_elapsed(oled_last_step_time) > 1500) {
            oled_last_step_time = timer_read();
            oled_display_mode_step();
        }
    }
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case LAYER_NEXT:
            if (record->event.pressed) {
                layer_key_timer = timer_read();
                layer_key_longpress = false;
                layer_key_held = true;
                oled_last_step_time = timer_read();
            } else {
                layer_key_held = false;
                if (!layer_key_longpress) {
                    // Short press: change layer on release
                    uint8_t next_layer = (get_highest_layer(layer_state) + 1) % 4;
                    layer_move(next_layer);
                }
                // else: long press, do NOT change layer, OLED UI handled in matrix_scan_user
            }
            return false;
    }
    return true;
}

// Housekeeping task runs frequently, checks if the longpress threshold has been crossed
void housekeeping_task_user(void) {
    if (layer_key_held && !layer_key_longpress) {
        if (timer_elapsed(layer_key_timer) > 1500) {
            layer_key_longpress = true;
            oled_last_step_time = timer_read();
            oled_display_mode_step();
        }
    }
}

#if defined(ENCODER_MAP_ENABLE)
const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][NUM_DIRECTIONS] = {
    [0] = { ENCODER_CCW_CW(KC_PGDN, KC_PGUP),  ENCODER_CCW_CW(KC_VOLD, KC_VOLU)  },
    [1] = { ENCODER_CCW_CW(KC_LEFT, KC_RGHT),  ENCODER_CCW_CW(KC_DOWN, KC_UP)    },
    [2] = { ENCODER_CCW_CW(KC_TAB, KC_ENT),    ENCODER_CCW_CW(KC_HOME, KC_END)   },
    [3] = { ENCODER_CCW_CW(KC_MPRV, KC_MNXT),  ENCODER_CCW_CW(KC_VOLD, KC_VOLU)  },
};
#endif