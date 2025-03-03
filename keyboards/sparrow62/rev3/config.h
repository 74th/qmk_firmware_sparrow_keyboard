// Copyright 2023 Atsushi Morimoto (@74th)
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#define PICO_XOSC_STARTUP_DELAY_MULTIPLIER 64

#define MATRIX_ROWS 5
#define MATRIX_COLS 14

#define I2C1_SCL_PIN GP1
#define I2C1_SDA_PIN GP0
#define I2C_DRIVER I2CD0
#define F_SCL 100000

// 右手左側の追加4キーオプション使用時は1にする
#define USE_RIGHT_4KEYS 0