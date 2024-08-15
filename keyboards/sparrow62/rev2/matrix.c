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

#include "timer.h"
#include "matrix.h"
#include "debug.h"
#include "wait.h"
#include "platforms/chibios/gpio.h"
#include "i2c_master.h"

#define MCP23017_I2C_ADDRESS 0x27

#ifndef MCP21017_I2C_TIMEOUT
#    define MCP21017_I2C_TIMEOUT 100
#endif
#ifndef DEBUG_INTERVAL
#    define MCP21017_I2C_TIMEOUT 100
#endif

#define CUSTOM_MATRIX_DEBUG_INTERVAL 500

uint8_t LEFT_COLS[]      = {GP5, GP6, GP7, GP8, GP9, GP10, GP11};
uint8_t LEFT_ROWS[]      = {GP0, GP1, GP2, GP3, GP4};
uint8_t RIGHT_COL_BITS[] = {1, 1 << 1, 1 << 2, 1 << 3, 1 << 4, 1 << 5, 1 << 6, 1 << 7};
uint8_t RIGHT_ROW_BITS[] = {1, 1 << 1, 1 << 2, 1 << 3};

static uint16_t d_timer = 0;

uint8_t matrix_init_custom(void) {
    i2c_init();
    wait_ms(10);

    for (int i = 0; i < sizeof(LEFT_ROWS); i++) {
        setPinInput(LEFT_ROWS[i]);
    }
    for (int i = 0; i < sizeof(LEFT_COLS); i++) {
        setPinOutputPushPull(GP5);
    }

    uint8_t      buf[]  = {0xff, 0x00};
    i2c_status_t status = i2c_writeReg(MCP23017_I2C_ADDRESS << 1, 0x05, buf, sizeof(buf), MCP21017_I2C_TIMEOUT);

    dprintf("set I2C IOCON i2c_status_t:%d\n", status);

    if (status == I2C_STATUS_SUCCESS) {
        return 1;
    }
    return 0;
}

bool matrix_scan_custom(matrix_row_t current_matrix[]) {
    bool debug = false;
    if (timer_elapsed(d_timer) > CUSTOM_MATRIX_DEBUG_INTERVAL) {
        debug   = true;
        d_timer = timer_read();
    }

    matrix_row_t scaned_matrix[MATRIX_ROWS];
    memset(scaned_matrix, 0, sizeof(scaned_matrix));

    for (int col = 0; col < MATRIX_COLS; col++) {
        if (col < sizeof(LEFT_COLS)) {
            for (int i = 0; i < sizeof(LEFT_COLS); i++) {
                if (i == col) {
                    writePinHigh(LEFT_COLS[i]);
                } else {
                    writePinLow(LEFT_COLS[i]);
                }
            }
            for (int row = 0; row < sizeof(LEFT_ROWS); row++) {
                if (readPin(LEFT_ROWS[row])) {
                    scaned_matrix[row] |= 1 << col;
                }
            }
        } else {
            uint8_t      write_buf = RIGHT_COL_BITS[col - sizeof(LEFT_COLS)];
            i2c_status_t status    = i2c_writeReg(MCP23017_I2C_ADDRESS << 1, 0x13 /* GPIOB */, &write_buf, 1, MCP21017_I2C_TIMEOUT);
            if (status != I2C_STATUS_SUCCESS) {
                return 0;
            }

            if (debug) {
                dprintf("write I2C GPIOB i2c_status_t:%d value:0x%02X col:%d\n", status, write_buf, col);
            }

            uint8_t read_buf;
            status = i2c_read_register(MCP23017_I2C_ADDRESS << 1, 0x12 /* GPIOA */, &read_buf, 1, MCP21017_I2C_TIMEOUT);
            if (status != I2C_STATUS_SUCCESS) {
                return 0;
            }

            if (debug) {
                dprintf("read I2C GPIOA i2c_status_t:%d value:0x%02X col:%d\n", status, read_buf, col);
            }

            for (int row = 0; row < sizeof(RIGHT_ROW_BITS); row++) {
                if (read_buf & RIGHT_ROW_BITS[row]) {
                    scaned_matrix[row] |= 1 << col;
                }
            }
        }
    }

    bool updated = false;
    for (int row = 0; row < MATRIX_ROWS; row++) {
        if(current_matrix[row] != scaned_matrix[row]){
            current_matrix[row] = scaned_matrix[row];
            updated = true;
        }
    }

    return updated;
}
