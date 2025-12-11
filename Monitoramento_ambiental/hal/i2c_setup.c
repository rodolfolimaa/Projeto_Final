#include "include/i2c_setup.h"
#include "pico/stdlib.h"

// Definições das interfaces I2C (globais)
const I2C_interface_t i2c_hw_iface0 = { .i2c = i2c0, .sda_pin = 0,  .scl_pin = 1  };
const I2C_interface_t i2c_hw_iface1 = { .i2c = i2c1, .sda_pin = 14, .scl_pin = 15 };

void setup_i2c(const I2C_interface_t *iface) {
    i2c_init(iface->i2c, 100 * 1000); // 100kHz
    gpio_set_function(iface->sda_pin, GPIO_FUNC_I2C);
    gpio_set_function(iface->scl_pin, GPIO_FUNC_I2C);
    gpio_pull_up(iface->sda_pin);
    gpio_pull_up(iface->scl_pin);
}
