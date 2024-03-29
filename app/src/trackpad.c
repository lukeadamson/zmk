#include <zephyr/drivers/sensor.h>
#include <zephyr/logging/log.h>
#include <zephyr/kernel.h>

#include "zmk/mouse.h"
#include "zmk/trackpad.h"
#include <zmk/hid.h>
#include <zmk/endpoints.h>
#include <zmk/event_manager.h>
#include <zmk/events/sensor_event.h>
#include <zmk/events/endpoint_changed.h>
#include "drivers/sensor/gen4.h"

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

const struct device *trackpad = DEVICE_DT_GET(DT_INST(0, cirque_gen4));

static zmk_trackpad_finger_contacts_t present_contacts = 0;
static zmk_trackpad_finger_contacts_t contacts_to_send = 0;

static uint8_t btns;
static uint16_t scantime;

static bool mousemode;
static bool surface_mode;
static bool button_mode;

static bool enabled;

static int8_t xDelta, yDelta, scrollDelta;

static struct zmk_ptp_finger fingers[5];
static const struct zmk_ptp_finger empty_finger = {0};

#if IS_ENABLED(CONFIG_ZMK_TRACKPAD_WORK_QUEUE_DEDICATED)
K_THREAD_STACK_DEFINE(trackpad_work_stack_area, CONFIG_ZMK_TRACKPAD_DEDICATED_THREAD_STACK_SIZE);
static struct k_work_q trackpad_work_q;
#endif

struct k_work_q *zmk_trackpad_work_q() {
#if IS_ENABLED(CONFIG_ZMK_TRACKPAD_WORK_QUEUE_DEDICATED)
    return &trackpad_work_q;
#else
    return &k_sys_work_q;
#endif
}

void zmk_trackpad_set_enabled(bool enabled) {}

bool zmk_trackpad_get_enabled() { return enabled; }

static void zmk_trackpad_tick(struct k_work *work) {
    if (mousemode) {
        zmk_hid_mouse_set(btns, xDelta, yDelta, scrollDelta);
        zmk_endpoints_send_mouse_report();
    } else if (contacts_to_send) {
        // LOG_DBG("Trackpad sendy thing trigd %d", 0);
        for (int i = 0; i < CONFIG_ZMK_TRACKPAD_MAX_FINGERS; i++)
            if (contacts_to_send & BIT(i)) {
                LOG_DBG("Trackpad sendy thing trigd %d", i);
                zmk_hid_ptp_set(fingers[i], present_contacts, scantime, button_mode ? btns : 0);
                zmk_endpoints_send_ptp_report();
                contacts_to_send &= !BIT(i);
                return;
            }
    } else if (contacts_to_send && !surface_mode) {
        // report buttons only
        LOG_DBG("Trackpad button thing trigd");
        zmk_hid_ptp_set(empty_finger, present_contacts, scantime, button_mode ? btns : 0);
        zmk_endpoints_send_ptp_report();
        contacts_to_send = 0;
    }
}

K_WORK_DEFINE(trackpad_work, zmk_trackpad_tick);

static void handle_trackpad_ptp(const struct device *dev, const struct sensor_trigger *trig) {
    int ret = sensor_sample_fetch(dev);
    if (ret < 0) {
        LOG_ERR("fetch: %d", ret);
        return;
    }
    LOG_DBG("Trackpad handler trigd %d", 0);

    struct sensor_value contacts, confidence_tip, id, x, y, buttons, scan_time;
    sensor_channel_get(dev, SENSOR_CHAN_CONTACTS, &contacts);
    sensor_channel_get(dev, SENSOR_CHAN_BUTTONS, &buttons);
    sensor_channel_get(dev, SENSOR_CHAN_SCAN_TIME, &scan_time);
    // expects bitmap format
    present_contacts = contacts.val1;
    // Buttons and scan time
    btns = button_mode ? buttons.val1 : 0;
    scantime = scan_time.val1;
    // released Fingers
    sensor_channel_get(dev, SENSOR_CHAN_X, &x);
    sensor_channel_get(dev, SENSOR_CHAN_Y, &y);
    sensor_channel_get(dev, SENSOR_CHAN_CONFIDENCE_TIP, &confidence_tip);
    sensor_channel_get(dev, SENSOR_CHAN_FINGER, &id);
    // If finger has changed
    fingers[id.val1].confidence_tip = confidence_tip.val1;
    fingers[id.val1].contact_id = id.val1;
    fingers[id.val1].x = x.val1;
    fingers[id.val1].y = y.val1;
    contacts_to_send |= BIT(id.val1);

    k_work_submit_to_queue(zmk_trackpad_work_q(), &trackpad_work);

    raise_zmk_sensor_event(
        (struct zmk_sensor_event){.sensor_index = 0,
                                  .channel_data_size = 1,
                                  .channel_data = {(struct zmk_sensor_channel_data){
                                      .value = buttons, .channel = SENSOR_CHAN_BUTTONS}},
                                  .timestamp = k_uptime_get()});
}

static void handle_mouse_mode(const struct device *dev, const struct sensor_trigger *trig) {
    LOG_DBG("Trackpad handler trigd in mouse mode %d", 0);
    int ret = sensor_sample_fetch(dev);
    if (ret < 0) {
        LOG_ERR("fetch: %d", ret);
        return;
    }

    struct sensor_value x, y, buttons, wheel;
    sensor_channel_get(dev, SENSOR_CHAN_XDELTA, &x);
    sensor_channel_get(dev, SENSOR_CHAN_YDELTA, &y);
    sensor_channel_get(dev, SENSOR_CHAN_BUTTONS, &buttons);
    sensor_channel_get(dev, SENSOR_CHAN_WHEEL, &wheel);

    btns = buttons.val1;
    xDelta = x.val1;
    yDelta = y.val1;
#if IS_ENABLED(CONFIG_ZMK_TRACKPAD_REVERSE_SCROLL)
    scrollDelta = -wheel.val1;
#else
    scrollDelta = wheel.val1;
#endif
    k_work_submit_to_queue(zmk_trackpad_work_q(), &trackpad_work);

    raise_zmk_sensor_event(
        (struct zmk_sensor_event){.sensor_index = 0,
                                  .channel_data_size = 1,
                                  .channel_data = {(struct zmk_sensor_channel_data){
                                      .value = buttons, .channel = SENSOR_CHAN_BUTTONS}},
                                  .timestamp = k_uptime_get()});
}

static void zmk_trackpad_tick_handler(struct k_timer *timer) {
    k_work_submit_to_queue(zmk_trackpad_work_q(), &trackpad_work);
}

K_TIMER_DEFINE(trackpad_tick, zmk_trackpad_tick_handler, NULL);

void zmk_trackpad_selective_set(uint8_t selective) {
    surface_mode = selective & BIT(0);
    button_mode = selective & BIT(1);
    LOG_DBG("Surface: %d, Button %d", surface_mode, button_mode);
}

void zmk_trackpad_set_mouse_mode(bool mouse_mode) {
    struct sensor_trigger trigger = {
        .type = SENSOR_TRIG_DATA_READY,
        .chan = SENSOR_CHAN_ALL,
    };
    struct sensor_value attr;
    attr.val1 = mouse_mode;
    mousemode = mouse_mode;
    sensor_attr_set(trackpad, SENSOR_CHAN_ALL, SENSOR_ATTR_CONFIGURATION, &attr);
    if (mouse_mode) {
        // k_timer_stop(&trackpad_tick);

        if (sensor_trigger_set(trackpad, &trigger, handle_mouse_mode) < 0) {
            LOG_ERR("can't set trigger mouse mode");
        };
    } else {
        zmk_hid_mouse_clear();
        zmk_endpoints_send_mouse_report();
        // k_timer_start(&trackpad_tick, K_NO_WAIT, K_MSEC(CONFIG_ZMK_TRACKPAD_TICK_DURATION));
        if (sensor_trigger_set(trackpad, &trigger, handle_trackpad_ptp) < 0) {
            LOG_ERR("can't set trigger");
        };
    }
}

static int trackpad_init() {

#if IS_ENABLED(CONFIG_ZMK_TRACKPAD_WORK_QUEUE_DEDICATED)
    k_work_queue_start(&trackpad_work_q, trackpad_work_stack_area,
                       K_THREAD_STACK_SIZEOF(trackpad_work_stack_area),
                       CONFIG_ZMK_TRACKPAD_DEDICATED_THREAD_PRIORITY, NULL);
#endif
    button_mode = true;
    surface_mode = true;
    zmk_trackpad_set_mouse_mode(true);
    enabled = true;
    return 0;
}

static int trackpad_event_listener(const zmk_event_t *eh) {
    // zmk_trackpad_set_mouse_mode(true);
    zmk_hid_ptp_set_feature_mode_report(0);
    return 0;
}

static ZMK_LISTENER(trackpad, trackpad_event_listener);
static ZMK_SUBSCRIPTION(trackpad, zmk_endpoint_changed);

SYS_INIT(trackpad_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);