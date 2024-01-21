#include "pointing_device.h"

#ifndef PIMORONI_TRACKBALL_ADDRESS
#    define PIMORONI_TRACKBALL_ADDRESS 0x0A
#endif
#ifndef PIMORONI_TRACKBALL_SCALE
#    define PIMORONI_TRACKBALL_SCALE 5
#endif
#ifndef PIMORONI_TRACKBALL_DEBOUNCE_CYCLES
#    define PIMORONI_TRACKBALL_DEBOUNCE_CYCLES 20
#endif
#ifndef PIMORONI_TRACKBALL_ERROR_COUNT
#    define PIMORONI_TRACKBALL_ERROR_COUNT 10
#endif

#ifndef PIMORONI_TRACKBALL_TIMEOUT
#    define PIMORONI_TRACKBALL_TIMEOUT 100
#endif

#ifndef PIMORONI_TRACKBALL_DEBUG_INTERVAL
#    define PIMORONI_TRACKBALL_DEBUG_INTERVAL 100
#endif

void           pointing_device_driver_init(void);
report_mouse_t pointing_device_driver_get_report(report_mouse_t mouse_report);
uint16_t       pointing_device_driver_get_cpi(void);
void           pointing_device_driver_set_cpi(uint16_t cpi);
