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

#define MCP23017_I2C_ADDRESS 0x20
#define MCP23017_IODIR_A 0x00
#define MCP23017_IODIR_B 0x01
#define MCP23017_GPIO_A 0x12
#define MCP23017_GPIO_B 0x13
#define MCP23017_IOCON_BANK0 0x0A
#define MCP23017_IOCON_BANK1 0x05

#ifndef MCP21017_I2C_TIMEOUT
#    define MCP21017_I2C_TIMEOUT 100
#endif
#ifndef CUSTOM_MATRIX_DEBUG_INTERVAL
#    define CUSTOM_MATRIX_DEBUG_INTERVAL 500
#endif

uint8_t LEFT_COLS[]      = {GP5, GP6, GP7, GP8, GP9, GP10, GP11};
uint8_t LEFT_ROWS[]      = {GP0, GP1, GP2, GP3, GP4};
uint8_t RIGHT_COL_BITS[] = {1, 1 << 1, 1 << 2, 1 << 3, 1 << 4, 1 << 5, 1 << 6, 1 << 7};
uint8_t RIGHT_ROW_BITS[] = {1, 1 << 1, 1 << 2, 1 << 3, 1 << 4};

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

    // BANK0ICON設定（念のため）
    uint8_t iocon = 0x00;
    i2c_status_t status = i2c_write_register(MCP23017_I2C_ADDRESS << 1, MCP23017_IOCON_BANK0, &iocon, 1, MCP21017_I2C_TIMEOUT);
    dprintf("set I2C IODIR_A i2c_status_t:%d\n", status);
    status = i2c_write_register(MCP23017_I2C_ADDRESS << 1, MCP23017_IOCON_BANK1, &iocon, 1, MCP21017_I2C_TIMEOUT);
    dprintf("set I2C IODIR_A i2c_status_t:%d\n", status);

    // GPIOAをInputに設定
    uint8_t      iodir_a  = 0xff;
    status = i2c_write_register(MCP23017_I2C_ADDRESS << 1, MCP23017_IODIR_A, &iodir_a, 1, MCP21017_I2C_TIMEOUT);
    dprintf("set I2C IODIR_A i2c_status_t:%d\n", status);

    // GPIOBをOutputに設定
    uint8_t      iodir_b  = 0x00;
    status = i2c_write_register(MCP23017_I2C_ADDRESS << 1, MCP23017_IODIR_B, &iodir_b, 1, MCP21017_I2C_TIMEOUT);
    dprintf("set I2C IODIR_B i2c_status_t:%d\n", status);
}

bool matrix_scan_custom(matrix_row_t current_matrix[]) {
    bool debug = false;
    if (timer_elapsed(d_timer) > CUSTOM_MATRIX_DEBUG_INTERVAL) {
        debug   = true;
        d_timer = timer_read();
    }

    if (debug) {
        dprintf("-- matrix_scan_custom --\n");

        // MCP23017の設定の確認
        uint8_t iocon;
        uint8_t ioaddr_a;
        uint8_t ioaddr_b;
        i2c_status_t op1 = i2c_read_register(MCP23017_I2C_ADDRESS << 1, MCP23017_IOCON_BANK0, &iocon, 1, MCP21017_I2C_TIMEOUT);
        i2c_status_t op2 = i2c_read_register(MCP23017_I2C_ADDRESS << 1, MCP23017_IODIR_A, &ioaddr_a, 1, MCP21017_I2C_TIMEOUT);
        i2c_status_t op3 = i2c_read_register(MCP23017_I2C_ADDRESS << 1, MCP23017_IODIR_B, &ioaddr_b, 1, MCP21017_I2C_TIMEOUT);
        dprintf("IOCON:%02X IODIR_A:%02X IODIR_B:%02X (status:%d, %d, %d)\n", iocon, ioaddr_a, ioaddr_b, op1, op2, op3);
    }

    matrix_row_t scaned_matrix[MATRIX_ROWS];
    memset(scaned_matrix, 0, sizeof(scaned_matrix));

    for (int col = 0; col < MATRIX_COLS; col++) {
        if (col < sizeof(LEFT_COLS)) {
            // MCU側のMatrix
            for (int i = 0; i < sizeof(LEFT_COLS); i++) {
                if (i == col) {
                    gpio_write_pin_high(LEFT_COLS[i]);
                } else {
                    gpio_write_pin_low(LEFT_COLS[i]);
                }
            }

            wait_us(100);

            for (int row = 0; row < sizeof(LEFT_ROWS); row++) {
                bool v = gpio_read_pin(LEFT_ROWS[row]);
                if (v) {
                    scaned_matrix[row] |= 1 << col;
                }
            }
        } else {
            // MCP23017側のMatrix
            // COL側、特定のCOLだけHIGHにして、他はLOWにする
            uint8_t      write_buf = RIGHT_COL_BITS[col - sizeof(LEFT_COLS)];
            i2c_status_t status    = i2c_write_register(MCP23017_I2C_ADDRESS << 1, MCP23017_GPIO_B, &write_buf, 1, MCP21017_I2C_TIMEOUT);
            if (debug) {
                dprintf("write I2C GPIOB i2c_status_t:%d value:0x%02X col:%d\n", status, write_buf, col);
            }

            if (status != I2C_STATUS_SUCCESS) {
                return 0;
            }

            // ROW側、読み取り
            uint8_t read_buf;
            status = i2c_read_register(MCP23017_I2C_ADDRESS << 1, MCP23017_GPIO_A, &read_buf, 1, MCP21017_I2C_TIMEOUT);

            if (debug) {
                dprintf("read I2C GPIOA i2c_status_t:%d value:0x%02X col:%d\n", status, read_buf, col);
            }

            if (status != I2C_STATUS_SUCCESS) {
                return 0;
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
