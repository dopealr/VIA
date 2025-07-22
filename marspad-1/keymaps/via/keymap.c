#include QMK_KEYBOARD_H

// Optional OLED step function (you must define this elsewhere)
void oled_display_mode_step(void);

// Keymap layers
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT(
        KC_A,                        KC_B,
        LT(0, KC_NO), KC_C, KC_D,   KC_E,
        KC_F,           KC_G, KC_H, KC_I,
        KC_J,           KC_K, KC_L, KC_M
    ),
    [1] = LAYOUT(
        KC_1,                        KC_2,
        LT(0, KC_NO), KC_3, KC_4,   KC_5,
        KC_6,           KC_7, KC_8, KC_9,
        KC_0,           KC_MINS, KC_EQL, KC_BSPC
    ),
    [2] = LAYOUT(
        KC_EXLM,                    KC_AT,
        LT(0, KC_NO), KC_HASH, KC_DLR, KC_PERC,
        KC_CIRC,       KC_AMPR, KC_ASTR, KC_LPRN,
        KC_RPRN,       KC_LEFT, KC_DOWN, KC_UP
    ),
    [3] = LAYOUT(
        KC_MPLY,                    KC_MUTE,
        LT(0, KC_NO), KC_VOLD, KC_VOLU, KC_MNXT,
        KC_MPRV,       KC_HOME, KC_END, KC_DEL,
        KC_PGUP,       KC_PGDN, KC_TAB, KC_ENT
    )
};

// Optional OLED step every 1500ms
void housekeeping_task_user(void) {
#ifdef OLED_ENABLE
    static uint16_t oled_timer = 0;
    if (timer_elapsed(oled_timer) > 1500) {
        oled_display_mode_step();
        oled_timer = timer_read();
    }
#endif
}

// Encoder map (if you have 2 encoders)
#if defined(ENCODER_MAP_ENABLE)
const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][NUM_DIRECTIONS] = {
    [0] = { ENCODER_CCW_CW(KC_PGDN, KC_PGUP),  ENCODER_CCW_CW(KC_VOLD, KC_VOLU)  },
    [1] = { ENCODER_CCW_CW(KC_LEFT, KC_RGHT),  ENCODER_CCW_CW(KC_DOWN, KC_UP)    },
    [2] = { ENCODER_CCW_CW(KC_TAB, KC_ENT),    ENCODER_CCW_CW(KC_HOME, KC_END)   },
    [3] = { ENCODER_CCW_CW(KC_MPRV, KC_MNXT),  ENCODER_CCW_CW(KC_VOLD, KC_VOLU)  },
};
#endif

// Toggle switch logic
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case LT(0, KC_NO):
            if (record->event.pressed) {
                if (record->tap.count > 0 && !record->tap.interrupted) {
                    // Tap: cycle through layers 0 → 1 → 2 → 3 → 0
                    uint8_t current = get_highest_layer(layer_state);
                    if (current >= 3) {
                        layer_clear();
                    } else {
                        layer_move(current + 1);
                    }
                } else {
#ifdef OLED_ENABLE
                    // Hold: show OLED step mode
                    oled_display_mode_step();
#endif
                }
            }
            return false;  // Block original LT(0, KC_NO)
    }
    return true;
}
