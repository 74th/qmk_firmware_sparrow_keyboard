#include "pointing_device.h"
#include "sparrowdial.h"
#include "i2c_master.h"

// clang-format off
#define PIMORONI_TRACKBALL_REG_LEFT  0x04
#define PIMORONI_TRACKBALL_REG_RIGHT 0x05
#define PIMORONI_TRACKBALL_REG_UP    0x06
#define PIMORONI_TRACKBALL_REG_DOWN  0x07
// clang-format on

typedef struct {
    uint8_t left;
    uint8_t right;
    uint8_t up;
    uint8_t down;
    uint8_t click;
} pimoroni_data_t;

void           pointing_device_driver_init(void);
report_mouse_t pointing_device_driver_get_report(report_mouse_t mouse_report);
uint16_t       pointing_device_driver_get_cpi(void);
void           pointing_device_driver_set_cpi(uint16_t cpi);

void pointing_device_driver_init(void) {
    i2c_init();
}

static uint16_t precision = 128;

mouse_xy_report_t pimoroni_trackball_adapt_values(clamp_range_t* offset) {
    if (*offset > XY_REPORT_MAX) {
        *offset -= XY_REPORT_MAX;
        return (mouse_xy_report_t)XY_REPORT_MAX;
    } else if (*offset < XY_REPORT_MIN) {
        *offset += XY_REPORT_MAX;
        return (mouse_xy_report_t)XY_REPORT_MIN;
    } else {
        mouse_xy_report_t temp_return = *offset;
        *offset                       = 0;
        return temp_return;
    }
}

i2c_status_t read_pimoroni_trackball(pimoroni_data_t* data) {
    i2c_status_t status = i2c_readReg(PIMORONI_TRACKBALL_ADDRESS << 1, PIMORONI_TRACKBALL_REG_LEFT, (uint8_t*)data, sizeof(*data), PIMORONI_TRACKBALL_TIMEOUT);

#ifdef POINTING_DEVICE_DEBUG
    static uint16_t d_timer;
    if (timer_elapsed(d_timer) > PIMORONI_TRACKBALL_DEBUG_INTERVAL) {
        pd_dprintf("Trackball READ i2c_status_t: %d L: %d R: %d Up: %d D: %d SW: %d\n", status, data->left, data->right, data->up, data->down, data->click);
        d_timer = timer_read();
    }
#endif

    return status;
}

int16_t pimoroni_trackball_get_offsets(uint8_t negative_dir, uint8_t positive_dir, uint8_t scale) {
    uint8_t offset     = 0;
    bool    isnegative = false;
    if (negative_dir > positive_dir) {
        offset     = negative_dir - positive_dir;
        isnegative = true;
    } else {
        offset = positive_dir - negative_dir;
    }
    uint16_t magnitude = (scale * offset * offset * precision) >> 7;
    return isnegative ? -(int16_t)(magnitude) : (int16_t)(magnitude);
}

report_mouse_t pointing_device_driver_get_report(report_mouse_t mouse_report) {
    static uint16_t      debounce      = 0;
    static uint8_t       error_count   = 0;
    pimoroni_data_t      pimoroni_data = {0};
    static clamp_range_t x_offset = 0, y_offset = 0;

    if (error_count < PIMORONI_TRACKBALL_ERROR_COUNT) {
        i2c_status_t status = read_pimoroni_trackball(&pimoroni_data);

        if (status == I2C_STATUS_SUCCESS) {
            error_count = 0;

            if (!(pimoroni_data.click & 128)) {
                mouse_report.buttons = pointing_device_handle_buttons(mouse_report.buttons, false, POINTING_DEVICE_BUTTON1);
                if (!debounce) {
                    x_offset += pimoroni_trackball_get_offsets(pimoroni_data.right, pimoroni_data.left, PIMORONI_TRACKBALL_SCALE);
                    y_offset += pimoroni_trackball_get_offsets(pimoroni_data.down, pimoroni_data.up, PIMORONI_TRACKBALL_SCALE);
                    mouse_report.x = pimoroni_trackball_adapt_values(&x_offset);
                    mouse_report.y = pimoroni_trackball_adapt_values(&y_offset);
                } else {
                    debounce--;
                }
            } else {
                mouse_report.buttons = pointing_device_handle_buttons(mouse_report.buttons, true, POINTING_DEVICE_BUTTON1);
                debounce             = PIMORONI_TRACKBALL_DEBOUNCE_CYCLES;
            }
        } else {
            error_count++;
        }
    }
    return mouse_report;
}

uint16_t pointing_device_driver_get_cpi(void) {
    return 0;
}

void pointing_device_driver_set_cpi(uint16_t cpi) {}
