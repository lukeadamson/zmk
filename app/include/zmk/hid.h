/*
 * Copyright (c) 2020 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <zephyr/usb/usb_device.h>
#include <zephyr/usb/class/usb_hid.h>

#include <zmk/keys.h>
#if IS_ENABLED(CONFIG_ZMK_MOUSE)
#include <zmk/mouse.h>
#endif // IS_ENABLED(CONFIG_ZMK_MOUSE)

#include <dt-bindings/zmk/hid_usage.h>
#include <dt-bindings/zmk/hid_usage_pages.h>

#if IS_ENABLED(CONFIG_ZMK_HID_KEYBOARD_NKRO_EXTENDED_REPORT)
#define ZMK_HID_KEYBOARD_NKRO_MAX_USAGE HID_USAGE_KEY_KEYBOARD_LANG8
#else
#define ZMK_HID_KEYBOARD_NKRO_MAX_USAGE HID_USAGE_KEY_KEYPAD_EQUAL
#endif

#define ZMK_HID_MOUSE_NUM_BUTTONS 0x05

// See https://www.usb.org/sites/default/files/hid1_11.pdf section 6.2.2.4 Main Items

#define ZMK_HID_MAIN_VAL_DATA (0x00 << 0)
#define ZMK_HID_MAIN_VAL_CONST (0x01 << 0)

#define ZMK_HID_MAIN_VAL_ARRAY (0x00 << 1)
#define ZMK_HID_MAIN_VAL_VAR (0x01 << 1)

#define ZMK_HID_MAIN_VAL_ABS (0x00 << 2)
#define ZMK_HID_MAIN_VAL_REL (0x01 << 2)

#define ZMK_HID_MAIN_VAL_NO_WRAP (0x00 << 3)
#define ZMK_HID_MAIN_VAL_WRAP (0x01 << 3)

#define ZMK_HID_MAIN_VAL_LIN (0x00 << 4)
#define ZMK_HID_MAIN_VAL_NON_LIN (0x01 << 4)

#define ZMK_HID_MAIN_VAL_PREFERRED (0x00 << 5)
#define ZMK_HID_MAIN_VAL_NO_PREFERRED (0x01 << 5)

#define ZMK_HID_MAIN_VAL_NO_NULL (0x00 << 6)
#define ZMK_HID_MAIN_VAL_NULL (0x01 << 6)

#define ZMK_HID_MAIN_VAL_NON_VOL (0x00 << 7)
#define ZMK_HID_MAIN_VAL_VOL (0x01 << 7)

#define ZMK_HID_MAIN_VAL_BIT_FIELD (0x00 << 8)
#define ZMK_HID_MAIN_VAL_BUFFERED_BYTES (0x01 << 8)

#define ZMK_HID_REPORT_ID_KEYBOARD 0x01
#define ZMK_HID_REPORT_ID_LEDS 0x01
#define ZMK_HID_REPORT_ID_CONSUMER 0x02
#define ZMK_HID_REPORT_ID_MOUSE 0x03

#define ZMK_HID_REPORT_ID_TRACKPAD 0x05
#define ZMK_HID_REPORT_ID_FEATURE_PTP_CAPABILITIES 0x06
#define ZMK_HID_REPORT_ID_FEATURE_PTPHQA 0x07
#define ZMK_HID_REPORT_ID_FEATURE_PTP_MODE 0x08
#define ZMK_HID_REPORT_ID_FEATURE_PTP_SELECTIVE 0x09

static const uint8_t zmk_hid_report_desc[] = {
    HID_USAGE_PAGE(HID_USAGE_GEN_DESKTOP),
    HID_USAGE(HID_USAGE_GD_KEYBOARD),
    HID_COLLECTION(HID_COLLECTION_APPLICATION),
    HID_REPORT_ID(ZMK_HID_REPORT_ID_KEYBOARD),
    HID_USAGE_PAGE(HID_USAGE_KEY),
    HID_USAGE_MIN8(HID_USAGE_KEY_KEYBOARD_LEFTCONTROL),
    HID_USAGE_MAX8(HID_USAGE_KEY_KEYBOARD_RIGHT_GUI),
    HID_LOGICAL_MIN8(0x00),
    HID_LOGICAL_MAX8(0x01),

    HID_REPORT_SIZE(0x01),
    HID_REPORT_COUNT(0x08),
    HID_INPUT(ZMK_HID_MAIN_VAL_DATA | ZMK_HID_MAIN_VAL_VAR | ZMK_HID_MAIN_VAL_ABS),

    HID_USAGE_PAGE(HID_USAGE_KEY),
    HID_REPORT_SIZE(0x08),
    HID_REPORT_COUNT(0x01),
    HID_INPUT(ZMK_HID_MAIN_VAL_CONST | ZMK_HID_MAIN_VAL_VAR | ZMK_HID_MAIN_VAL_ABS),

#if IS_ENABLED(CONFIG_ZMK_HID_INDICATORS)

    HID_USAGE_PAGE(HID_USAGE_LED),
    HID_USAGE_MIN8(HID_USAGE_LED_NUM_LOCK),
    HID_USAGE_MAX8(HID_USAGE_LED_KANA),
    HID_REPORT_SIZE(0x01),
    HID_REPORT_COUNT(0x05),
    HID_OUTPUT(ZMK_HID_MAIN_VAL_DATA | ZMK_HID_MAIN_VAL_VAR | ZMK_HID_MAIN_VAL_ABS),

    HID_USAGE_PAGE(HID_USAGE_LED),
    HID_REPORT_SIZE(0x03),
    HID_REPORT_COUNT(0x01),
    HID_OUTPUT(ZMK_HID_MAIN_VAL_CONST | ZMK_HID_MAIN_VAL_VAR | ZMK_HID_MAIN_VAL_ABS),

#endif // IS_ENABLED(CONFIG_ZMK_HID_INDICATORS)

    HID_USAGE_PAGE(HID_USAGE_KEY),

#if IS_ENABLED(CONFIG_ZMK_HID_REPORT_TYPE_NKRO)
    HID_LOGICAL_MIN8(0x00),
    HID_LOGICAL_MAX8(0x01),
    HID_USAGE_MIN8(0x00),
    HID_USAGE_MAX8(ZMK_HID_KEYBOARD_NKRO_MAX_USAGE),
    HID_REPORT_SIZE(0x01),
    HID_REPORT_COUNT(ZMK_HID_KEYBOARD_NKRO_MAX_USAGE + 1),
    HID_INPUT(ZMK_HID_MAIN_VAL_DATA | ZMK_HID_MAIN_VAL_VAR | ZMK_HID_MAIN_VAL_ABS),
#elif IS_ENABLED(CONFIG_ZMK_HID_REPORT_TYPE_HKRO)
    HID_LOGICAL_MIN8(0x00),
    HID_LOGICAL_MAX16(0xFF, 0x00),
    HID_USAGE_MIN8(0x00),
    HID_USAGE_MAX8(0xFF),
    HID_REPORT_SIZE(0x08),
    HID_REPORT_COUNT(CONFIG_ZMK_HID_KEYBOARD_REPORT_SIZE),
    HID_INPUT(ZMK_HID_MAIN_VAL_DATA | ZMK_HID_MAIN_VAL_ARRAY | ZMK_HID_MAIN_VAL_ABS),
#else
#error "A proper HID report type must be selected"
#endif

    HID_END_COLLECTION,
    HID_USAGE_PAGE(HID_USAGE_CONSUMER),
    HID_USAGE(HID_USAGE_CONSUMER_CONSUMER_CONTROL),
    HID_COLLECTION(HID_COLLECTION_APPLICATION),
    HID_REPORT_ID(ZMK_HID_REPORT_ID_CONSUMER),
    HID_USAGE_PAGE(HID_USAGE_CONSUMER),

#if IS_ENABLED(CONFIG_ZMK_HID_CONSUMER_REPORT_USAGES_BASIC)
    HID_LOGICAL_MIN8(0x00),
    HID_LOGICAL_MAX16(0xFF, 0x00),
    HID_USAGE_MIN8(0x00),
    HID_USAGE_MAX8(0xFF),
    HID_REPORT_SIZE(0x08),
#elif IS_ENABLED(CONFIG_ZMK_HID_CONSUMER_REPORT_USAGES_FULL)
    HID_LOGICAL_MIN8(0x00),
    HID_LOGICAL_MAX16(0xFF, 0x0F),
    HID_USAGE_MIN8(0x00),
    HID_USAGE_MAX16(0xFF, 0x0F),
    HID_REPORT_SIZE(0x10),
#else
#error "A proper consumer HID report usage range must be selected"
#endif
    HID_REPORT_COUNT(CONFIG_ZMK_HID_CONSUMER_REPORT_SIZE),
    HID_INPUT(ZMK_HID_MAIN_VAL_DATA | ZMK_HID_MAIN_VAL_ARRAY | ZMK_HID_MAIN_VAL_ABS),
    HID_END_COLLECTION,

#if IS_ENABLED(CONFIG_ZMK_MOUSE)
    HID_USAGE_PAGE(HID_USAGE_GD),
    HID_USAGE(HID_USAGE_GD_MOUSE),
    HID_COLLECTION(HID_COLLECTION_APPLICATION),
    HID_REPORT_ID(ZMK_HID_REPORT_ID_MOUSE),
    HID_USAGE(HID_USAGE_GD_POINTER),
    HID_COLLECTION(HID_COLLECTION_PHYSICAL),
    HID_USAGE_PAGE(HID_USAGE_BUTTON),
    HID_USAGE_MIN8(0x1),
    HID_USAGE_MAX8(ZMK_HID_MOUSE_NUM_BUTTONS),
    HID_LOGICAL_MIN8(0x00),
    HID_LOGICAL_MAX8(0x01),
    HID_REPORT_SIZE(0x01),
    HID_REPORT_COUNT(0x5),
    HID_INPUT(ZMK_HID_MAIN_VAL_DATA | ZMK_HID_MAIN_VAL_VAR | ZMK_HID_MAIN_VAL_ABS),
    // Constant padding for the last 3 bits.
    HID_REPORT_SIZE(0x03),
    HID_REPORT_COUNT(0x01),
    HID_INPUT(ZMK_HID_MAIN_VAL_CONST | ZMK_HID_MAIN_VAL_VAR | ZMK_HID_MAIN_VAL_ABS),
    // Some OSes ignore pointer devices without X/Y data.
    HID_USAGE_PAGE(HID_USAGE_GEN_DESKTOP),
    HID_USAGE(HID_USAGE_GD_X),
    HID_USAGE(HID_USAGE_GD_Y),
    HID_USAGE(HID_USAGE_GD_WHEEL),
    HID_LOGICAL_MIN8(-0x7F),
    HID_LOGICAL_MAX8(0x7F),
    HID_REPORT_SIZE(0x08),
    HID_REPORT_COUNT(0x03),
    HID_INPUT(ZMK_HID_MAIN_VAL_DATA | ZMK_HID_MAIN_VAL_VAR | ZMK_HID_MAIN_VAL_REL),
    HID_END_COLLECTION,
    HID_END_COLLECTION,
#endif // IS_ENABLED(CONFIG_ZMK_MOUSE)
#if IS_ENABLED(CONFIG_ZMK_TRACKPAD)
    HID_USAGE_PAGE(HID_USAGE_DIGITIZERS),
    HID_USAGE(HID_USAGE_DIGITIZERS_TOUCH_PAD),
    HID_COLLECTION(HID_COLLECTION_APPLICATION),
    // Windows Precision Touchpad Input Reports
    // REPORT_ID (0x05)
    HID_REPORT_ID(ZMK_HID_REPORT_ID_TRACKPAD),
    // Confidence Tip Switch
    HID_USAGE(HID_USAGE_DIGITIZERS_FINGER),
    HID_COLLECTION(HID_COLLECTION_LOGICAL),
    HID_LOGICAL_MIN8(0),
    HID_LOGICAL_MAX8(1),
    // USAGE (Confidence)
    HID_USAGE(0x47),
    // USAGE (Tip switch)
    HID_USAGE(0x42),
    HID_REPORT_COUNT(2),
    HID_REPORT_SIZE(1),
    HID_INPUT(0x02),
    // Byte Padding
    HID_REPORT_COUNT(6),
    HID_INPUT(0x03),

    // Contact ID
    HID_USAGE(HID_USAGE_DIGITIZERS_CONTACT_IDENTIFIER),
    HID_REPORT_COUNT(1),
    HID_REPORT_SIZE(3),
    HID_LOGICAL_MAX8(5),
    HID_INPUT(0x02),
    // Byte padding
    HID_REPORT_SIZE(1),
    HID_REPORT_COUNT(5),
    HID_INPUT(0x03),
    // X and Y coords
    HID_USAGE_PAGE(HID_USAGE_GD),
    HID_LOGICAL_MIN8(0),
    // Defined in KConfig
    HID_LOGICAL_MAX16((CONFIG_ZMK_TRACKPAD_LOGICAL_X & 0xFF),
                      ((CONFIG_ZMK_TRACKPAD_LOGICAL_X >> 8) & 0xFF)),
    HID_REPORT_SIZE(16),
    // Exponent (-2)
    0x55,
    0x0e,
    // Unit (Linear CM)
    0x65,
    0x11,
    // X Usage
    HID_USAGE(HID_USAGE_GD_X),
    // Physical Min (0)
    0x35,
    0x00,
    // Physical Max (defined in KConfig)
    0x46,
    (CONFIG_ZMK_TRACKPAD_PHYSICAL_X & 0xFF),
    ((CONFIG_ZMK_TRACKPAD_PHYSICAL_X >> 8) & 0xFF),
    HID_REPORT_COUNT(1),
    HID_INPUT(0x02),

    // Y Usage
    HID_USAGE(HID_USAGE_GD_Y),
    // Defined in KConfig
    HID_LOGICAL_MAX16((CONFIG_ZMK_TRACKPAD_LOGICAL_Y & 0xFF),
                      ((CONFIG_ZMK_TRACKPAD_LOGICAL_Y >> 8) & 0xFF)),
    // Physical Max (defined in KConfig)
    0x46,
    (CONFIG_ZMK_TRACKPAD_PHYSICAL_Y & 0xFF),
    ((CONFIG_ZMK_TRACKPAD_PHYSICAL_Y >> 8) & 0xFF),
    HID_INPUT(0x02),
    HID_END_COLLECTION,

    HID_USAGE_PAGE(HID_USAGE_DIGITIZERS),
    // Scan Time
    HID_USAGE(HID_USAGE_DIGITIZERS_SCAN_TIME),
    // Exponent (-4)
    0x55,
    0x0C,
    // Unit (Linear Seconds)
    0x66,
    0x01,
    0x10,
    // Physical Maximum (65535)
    0x47,
    0xff,
    0xff,
    0x00,
    0x00,
    // Logical Maximum (65535)
    0x27,
    0xff,
    0xff,
    0x00,
    0x00,

    HID_REPORT_SIZE(16),
    HID_REPORT_COUNT(1),
    HID_INPUT(0x02),
    // Contact Count
    HID_USAGE(HID_USAGE_DIGITIZERS_CONTACT_COUNT),
    // Physical Maximum (0)
    0x45,
    0x00,

    HID_LOGICAL_MAX8(0x05),
    HID_REPORT_COUNT(1),
    HID_REPORT_SIZE(8),
    HID_INPUT(0x02),

    // Button report
    HID_USAGE_PAGE(HID_USAGE_GEN_BUTTON),
    // Buttons usages (1, 2, 3)
    HID_USAGE(0x01),
    HID_USAGE(0x02),
    HID_USAGE(0x03),

    HID_LOGICAL_MAX8(1),
    HID_REPORT_SIZE(1),
    HID_REPORT_COUNT(3),
    HID_INPUT(0x02),
    // Byte Padding
    HID_REPORT_SIZE(1),
    HID_REPORT_COUNT(5),
    HID_INPUT(0x03),

    // Device Capabilities Feature Report

    HID_USAGE_PAGE(HID_USAGE_DIGITIZERS),
    HID_REPORT_ID(ZMK_HID_REPORT_ID_FEATURE_PTP_CAPABILITIES),
    HID_USAGE(HID_USAGE_DIGITIZERS_CONTACT_COUNT_MAXIMUM),
    HID_REPORT_SIZE(8),
    HID_REPORT_COUNT(1),
    HID_LOGICAL_MAX8(0x05),
    HID_FEATURE(0x02),

    HID_USAGE(HID_USAGE_DIGITIZERS_PAD_TYPE),
    HID_LOGICAL_MAX8(0x7F),
    HID_FEATURE(0x02),

    // PTPHQA Blob: Necessary for < Windows 10

    // USAGE_PAGE (Vendor Defined)
    0x06,
    0xff,
    0x00,

    HID_REPORT_ID(ZMK_HID_REPORT_ID_FEATURE_PTPHQA),
    // Vendor Usage 0xC5)
    HID_USAGE(0xC5),

    HID_LOGICAL_MIN8(0),
    HID_LOGICAL_MAX16(0xFF, 0x00),
    HID_REPORT_SIZE(8),
    // Report Count (256)
    0x96,
    0x00,
    0x01,

    HID_FEATURE(0x02),
    HID_END_COLLECTION,

    // Configuration collection
    HID_USAGE_PAGE(HID_USAGE_DIGITIZERS),
    HID_USAGE(HID_USAGE_DIGITIZERS_DEVICE_CONFIGURATION),
    HID_COLLECTION(HID_COLLECTION_APPLICATION),

    // PTP input mode report
    HID_REPORT_ID(ZMK_HID_REPORT_ID_FEATURE_PTP_MODE),
    HID_USAGE(HID_USAGE_DIGITIZERS_FINGER),
    HID_COLLECTION(HID_COLLECTION_LOGICAL),
    HID_USAGE(HID_USAGE_DIGITIZERS_DEVICE_MODE),
    HID_LOGICAL_MIN8(0),
    HID_LOGICAL_MAX8(10),
    HID_REPORT_SIZE(8),
    HID_REPORT_COUNT(1),
    HID_FEATURE(0x02),
    HID_END_COLLECTION,

    // PTP Selective reporting report
    HID_USAGE(HID_USAGE_DIGITIZERS_FINGER),
    HID_COLLECTION(HID_COLLECTION_PHYSICAL),
    HID_REPORT_ID(ZMK_HID_REPORT_ID_FEATURE_PTP_SELECTIVE),

    HID_USAGE(HID_USAGE_DIGITIZERS_SURFACE_SWITCH),
    HID_USAGE(HID_USAGE_DIGITIZERS_BUTTON_SWITCH),
    HID_REPORT_SIZE(1),
    HID_REPORT_COUNT(2),

    HID_LOGICAL_MIN8(0),
    HID_LOGICAL_MAX8(1),
    HID_FEATURE(0x02),
    // Byte Padding
    HID_REPORT_COUNT(6),
    HID_FEATURE(0x03),
    HID_END_COLLECTION,
    HID_END_COLLECTION,
#endif
};

#if IS_ENABLED(CONFIG_ZMK_USB_BOOT)

#define HID_ERROR_ROLLOVER 0x1
#define HID_BOOT_KEY_LEN 6

#if IS_ENABLED(CONFIG_ZMK_HID_REPORT_TYPE_HKRO) &&                                                 \
    CONFIG_ZMK_HID_KEYBOARD_REPORT_SIZE == HID_BOOT_KEY_LEN
typedef struct zmk_hid_keyboard_report_body zmk_hid_boot_report_t;
#else
struct zmk_hid_boot_report {
    zmk_mod_flags_t modifiers;
    uint8_t _reserved;
    uint8_t keys[HID_BOOT_KEY_LEN];
} __packed;

typedef struct zmk_hid_boot_report zmk_hid_boot_report_t;
#endif
#endif

struct zmk_hid_keyboard_report_body {
    zmk_mod_flags_t modifiers;
    uint8_t _reserved;
#if IS_ENABLED(CONFIG_ZMK_HID_REPORT_TYPE_NKRO)
    uint8_t keys[(ZMK_HID_KEYBOARD_NKRO_MAX_USAGE + 1) / 8];
#elif IS_ENABLED(CONFIG_ZMK_HID_REPORT_TYPE_HKRO)
    uint8_t keys[CONFIG_ZMK_HID_KEYBOARD_REPORT_SIZE];
#endif
} __packed;

struct zmk_hid_keyboard_report {
    uint8_t report_id;
    struct zmk_hid_keyboard_report_body body;
} __packed;

#if IS_ENABLED(CONFIG_ZMK_HID_INDICATORS)

struct zmk_hid_led_report_body {
    uint8_t leds;
} __packed;

struct zmk_hid_led_report {
    uint8_t report_id;
    struct zmk_hid_led_report_body body;
} __packed;

#endif // IS_ENABLED(CONFIG_ZMK_HID_INDICATORS)

struct zmk_hid_consumer_report_body {
#if IS_ENABLED(CONFIG_ZMK_HID_CONSUMER_REPORT_USAGES_BASIC)
    uint8_t keys[CONFIG_ZMK_HID_CONSUMER_REPORT_SIZE];
#elif IS_ENABLED(CONFIG_ZMK_HID_CONSUMER_REPORT_USAGES_FULL)
    uint16_t keys[CONFIG_ZMK_HID_CONSUMER_REPORT_SIZE];
#endif
} __packed;

struct zmk_hid_consumer_report {
    uint8_t report_id;
    struct zmk_hid_consumer_report_body body;
} __packed;

#if IS_ENABLED(CONFIG_ZMK_MOUSE)
struct zmk_hid_mouse_report_body {
    zmk_mouse_button_flags_t buttons;
    int8_t d_x;
    int8_t d_y;
    int8_t d_wheel;
} __packed;

struct zmk_hid_mouse_report {
    uint8_t report_id;
    struct zmk_hid_mouse_report_body body;
} __packed;

#endif // IS_ENABLED(CONFIG_ZMK_MOUSE)

#if IS_ENABLED(CONFIG_ZMK_TRACKPAD)
// Reporting structure from osmakari
// Report for single finger
struct zmk_ptp_finger {
    // Confidence (bit 0) and tip switch (bit 1)
    uint8_t confidence_tip;
    // Contact ID
    uint8_t contact_id;
    // X
    uint16_t x;
    // Y
    uint16_t y;
} __packed;

struct zmk_hid_ptp_report_body {
    // Finger reporting
    struct zmk_ptp_finger finger;
    // scantime
    uint16_t scan_time;
    // Contact count
    uint8_t contact_count;
    // Buttons
    uint8_t buttons;

} __packed;

// Report containing finger data
struct zmk_hid_ptp_report {
    // 0x05
    uint8_t report_id;
    struct zmk_hid_ptp_report_body body;

} __packed;

// Feature report for configuration
struct zmk_hid_ptp_feature_selective_report {
    uint8_t report_id;
    // Selective reporting: Surface switch (bit 0), Button switch (bit 1)
    uint8_t selective_reporting;
} __packed;

// Feature report for mode
struct zmk_hid_ptp_feature_mode_report {
    uint8_t report_id;
    // input mode, 0 for mouse, 3 for trackpad
    uint8_t mode;
} __packed;

// Feature report for certification
struct zmk_hid_ptp_feature_certification_report {
    uint8_t report_id;

    uint8_t ptphqa_blob[256];
} __packed;

#define PTP_PAD_TYPE_DEPRESSIBLE 0x00
#define PTP_PAD_TYPE_PRESSURE 0x01
#define PTP_PAD_TYPE_NON_CLICKABLE 0x02

// Feature report for device capabilities
struct zmk_hid_ptp_feature_capabilities_report {
    uint8_t report_id;
    // Max touches (L 4bit) and pad type (H 4bit):
    // Max touches: number 3-5
    // Pad type:    0 for Depressible, 1 for Non-depressible, 2 for Non-clickable
    uint8_t max_touches;
    uint8_t pad_type;

} __packed;

#endif

zmk_mod_flags_t zmk_hid_get_explicit_mods(void);
int zmk_hid_register_mod(zmk_mod_t modifier);
int zmk_hid_unregister_mod(zmk_mod_t modifier);
bool zmk_hid_mod_is_pressed(zmk_mod_t modifier);

int zmk_hid_register_mods(zmk_mod_flags_t explicit_modifiers);
int zmk_hid_unregister_mods(zmk_mod_flags_t explicit_modifiers);
int zmk_hid_implicit_modifiers_press(zmk_mod_flags_t implicit_modifiers);
int zmk_hid_implicit_modifiers_release(void);
int zmk_hid_masked_modifiers_set(zmk_mod_flags_t masked_modifiers);
int zmk_hid_masked_modifiers_clear(void);

int zmk_hid_keyboard_press(zmk_key_t key);
int zmk_hid_keyboard_release(zmk_key_t key);
void zmk_hid_keyboard_clear(void);
bool zmk_hid_keyboard_is_pressed(zmk_key_t key);

int zmk_hid_consumer_press(zmk_key_t key);
int zmk_hid_consumer_release(zmk_key_t key);
void zmk_hid_consumer_clear(void);
bool zmk_hid_consumer_is_pressed(zmk_key_t key);

int zmk_hid_press(uint32_t usage);
int zmk_hid_release(uint32_t usage);
bool zmk_hid_is_pressed(uint32_t usage);

#if IS_ENABLED(CONFIG_ZMK_MOUSE)
int zmk_hid_mouse_button_press(zmk_mouse_button_t button);
int zmk_hid_mouse_button_release(zmk_mouse_button_t button);
int zmk_hid_mouse_buttons_press(zmk_mouse_button_flags_t buttons);
int zmk_hid_mouse_buttons_release(zmk_mouse_button_flags_t buttons);
void zmk_hid_mouse_set(zmk_mouse_button_flags_t buttons, int8_t xDelta, int8_t yDelta,
                       int8_t scrollDelta);
void zmk_hid_mouse_clear(void);
#endif // IS_ENABLED(CONFIG_ZMK_MOUSE)

struct zmk_hid_keyboard_report *zmk_hid_get_keyboard_report(void);
struct zmk_hid_consumer_report *zmk_hid_get_consumer_report(void);

#if IS_ENABLED(CONFIG_ZMK_TRACKPAD)
void zmk_hid_ptp_set(struct zmk_ptp_finger finger, uint8_t contact_count, uint16_t scan_time,
                     uint8_t buttons);
struct zmk_hid_ptp_report *zmk_hid_get_ptp_report();
struct zmk_hid_ptp_feature_selective_report *zmk_hid_ptp_get_feature_selective_report();
void zmk_hid_ptp_set_feature_selective_report(uint8_t selective_report);
struct zmk_hid_ptp_feature_mode_report *zmk_hid_ptp_get_feature_mode_report();
void zmk_hid_ptp_set_feature_mode_report(uint8_t mode_report);
struct zmk_hid_ptp_feature_certification_report *zmk_hid_ptp_get_feature_certification_report();
struct zmk_hid_ptp_feature_capabilities_report *zmk_hid_ptp_get_feature_capabilities_report();
#endif

#if IS_ENABLED(CONFIG_ZMK_USB_BOOT)
zmk_hid_boot_report_t *zmk_hid_get_boot_report();
#endif

#if IS_ENABLED(CONFIG_ZMK_MOUSE)
struct zmk_hid_mouse_report *zmk_hid_get_mouse_report();
#endif // IS_ENABLED(CONFIG_ZMK_MOUSE)
