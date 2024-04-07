/*
 * Copyright (c) 2020 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#include "zmk/keys.h"
#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#include <zmk/hid.h>
#include <dt-bindings/zmk/modifiers.h>
#if IS_ENABLED(CONFIG_ZMK_TRACKPAD)
#include <zmk/trackpad.h>
#endif

static struct zmk_hid_keyboard_report keyboard_report = {

    .report_id = ZMK_HID_REPORT_ID_KEYBOARD, .body = {.modifiers = 0, ._reserved = 0, .keys = {0}}};

static struct zmk_hid_consumer_report consumer_report = {.report_id = ZMK_HID_REPORT_ID_CONSUMER,
                                                         .body = {.keys = {0}}};

#if IS_ENABLED(CONFIG_ZMK_USB_BOOT)

static zmk_hid_boot_report_t boot_report = {.modifiers = 0, ._reserved = 0, .keys = {0}};
static uint8_t keys_held = 0;

#endif /* IS_ENABLED(CONFIG_ZMK_USB_BOOT) */

#if IS_ENABLED(CONFIG_ZMK_MOUSE)

static struct zmk_hid_mouse_report mouse_report = {
    .report_id = ZMK_HID_REPORT_ID_MOUSE, .body = {.buttons = 0, .d_x = 0, .d_y = 0, .d_wheel = 0}};

#endif // IS_ENABLED(CONFIG_ZMK_MOUSE)

#if IS_ENABLED(CONFIG_ZMK_TRACKPAD)

// Report containing finger data
struct zmk_hid_ptp_report ptp_report = {
    .report_id = ZMK_HID_REPORT_ID_TRACKPAD,
    .body = {.finger = {.confidence_tip = 0, .contact_id = 0, .x = 0, .y = 0},
             .contact_count = 0,
             .scan_time = 0,
             .buttons = 0}};

// Feature report for configuration
struct zmk_hid_ptp_feature_selective_report ptp_feature_selective_report = {
    .report_id = ZMK_HID_REPORT_ID_FEATURE_PTP_SELECTIVE, .selective_reporting = 3};

// Feature report for input mode
struct zmk_hid_ptp_feature_mode_report ptp_feature_mode_report = {
    .report_id = ZMK_HID_REPORT_ID_FEATURE_PTP_MODE, .mode = 0};

// Feature report for ptphqa
struct zmk_hid_ptp_feature_certification_report ptp_feature_certification_report = {
    .report_id = ZMK_HID_REPORT_ID_FEATURE_PTPHQA,
    .ptphqa_blob = {
        0xfc, 0x28, 0xfe, 0x84, 0x40, 0xcb, 0x9a, 0x87, 0x0d, 0xbe, 0x57, 0x3c, 0xb6, 0x70, 0x09,
        0x88, 0x07, 0x97, 0x2d, 0x2b, 0xe3, 0x38, 0x34, 0xb6, 0x6c, 0xed, 0xb0, 0xf7, 0xe5, 0x9c,
        0xf6, 0xc2, 0x2e, 0x84, 0x1b, 0xe8, 0xb4, 0x51, 0x78, 0x43, 0x1f, 0x28, 0x4b, 0x7c, 0x2d,
        0x53, 0xaf, 0xfc, 0x47, 0x70, 0x1b, 0x59, 0x6f, 0x74, 0x43, 0xc4, 0xf3, 0x47, 0x18, 0x53,
        0x1a, 0xa2, 0xa1, 0x71, 0xc7, 0x95, 0x0e, 0x31, 0x55, 0x21, 0xd3, 0xb5, 0x1e, 0xe9, 0x0c,
        0xba, 0xec, 0xb8, 0x89, 0x19, 0x3e, 0xb3, 0xaf, 0x75, 0x81, 0x9d, 0x53, 0xb9, 0x41, 0x57,
        0xf4, 0x6d, 0x39, 0x25, 0x29, 0x7c, 0x87, 0xd9, 0xb4, 0x98, 0x45, 0x7d, 0xa7, 0x26, 0x9c,
        0x65, 0x3b, 0x85, 0x68, 0x89, 0xd7, 0x3b, 0xbd, 0xff, 0x14, 0x67, 0xf2, 0x2b, 0xf0, 0x2a,
        0x41, 0x54, 0xf0, 0xfd, 0x2c, 0x66, 0x7c, 0xf8, 0xc0, 0x8f, 0x33, 0x13, 0x03, 0xf1, 0xd3,
        0xc1, 0x0b, 0x89, 0xd9, 0x1b, 0x62, 0xcd, 0x51, 0xb7, 0x80, 0xb8, 0xaf, 0x3a, 0x10, 0xc1,
        0x8a, 0x5b, 0xe8, 0x8a, 0x56, 0xf0, 0x8c, 0xaa, 0xfa, 0x35, 0xe9, 0x42, 0xc4, 0xd8, 0x55,
        0xc3, 0x38, 0xcc, 0x2b, 0x53, 0x5c, 0x69, 0x52, 0xd5, 0xc8, 0x73, 0x02, 0x38, 0x7c, 0x73,
        0xb6, 0x41, 0xe7, 0xff, 0x05, 0xd8, 0x2b, 0x79, 0x9a, 0xe2, 0x34, 0x60, 0x8f, 0xa3, 0x32,
        0x1f, 0x09, 0x78, 0x62, 0xbc, 0x80, 0xe3, 0x0f, 0xbd, 0x65, 0x20, 0x08, 0x13, 0xc1, 0xe2,
        0xee, 0x53, 0x2d, 0x86, 0x7e, 0xa7, 0x5a, 0xc5, 0xd3, 0x7d, 0x98, 0xbe, 0x31, 0x48, 0x1f,
        0xfb, 0xda, 0xaf, 0xa2, 0xa8, 0x6a, 0x89, 0xd6, 0xbf, 0xf2, 0xd3, 0x32, 0x2a, 0x9a, 0xe4,
        0xcf, 0x17, 0xb7, 0xb8, 0xf4, 0xe1, 0x33, 0x08, 0x24, 0x8b, 0xc4, 0x43, 0xa5, 0xe5, 0x24,
        0xc2}};

// Feature report for device capabilities
static struct zmk_hid_ptp_feature_capabilities_report ptp_feature_capabilities_report = {
    .report_id = ZMK_HID_REPORT_ID_FEATURE_PTP_CAPABILITIES,
    .body =
        {
            .max_touches = CONFIG_ZMK_TRACKPAD_MAX_FINGERS,
            .pad_type = PTP_PAD_TYPE_DEPRESSIBLE,
        },
};
#endif

// Keep track of how often a modifier was pressed.
// Only release the modifier if the count is 0.
static int explicit_modifier_counts[8] = {0, 0, 0, 0, 0, 0, 0, 0};
static zmk_mod_flags_t explicit_modifiers = 0;
static zmk_mod_flags_t implicit_modifiers = 0;
static zmk_mod_flags_t masked_modifiers = 0;

#define SET_MODIFIERS(mods)                                                                        \
    {                                                                                              \
        keyboard_report.body.modifiers = (mods & ~masked_modifiers) | implicit_modifiers;          \
        LOG_DBG("Modifiers set to 0x%02X", keyboard_report.body.modifiers);                        \
    }

#define GET_MODIFIERS (keyboard_report.body.modifiers)

zmk_mod_flags_t zmk_hid_get_explicit_mods(void) { return explicit_modifiers; }

int zmk_hid_register_mod(zmk_mod_t modifier) {
    explicit_modifier_counts[modifier]++;
    LOG_DBG("Modifier %d count %d", modifier, explicit_modifier_counts[modifier]);
    WRITE_BIT(explicit_modifiers, modifier, true);
    zmk_mod_flags_t current = GET_MODIFIERS;
    SET_MODIFIERS(explicit_modifiers);
    return current == GET_MODIFIERS ? 0 : 1;
}

int zmk_hid_unregister_mod(zmk_mod_t modifier) {
    if (explicit_modifier_counts[modifier] <= 0) {
        LOG_ERR("Tried to unregister modifier %d too often", modifier);
        return -EINVAL;
    }
    explicit_modifier_counts[modifier]--;
    LOG_DBG("Modifier %d count: %d", modifier, explicit_modifier_counts[modifier]);
    if (explicit_modifier_counts[modifier] == 0) {
        LOG_DBG("Modifier %d released", modifier);
        WRITE_BIT(explicit_modifiers, modifier, false);
    }
    zmk_mod_flags_t current = GET_MODIFIERS;
    SET_MODIFIERS(explicit_modifiers);
    return current == GET_MODIFIERS ? 0 : 1;
}

bool zmk_hid_mod_is_pressed(zmk_mod_t modifier) {
    zmk_mod_flags_t mod_flag = 1 << modifier;
    return (zmk_hid_get_explicit_mods() & mod_flag) == mod_flag;
}

int zmk_hid_register_mods(zmk_mod_flags_t modifiers) {
    int ret = 0;
    for (zmk_mod_t i = 0; i < 8; i++) {
        if (modifiers & (1 << i)) {
            ret += zmk_hid_register_mod(i);
        }
    }
    return ret;
}

int zmk_hid_unregister_mods(zmk_mod_flags_t modifiers) {
    int ret = 0;
    for (zmk_mod_t i = 0; i < 8; i++) {
        if (modifiers & (1 << i)) {
            ret += zmk_hid_unregister_mod(i);
        }
    }

    return ret;
}

#if IS_ENABLED(CONFIG_ZMK_USB_BOOT)

static zmk_hid_boot_report_t *boot_report_rollover(uint8_t modifiers) {
    boot_report.modifiers = modifiers;
    for (int i = 0; i < HID_BOOT_KEY_LEN; i++) {
        boot_report.keys[i] = HID_ERROR_ROLLOVER;
    }
    return &boot_report;
}

#endif /* IS_ENABLED(CONFIG_ZMK_USB_BOOT) */

#if IS_ENABLED(CONFIG_ZMK_HID_REPORT_TYPE_NKRO)

#define TOGGLE_KEYBOARD(code, val) WRITE_BIT(keyboard_report.body.keys[code / 8], code % 8, val)

#if IS_ENABLED(CONFIG_ZMK_USB_BOOT)
zmk_hid_boot_report_t *zmk_hid_get_boot_report(void) {
    if (keys_held > HID_BOOT_KEY_LEN) {
        return boot_report_rollover(keyboard_report.body.modifiers);
    }

    boot_report.modifiers = keyboard_report.body.modifiers;
    memset(&boot_report.keys, 0, HID_BOOT_KEY_LEN);
    int ix = 0;
    uint8_t base_code = 0;
    for (int i = 0; i < (ZMK_HID_KEYBOARD_NKRO_MAX_USAGE + 1) / 8; ++i) {
        if (ix == keys_held) {
            break;
        }
        if (!keyboard_report.body.keys[i]) {
            continue;
        }
        base_code = i * 8;
        for (int j = 0; j < 8; ++j) {
            if (keyboard_report.body.keys[i] & BIT(j)) {
                boot_report.keys[ix++] = base_code + j;
            }
        }
    }
    return &boot_report;
}
#endif

static inline int select_keyboard_usage(zmk_key_t usage) {
    if (usage > ZMK_HID_KEYBOARD_NKRO_MAX_USAGE) {
        return -EINVAL;
    }
    TOGGLE_KEYBOARD(usage, 1);
#if IS_ENABLED(CONFIG_ZMK_USB_BOOT)
    ++keys_held;
#endif
    return 0;
}

static inline int deselect_keyboard_usage(zmk_key_t usage) {
    if (usage > ZMK_HID_KEYBOARD_NKRO_MAX_USAGE) {
        return -EINVAL;
    }
    TOGGLE_KEYBOARD(usage, 0);
#if IS_ENABLED(CONFIG_ZMK_USB_BOOT)
    --keys_held;
#endif
    return 0;
}

static inline bool check_keyboard_usage(zmk_key_t usage) {
    if (usage > ZMK_HID_KEYBOARD_NKRO_MAX_USAGE) {
        return false;
    }
    return keyboard_report.body.keys[usage / 8] & (1 << (usage % 8));
}

#elif IS_ENABLED(CONFIG_ZMK_HID_REPORT_TYPE_HKRO)

#define TOGGLE_KEYBOARD(match, val)                                                                \
    for (int idx = 0; idx < CONFIG_ZMK_HID_KEYBOARD_REPORT_SIZE; idx++) {                          \
        if (keyboard_report.body.keys[idx] != match) {                                             \
            continue;                                                                              \
        }                                                                                          \
        keyboard_report.body.keys[idx] = val;                                                      \
        if (val) {                                                                                 \
            break;                                                                                 \
        }                                                                                          \
    }

#if IS_ENABLED(CONFIG_ZMK_USB_BOOT)
zmk_hid_boot_report_t *zmk_hid_get_boot_report(void) {
    if (keys_held > HID_BOOT_KEY_LEN) {
        return boot_report_rollover(keyboard_report.body.modifiers);
    }

#if CONFIG_ZMK_HID_KEYBOARD_REPORT_SIZE != HID_BOOT_KEY_LEN
    // Form a boot report from a report of different size.

    boot_report.modifiers = keyboard_report.body.modifiers;

    int out = 0;
    for (int i = 0; i < CONFIG_ZMK_HID_KEYBOARD_REPORT_SIZE; i++) {
        uint8_t key = keyboard_report.body.keys[i];
        if (key) {
            boot_report.keys[out++] = key;
            if (out == keys_held) {
                break;
            }
        }
    }

    while (out < HID_BOOT_KEY_LEN) {
        boot_report.keys[out++] = 0;
    }

    return &boot_report;
#else
    return &keyboard_report.body;
#endif /* CONFIG_ZMK_HID_KEYBOARD_REPORT_SIZE != HID_BOOT_KEY_LEN */
}
#endif /* IS_ENABLED(CONFIG_ZMK_USB_BOOT) */

static inline int select_keyboard_usage(zmk_key_t usage) {
    TOGGLE_KEYBOARD(0U, usage);
#if IS_ENABLED(CONFIG_ZMK_USB_BOOT)
    ++keys_held;
#endif
    return 0;
}

static inline int deselect_keyboard_usage(zmk_key_t usage) {
    TOGGLE_KEYBOARD(usage, 0U);
#if IS_ENABLED(CONFIG_ZMK_USB_BOOT)
    --keys_held;
#endif
    return 0;
}

static inline int check_keyboard_usage(zmk_key_t usage) {
    for (int idx = 0; idx < CONFIG_ZMK_HID_KEYBOARD_REPORT_SIZE; idx++) {
        if (keyboard_report.body.keys[idx] == usage) {
            return true;
        }
    }
    return false;
}

#else
#error "A proper HID report type must be selected"
#endif

#define TOGGLE_CONSUMER(match, val)                                                                \
    COND_CODE_1(IS_ENABLED(CONFIG_ZMK_HID_CONSUMER_REPORT_USAGES_BASIC),                           \
                (if (val > 0xFF) { return -ENOTSUP; }), ())                                        \
    for (int idx = 0; idx < CONFIG_ZMK_HID_CONSUMER_REPORT_SIZE; idx++) {                          \
        if (consumer_report.body.keys[idx] != match) {                                             \
            continue;                                                                              \
        }                                                                                          \
        consumer_report.body.keys[idx] = val;                                                      \
        if (val) {                                                                                 \
            break;                                                                                 \
        }                                                                                          \
    }

int zmk_hid_implicit_modifiers_press(zmk_mod_flags_t new_implicit_modifiers) {
    implicit_modifiers = new_implicit_modifiers;
    zmk_mod_flags_t current = GET_MODIFIERS;
    SET_MODIFIERS(explicit_modifiers);
    return current == GET_MODIFIERS ? 0 : 1;
}

int zmk_hid_implicit_modifiers_release(void) {
    implicit_modifiers = 0;
    zmk_mod_flags_t current = GET_MODIFIERS;
    SET_MODIFIERS(explicit_modifiers);
    return current == GET_MODIFIERS ? 0 : 1;
}

int zmk_hid_masked_modifiers_set(zmk_mod_flags_t new_masked_modifiers) {
    masked_modifiers = new_masked_modifiers;
    zmk_mod_flags_t current = GET_MODIFIERS;
    SET_MODIFIERS(explicit_modifiers);
    return current == GET_MODIFIERS ? 0 : 1;
}

int zmk_hid_masked_modifiers_clear(void) {
    masked_modifiers = 0;
    zmk_mod_flags_t current = GET_MODIFIERS;
    SET_MODIFIERS(explicit_modifiers);
    return current == GET_MODIFIERS ? 0 : 1;
}

int zmk_hid_keyboard_press(zmk_key_t code) {
    if (code >= HID_USAGE_KEY_KEYBOARD_LEFTCONTROL && code <= HID_USAGE_KEY_KEYBOARD_RIGHT_GUI) {
        return zmk_hid_register_mod(code - HID_USAGE_KEY_KEYBOARD_LEFTCONTROL);
    }
    select_keyboard_usage(code);
    return 0;
};

int zmk_hid_keyboard_release(zmk_key_t code) {
    if (code >= HID_USAGE_KEY_KEYBOARD_LEFTCONTROL && code <= HID_USAGE_KEY_KEYBOARD_RIGHT_GUI) {
        return zmk_hid_unregister_mod(code - HID_USAGE_KEY_KEYBOARD_LEFTCONTROL);
    }
    deselect_keyboard_usage(code);
    return 0;
};

bool zmk_hid_keyboard_is_pressed(zmk_key_t code) {
    if (code >= HID_USAGE_KEY_KEYBOARD_LEFTCONTROL && code <= HID_USAGE_KEY_KEYBOARD_RIGHT_GUI) {
        return zmk_hid_mod_is_pressed(code - HID_USAGE_KEY_KEYBOARD_LEFTCONTROL);
    }
    return check_keyboard_usage(code);
}

void zmk_hid_keyboard_clear(void) {
    memset(&keyboard_report.body, 0, sizeof(keyboard_report.body));
}

int zmk_hid_consumer_press(zmk_key_t code) {
    TOGGLE_CONSUMER(0U, code);
    return 0;
};

int zmk_hid_consumer_release(zmk_key_t code) {
    TOGGLE_CONSUMER(code, 0U);
    return 0;
};

void zmk_hid_consumer_clear(void) {
    memset(&consumer_report.body, 0, sizeof(consumer_report.body));
}

bool zmk_hid_consumer_is_pressed(zmk_key_t key) {
    for (int idx = 0; idx < CONFIG_ZMK_HID_CONSUMER_REPORT_SIZE; idx++) {
        if (consumer_report.body.keys[idx] == key) {
            return true;
        }
    }
    return false;
}

int zmk_hid_press(uint32_t usage) {
    switch (ZMK_HID_USAGE_PAGE(usage)) {
    case HID_USAGE_KEY:
        return zmk_hid_keyboard_press(ZMK_HID_USAGE_ID(usage));
    case HID_USAGE_CONSUMER:
        return zmk_hid_consumer_press(ZMK_HID_USAGE_ID(usage));
    }
    return -EINVAL;
}

int zmk_hid_release(uint32_t usage) {
    switch (ZMK_HID_USAGE_PAGE(usage)) {
    case HID_USAGE_KEY:
        return zmk_hid_keyboard_release(ZMK_HID_USAGE_ID(usage));
    case HID_USAGE_CONSUMER:
        return zmk_hid_consumer_release(ZMK_HID_USAGE_ID(usage));
    }
    return -EINVAL;
}

bool zmk_hid_is_pressed(uint32_t usage) {
    switch (ZMK_HID_USAGE_PAGE(usage)) {
    case HID_USAGE_KEY:
        return zmk_hid_keyboard_is_pressed(ZMK_HID_USAGE_ID(usage));
    case HID_USAGE_CONSUMER:
        return zmk_hid_consumer_is_pressed(ZMK_HID_USAGE_ID(usage));
    }
    return false;
}

#if IS_ENABLED(CONFIG_ZMK_MOUSE)

// Keep track of how often a button was pressed.
// Only release the button if the count is 0.
static int explicit_button_counts[5] = {0, 0, 0, 0, 0};
static zmk_mod_flags_t explicit_buttons = 0;

#define SET_MOUSE_BUTTONS(btns)                                                                    \
    {                                                                                              \
        mouse_report.body.buttons = btns;                                                          \
        LOG_DBG("Mouse buttons set to 0x%02X", mouse_report.body.buttons);                         \
    }

int zmk_hid_mouse_button_press(zmk_mouse_button_t button) {
    if (button >= ZMK_HID_MOUSE_NUM_BUTTONS) {
        return -EINVAL;
    }

    explicit_button_counts[button]++;
    LOG_DBG("Button %d count %d", button, explicit_button_counts[button]);
    WRITE_BIT(explicit_buttons, button, true);
    SET_MOUSE_BUTTONS(explicit_buttons);
    return 0;
}

int zmk_hid_mouse_button_release(zmk_mouse_button_t button) {
    if (button >= ZMK_HID_MOUSE_NUM_BUTTONS) {
        return -EINVAL;
    }

    if (explicit_button_counts[button] <= 0) {
        LOG_ERR("Tried to release button %d too often", button);
        return -EINVAL;
    }
    explicit_button_counts[button]--;
    LOG_DBG("Button %d count: %d", button, explicit_button_counts[button]);
    if (explicit_button_counts[button] == 0) {
        LOG_DBG("Button %d released", button);
        WRITE_BIT(explicit_buttons, button, false);
    }
    SET_MOUSE_BUTTONS(explicit_buttons);
    return 0;
}

int zmk_hid_mouse_buttons_press(zmk_mouse_button_flags_t buttons) {
    for (zmk_mouse_button_t i = 0; i < ZMK_HID_MOUSE_NUM_BUTTONS; i++) {
        if (buttons & BIT(i)) {
            zmk_hid_mouse_button_press(i);
        }
    }
    return 0;
}

int zmk_hid_mouse_buttons_release(zmk_mouse_button_flags_t buttons) {
    for (zmk_mouse_button_t i = 0; i < ZMK_HID_MOUSE_NUM_BUTTONS; i++) {
        if (buttons & BIT(i)) {
            zmk_hid_mouse_button_release(i);
        }
    }
    return 0;
}

void zmk_hid_mouse_set(uint8_t buttons, int8_t xDelta, int8_t yDelta, int8_t scrollDelta) {
    mouse_report.body.buttons = buttons;
    mouse_report.body.d_x = xDelta;
    mouse_report.body.d_y = yDelta;
    mouse_report.body.d_wheel = scrollDelta;
}

void zmk_hid_mouse_clear(void) { memset(&mouse_report.body, 0, sizeof(mouse_report.body)); }

#endif // IS_ENABLED(CONFIG_ZMK_MOUSE)

#if IS_ENABLED(CONFIG_ZMK_TRACKPAD)
void zmk_hid_ptp_set(struct zmk_ptp_finger finger, uint8_t contact_count, uint16_t scan_time,
                     uint8_t buttons) {
    ptp_report.body.finger = finger;
    ptp_report.body.contact_count = contact_count;
    ptp_report.body.scan_time = scan_time;
    ptp_report.body.buttons = buttons;
}
#endif

struct zmk_hid_keyboard_report *zmk_hid_get_keyboard_report(void) { return &keyboard_report; }

struct zmk_hid_consumer_report *zmk_hid_get_consumer_report(void) {
    return &consumer_report;
}

#if IS_ENABLED(CONFIG_ZMK_MOUSE)

struct zmk_hid_mouse_report *zmk_hid_get_mouse_report(void) {
    return &mouse_report;
}

#endif // IS_ENABLED(CONFIG_ZMK_MOUSE)

#if IS_ENABLED(CONFIG_ZMK_TRACKPAD)
struct zmk_hid_ptp_report *zmk_hid_get_ptp_report(void) { return &ptp_report; }

struct zmk_hid_ptp_feature_selective_report *zmk_hid_ptp_get_feature_selective_report() {
    return &ptp_feature_selective_report;
}

void zmk_hid_trackpad_selective_mode(struct k_work *work) {
    zmk_trackpad_selective_set(ptp_feature_selective_report.selective_reporting);
}

K_WORK_DEFINE(selective_mode_work, zmk_hid_trackpad_selective_mode);

void zmk_hid_ptp_set_feature_selective_report(uint8_t selective_report) {
    ptp_feature_selective_report.selective_reporting = selective_report;
    LOG_DBG("Setting selective reporting to: %d", selective_report);
    k_work_submit(&selective_mode_work);
}

struct zmk_hid_ptp_feature_mode_report *zmk_hid_ptp_get_feature_mode_report() {
    return &ptp_feature_mode_report;
}

void zmk_hid_ptp_set_feature_mode_report(uint8_t mode) {
    LOG_DBG("Setting mode report to: %d", mode);
    ptp_feature_mode_report.mode = mode;
}

struct zmk_hid_ptp_feature_certification_report *zmk_hid_ptp_get_feature_certification_report() {
    return &ptp_feature_certification_report;
}

struct zmk_hid_ptp_feature_capabilities_report *zmk_hid_ptp_get_feature_capabilities_report() {
    return &ptp_feature_capabilities_report;
}
#endif
