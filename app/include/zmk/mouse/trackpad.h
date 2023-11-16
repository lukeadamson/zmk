/*
 * Copyright (c) 2024 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <zephyr/kernel.h>
#include <zmk/endpoints_types.h>

typedef uint8_t zmk_trackpad_finger_contacts_t;

void zmk_trackpad_set_enabled(bool enabled);

bool zmk_trackpad_get_enabled();

void zmk_trackpad_set_selective_report(bool surface_switch, bool button_switch,
                                       struct zmk_endpoint_instance endpoint);

void zmk_trackpad_set_mode_report(uint8_t *report, struct zmk_endpoint_instance endpoint);