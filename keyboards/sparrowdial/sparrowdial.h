#include "pointing_device.h"

#ifndef SIMPLE_POINTER_TIMEOUT
#    define SIMPLE_POINTER_TIMEOUT 100
#endif

#ifndef SIMPLE_POINTER_SCALE
#    define SIMPLE_POINTER_SCALE 5
#endif

#ifndef SIMPLE_POINTER_DEBUG_INTERVAL
#    define SIMPLE_POINTER_DEBUG_INTERVAL 100
#endif

#ifndef SIMPLE_POINTER_ADDRESS
#    define SIMPLE_POINTER_ADDRESS 0x0B
#endif

void           pointing_device_driver_init(void);
report_mouse_t pointing_device_driver_get_report(report_mouse_t mouse_report);
uint16_t       pointing_device_driver_get_cpi(void);
void           pointing_device_driver_set_cpi(uint16_t cpi);
