CUSTOM_MATRIX = lite
SRC += matrix.c
I2C_DRIVER_REQUIRED = yes
RGBLIGHT_ENABLE = yes
# SparrowS v3 uses an RP2040.  `uf2boot` is only for STM32F103 boards.
BOOTLOADER = rp2040
