/*
 * Copyright (c) 2024 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#define DT_DRV_COMPAT zmk_behavior_trackpad

#include <zephyr/device.h>
#include <drivers/behavior.h>
#include <zephyr/logging/log.h>

#include <zmk/behavior.h>

#include <dt-bindings/zmk/trackpad.h>
#include <zmk/trackpad.h>
#include <zmk/hid.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#if DT_HAS_COMPAT_STATUS_OKAY(DT_DRV_COMPAT)

static int behavior_trackpad_init(const struct device *dev) { return 0; }

static int on_trackpad_keymap_binding_pressed(struct zmk_behavior_binding *binding,
                                              struct zmk_behavior_binding_event event) {
    LOG_DBG("tp behaviuor hit");
    switch (binding->param1) {
    case TP_TOG: {
        zmk_trackpad_set_enabled(!zmk_trackpad_get_enabled());
        return 0;
    }
    case TP_ON: {
        zmk_trackpad_set_enabled(true);
        return 0;
    }
    case TP_OFF: {
        zmk_trackpad_set_enabled(false);
        return 0;
    }
    case TP_MULTI_TOG: {
        zmk_hid_ptp_set_feature_mode_report(zmk_hid_ptp_get_feature_mode_report()->mode ? 0 : 3);
        return 0;
    }
    case TP_MULTI_ON: {
        zmk_hid_ptp_set_feature_mode_report(3);
        return 0;
    }
    case TP_MULTI_OFF: {
        zmk_hid_ptp_set_feature_mode_report(0);
        return 0;
    }
    }
    return -ENOTSUP;
}

static int on_trackpad_keymap_binding_released(struct zmk_behavior_binding *binding,
                                               struct zmk_behavior_binding_event event) {
    return ZMK_BEHAVIOR_OPAQUE;
}

static const struct behavior_driver_api behavior_trackpad_driver_api = {
    .binding_pressed = on_trackpad_keymap_binding_pressed,
    .binding_released = on_trackpad_keymap_binding_released,
};

BEHAVIOR_DT_INST_DEFINE(0, behavior_trackpad_init, NULL, NULL, NULL, APPLICATION,
                        CONFIG_KERNEL_INIT_PRIORITY_DEFAULT, &behavior_trackpad_driver_api);

#endif /* DT_HAS_COMPAT_STATUS_OKAY(DT_DRV_COMPAT) */