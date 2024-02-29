/*
Copyright 2024 Atsushi Morimoto (@74th)

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef SIMPLE_POINTER_TIMEOUT
#    define SIMPLE_POINTER_TIMEOUT 100
#endif

#ifndef SIMPLE_POINTER_POINTER_SCALE
#    define SIMPLE_POINTER_POINTER_SCALE 5
#endif

#ifndef SIMPLE_POINTER_WHEEL_SCALE
#    define SIMPLE_POINTER_WHEEL_SCALE 2
#endif

#ifndef SIMPLE_POINTER_DEBUG_INTERVAL
#    define SIMPLE_POINTER_DEBUG_INTERVAL 100
#endif

#ifndef SIMPLE_POINTER_ADDRESS
#    define SIMPLE_POINTER_ADDRESS 0x0B
#endif

void           simple_pointer_device_init(void);
report_mouse_t simple_pointer_get_report(report_mouse_t mouse_report);
uint16_t       simple_pointer_get_cpi(void);
void           simple_pointer_set_cpi(uint16_t cpi);
