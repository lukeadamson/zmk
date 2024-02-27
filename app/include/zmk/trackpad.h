/*
 * Copyright (c) 2023 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <zephyr/kernel.h>

typedef uint8_t zmk_trackpad_finger_contacts_t;

struct zmk_trackpad_finger_data_t {
    bool present;
    bool palm;
    uint16_t x, y;
};

void zmk_trackpad_set_enabled(bool enabled);

bool zmk_trackpad_get_enabled();

void zmk_trackpad_set_mouse_mode(bool mouse_mode);

void zmk_trackpad_selective_set(uint8_t selective);

struct k_work_q *zmk_trackpad_work_q();