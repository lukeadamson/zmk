/*
 * Copyright (c) 2020 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#include <zephyr/device.h>
#include <zephyr/init.h>

#include <zephyr/usb/usb_device.h>
#include <zephyr/usb/class/usb_hid.h>

#include <zmk/usb.h>
#include <zmk/mouse/hid.h>
#include <zmk/keymap.h>
#include <zmk/event_manager.h>

#if IS_ENABLED(CONFIG_ZMK_TRACKPAD)
#include <zmk/mouse/trackpad.h>
#endif

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

static const struct device *hid_dev;

static K_SEM_DEFINE(hid_sem, 1, 1);

static void in_ready_cb(const struct device *dev) { k_sem_give(&hid_sem); }

#define HID_GET_REPORT_TYPE_MASK 0xff00
#define HID_GET_REPORT_ID_MASK 0x00ff

#define HID_REPORT_TYPE_INPUT 0x100
#define HID_REPORT_TYPE_OUTPUT 0x200
#define HID_REPORT_TYPE_FEATURE 0x300

#if IS_ENABLED(CONFIG_ZMK_USB_BOOT)
static uint8_t hid_protocol = HID_PROTOCOL_REPORT;

static void set_proto_cb(const struct device *dev, uint8_t protocol) { hid_protocol = protocol; }

#endif /* IS_ENABLED(CONFIG_ZMK_USB_BOOT) */

static int get_report_cb(const struct device *dev, struct usb_setup_packet *setup, int32_t *len,
                         uint8_t **data) {

    /*
     * 7.2.1 of the HID v1.11 spec is unclear about handling requests for reports that do not exist
     * For requested reports that aren't input reports, return -ENOTSUP like the Zephyr subsys does
     */
    if ((setup->wValue & HID_GET_REPORT_TYPE_MASK) != HID_REPORT_TYPE_INPUT &&
        (setup->wValue & HID_GET_REPORT_TYPE_MASK) != HID_REPORT_TYPE_FEATURE) {
        LOG_ERR("Get: Unsupported report type %d requested",
                (setup->wValue & HID_GET_REPORT_TYPE_MASK) >> 8);
        return -ENOTSUP;
    }

    switch (setup->wValue & HID_GET_REPORT_ID_MASK) {
#if IS_ENABLED(CONFIG_ZMK_TRACKPAD)
    case ZMK_MOUSE_HID_REPORT_ID_DIGITIZER:
        struct zmk_hid_ptp_report *ptp_report = zmk_mouse_hid_get_ptp_report();
        LOG_WRN("Get PTP report");
        *data = (uint8_t *)ptp_report;
        *len = sizeof(*ptp_report);
        break;
    case ZMK_MOUSE_HID_REPORT_ID_FEATURE_PTP_SELECTIVE:
        struct zmk_hid_ptp_feature_selective_report *sel_report =
            zmk_mouse_hid_ptp_get_feature_selective_report();
        *data = (uint8_t *)sel_report;
        LOG_DBG("Selective report get %d", 0);
        *len = sizeof(*sel_report);
        break;
    case ZMK_MOUSE_HID_REPORT_ID_FEATURE_PTP_CAPABILITIES:
        LOG_WRN("Get CAPABILITIES");
        struct zmk_hid_ptp_feature_capabilities_report *cap_report =
            zmk_mouse_hid_ptp_get_feature_capabilities_report();
        *data = (uint8_t *)cap_report;
        *len = sizeof(*cap_report);
        break;
    case ZMK_MOUSE_HID_REPORT_ID_FEATURE_PTPHQA:
        LOG_WRN("Get HQA");
        struct zmk_hid_ptp_feature_certification_report *cert_report =
            zmk_mouse_hid_ptp_get_feature_certification_report();
        *data = (uint8_t *)cert_report;
        *len = sizeof(*cert_report);
        break;
    case ZMK_MOUSE_HID_REPORT_ID_FEATURE_PTP_MODE:
        struct zmk_hid_ptp_feature_mode_report *mode_report =
            zmk_mouse_hid_ptp_get_feature_mode_report();
        *data = (uint8_t *)mode_report;
        LOG_DBG("mode report get %d", 0);
        *len = sizeof(*mode_report);
        break;
#endif // IS_ENABLED(CONFIG_ZMK_TRACKPAD)
    default:
        LOG_ERR("Invalid report ID %d requested", setup->wValue & HID_GET_REPORT_ID_MASK);
        return -EINVAL;
    }

    return 0;
}

static int set_report_cb(const struct device *dev, struct usb_setup_packet *setup, int32_t *len,
                         uint8_t **data) {
    if ((setup->wValue & HID_GET_REPORT_TYPE_MASK) != HID_REPORT_TYPE_OUTPUT &&
        (setup->wValue & HID_GET_REPORT_TYPE_MASK) != HID_REPORT_TYPE_FEATURE) {
        LOG_ERR("Set: Unsupported report type %d requested",
                (setup->wValue & HID_GET_REPORT_TYPE_MASK) >> 8);
        return -ENOTSUP;
    }

    switch (setup->wValue & HID_GET_REPORT_ID_MASK) {
#if IS_ENABLED(CONFIG_ZMK_TRACKPAD)
    case ZMK_MOUSE_HID_REPORT_ID_FEATURE_PTP_MODE:
        if (*len != sizeof(struct zmk_hid_ptp_feature_mode_report)) {
            LOG_ERR("Mode set report is malformed: length=%d", *len);
            return -EINVAL;
        } else {
            struct zmk_hid_ptp_feature_mode_report *report =
                (struct zmk_hid_ptp_feature_mode_report *)*data;
            struct zmk_endpoint_instance endpoint = {
                .transport = ZMK_TRANSPORT_USB,
            };
            zmk_trackpad_set_mode_report(&report->mode, endpoint);
        }
        break;
    case ZMK_MOUSE_HID_REPORT_ID_FEATURE_PTP_SELECTIVE:
        if (*len != sizeof(struct zmk_hid_ptp_feature_selective_report)) {
            LOG_ERR("Mode set report is malformed: length=%d", *len);
            return -EINVAL;
        } else {
            struct zmk_hid_ptp_feature_selective_report *report =
                (struct zmk_hid_ptp_feature_selective_report *)*data;
            LOG_DBG("PTP selective: surface: %d, button: %d", report->body.surface_switch,
                    report->body.button_switch);
            struct zmk_endpoint_instance endpoint2 = {
                .transport = ZMK_TRANSPORT_USB,
            };
            zmk_trackpad_set_selective_report(report->body.surface_switch,
                                              report->body.button_switch, endpoint2);
        }
        break;
#endif // IS_ENABLED(CONFIG_ZMK_TRACKPAD)
    default:
        LOG_ERR("Invalid report ID %d requested", setup->wValue & HID_GET_REPORT_ID_MASK);
        return -EINVAL;
    }

    return 0;
}

static const struct hid_ops ops = {
#if IS_ENABLED(CONFIG_ZMK_USB_BOOT)
    .protocol_change = set_proto_cb,
#endif
    .int_in_ready = in_ready_cb,
    .get_report = get_report_cb,
    .set_report = set_report_cb,
};

static int zmk_mouse_usb_hid_send_report(const uint8_t *report, size_t len) {
    switch (zmk_usb_get_status()) {
    case USB_DC_SUSPEND:
        return usb_wakeup_request();
    case USB_DC_ERROR:
    case USB_DC_RESET:
    case USB_DC_DISCONNECTED:
    case USB_DC_UNKNOWN:
        return -ENODEV;
    default:
        k_sem_take(&hid_sem, K_MSEC(30));
        LOG_HEXDUMP_DBG(report, len, "Mouse HID report");
        int err = hid_int_ep_write(hid_dev, report, len, NULL);

        if (err) {
            LOG_ERR("Failed to write %d", err);
            k_sem_give(&hid_sem);
        }

        return err;
    }
}

#if IS_ENABLED(CONFIG_ZMK_TRACKPAD)
int zmk_mouse_usb_hid_send_ptp_report() {
#if IS_ENABLED(CONFIG_ZMK_USB_BOOT)
    if (hid_protocol == HID_PROTOCOL_BOOT) {
        return -ENOTSUP;
    }
#endif /* IS_ENABLED(CONFIG_ZMK_USB_BOOT) */

    struct zmk_hid_ptp_report *report = zmk_mouse_hid_get_ptp_report();
    return zmk_mouse_usb_hid_send_report((uint8_t *)report, sizeof(*report));
}
#endif // IS_ENABLED(CONFIG_ZMK_TRACKPAD)

static int zmk_mouse_usb_hid_init(void) {
    hid_dev = device_get_binding("HID_1");
    if (hid_dev == NULL) {
        LOG_ERR("Unable to locate HID device");
        return -EINVAL;
    }

    usb_hid_register_device(hid_dev, zmk_mouse_hid_report_desc, sizeof(zmk_mouse_hid_report_desc),
                            &ops);

    // usb_hid_set_proto_code(hid_dev, HID_BOOT_IFACE_CODE_MOUSE);

    usb_hid_init(hid_dev);

    return 0;
}

SYS_INIT(zmk_mouse_usb_hid_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
