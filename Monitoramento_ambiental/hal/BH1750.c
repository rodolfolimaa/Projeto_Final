#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "include/BH1750.h"
#include "include/i2c_setup.h" // Inclui o cabeçalho do setup I2C

void bh1750_init(i2c_inst_t *i2c) {
    uint8_t cmd_on = BH1750_POWER_ON;
    i2c_write_blocking(i2c, BH1750_ADDR, &cmd_on, 1, false);

    uint8_t cmd_reset = BH1750_RESET;
    i2c_write_blocking(i2c, BH1750_ADDR, &cmd_reset, 1, false);

    uint8_t cmd_mode = BH1750_CONT_HRES;
    i2c_write_blocking(i2c, BH1750_ADDR, &cmd_mode, 1, false);

    sleep_ms(180); 
}

float bh1750_read_lux(i2c_inst_t *i2c) {
    uint8_t data[2];
    if (i2c_read_blocking(i2c, BH1750_ADDR, data, 2, false) < 0) return -1.0f;

    uint16_t raw = (data[0] << 8) | data[1];
    return (float)raw / 1.2f;
}