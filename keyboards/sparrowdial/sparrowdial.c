#include "timer.h"
#include "pointing_device.h"
#include "sparrowdial.h"
#include "i2c_master.h"
#include "pointing_device_internal.h"
#include "print.h"

// clang-format off
#define SIMPLE_POINTER_REG_LEFT     0x04
#define SIMPLE_POINTER_LEFT_CLICK   0x01
#define SIMPLE_POINTER_RIGHT_CLICK  0x02
#define SIMPLE_POINTER_MIDDLE_CLICK 0x03
// clang-format on

typedef struct {
    uint8_t click;
    int8_t pointer_x;
    int8_t pointer_y;
    int8_t wheel_h;
    int8_t wheel_v;
} simple_pointer_data_t;

void           pointing_device_driver_init(void);
report_mouse_t pointing_device_driver_get_report(report_mouse_t mouse_report);
uint16_t       pointing_device_driver_get_cpi(void);
void           pointing_device_driver_set_cpi(uint16_t cpi);

void pointing_device_driver_init(void) {
    i2c_init();
}

i2c_status_t read_simple_pointer(simple_pointer_data_t* data) {
    i2c_status_t status = i2c_readReg(SIMPLE_POINTER_ADDRESS << 1, SIMPLE_POINTER_REG_LEFT, (uint8_t*)data, sizeof(*data), SIMPLE_POINTER_TIMEOUT);

#ifdef POINTING_DEVICE_DEBUG
    static uint16_t d_timer;
    if (timer_elapsed(d_timer) > SIMPLE_POINTER_DEBUG_INTERVAL) {
        pd_dprintf("READ i2c_status_t: %d X: %d Y: %d WH: %d WV: %d SW: %d\n", status, data->pointer_x, data->pointer_y, data->wheel_h, data->wheel_v, data->click);
        d_timer = timer_read();
    }
#endif

    return status;
}

report_mouse_t pointing_device_driver_get_report(report_mouse_t mouse_report) {
    simple_pointer_data_t           data = {0};
    i2c_status_t status = read_simple_pointer(&data);

    if (status == I2C_STATUS_SUCCESS) {

        mouse_report.buttons = pointing_device_handle_buttons(mouse_report.buttons, data.click & SIMPLE_POINTER_LEFT_CLICK, POINTING_DEVICE_BUTTON1);
        mouse_report.buttons = pointing_device_handle_buttons(mouse_report.buttons, data.click & SIMPLE_POINTER_RIGHT_CLICK, POINTING_DEVICE_BUTTON2);
        mouse_report.buttons = pointing_device_handle_buttons(mouse_report.buttons, data.click & SIMPLE_POINTER_MIDDLE_CLICK, POINTING_DEVICE_BUTTON3);

        mouse_report.x = data.pointer_x * SIMPLE_POINTER_SCALE;
        mouse_report.y = data.pointer_y * SIMPLE_POINTER_SCALE;
        mouse_report.v = data.wheel_v;
        mouse_report.h = data.wheel_h;
    }
    return mouse_report;
}

uint16_t pointing_device_driver_get_cpi(void) {
    return 0;
}

void pointing_device_driver_set_cpi(uint16_t cpi) {}
