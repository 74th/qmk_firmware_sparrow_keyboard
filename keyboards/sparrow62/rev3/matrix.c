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
#include <print.h>
#include "platforms/chibios/gpio.h"
#include "i2c_master.h"

#define RIGHT_I2C_ADDRESS 0x20
#define BASE_I2C_REGISTER_ADDRESS 0x00

#ifndef I2C_TIMEOUT
#    define I2C_TIMEOUT 100
#endif
#ifndef CUSTOM_MATRIX_DEBUG_INTERVAL
#    define CUSTOM_MATRIX_DEBUG_INTERVAL 500
#endif

uint8_t LEFT_COLS[]      = {GP2, GP3, GP4, GP5, GP6, GP8, GP9};
uint8_t LEFT_ROWS[]      = {GP11, GP12, GP13, GP14, GP15};
uint8_t RIGHT_COL_BITS[] = {1, 1 << 1, 1 << 2, 1 << 3, 1 << 4, 1 << 5, 1 << 6};

#define LEFT_COLS_SIZE sizeof(LEFT_COLS)
#define ROW_SIZE sizeof(LEFT_ROWS)
// 現状1列空ける
#define RIGHT_COLS_START sizeof(LEFT_COLS) + 1

static uint16_t d_timer = 0;

void matrix_init_custom(void) {
    i2c_init();
    wait_ms(10);

    // MCU側のMatrixのピンの方向設定
    for (int i = 0; i < sizeof(LEFT_ROWS); i++) {
        gpio_set_pin_input_low(LEFT_ROWS[i]);
    }
    for (int i = 0; i < sizeof(LEFT_COLS); i++) {
        gpio_set_pin_output_push_pull(LEFT_COLS[i]);
    }

}

bool matrix_scan_custom(matrix_row_t current_matrix[]) {
    bool debug = false;
    if (timer_elapsed(d_timer) > CUSTOM_MATRIX_DEBUG_INTERVAL) {
        debug   = true;
        d_timer = timer_read();
    }

    if (debug) {
        dprintf("-- matrix_scan_custom --\n");
    }

    matrix_row_t scaned_matrix[MATRIX_ROWS];
    memset(scaned_matrix, 0, sizeof(scaned_matrix));

    for (int col = 0; col < MATRIX_COLS; col++) {

        if (col < LEFT_COLS_SIZE) {
            // MCU側のMatrix
            for (int i = 0; i < LEFT_COLS_SIZE; i++) {
                if (i == col) {
                    gpio_write_pin_high(LEFT_COLS[i]);
                } else {
                    gpio_write_pin_low(LEFT_COLS[i]);
                }
            }

            wait_us(100);

            for (int row = 0; row < ROW_SIZE; row++) {
                bool v = gpio_read_pin(LEFT_ROWS[row]);
                if (v) {
                    scaned_matrix[row] |= 1 << col;
                }
            }
        }
    }

    // 右手側のMatrix
    uint8_t read_buf[ROW_SIZE] = {0};
    i2c_status_t status = i2c_read_register(RIGHT_I2C_ADDRESS << 1, BASE_I2C_REGISTER_ADDRESS, read_buf, ROW_SIZE, I2C_TIMEOUT);

    if (debug) {
        dprintf("read I2C i2c_status_t:%d value:0x%02X%02X%02X", status, read_buf[0], read_buf[1], read_buf[2]);
    }

    if (status == I2C_STATUS_SUCCESS) {
        for (int row = 0; row < ROW_SIZE; row++) {
            scaned_matrix[row] |= ((matrix_row_t)read_buf[row] << (RIGHT_COLS_START));
        }
    }

    bool updated = false;
    for (int row = 0; row < MATRIX_ROWS; row++) {
        if (current_matrix[row] != scaned_matrix[row]) {
            current_matrix[row] = scaned_matrix[row];
            updated             = true;
        }
    }

    if (debug) {
        dprintf("scaned 0x%04X 0x%04X 0x%04X 0x%04X 0x%04X\n", scaned_matrix[0], scaned_matrix[1], scaned_matrix[2], scaned_matrix[3], scaned_matrix[4]);
    }
    return updated;
}
