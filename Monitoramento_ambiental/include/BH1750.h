#ifndef BH1750_H
#define BH1750_H

#include "hardware/i2c.h"
#include <stdint.h>

// Endereço I2C do sensor BH1750
#define BH1750_ADDR 0x23

// Comandos BH1750
#define BH1750_POWER_ON   0x01
#define BH1750_RESET      0x07
#define BH1750_CONT_HRES  0x10

// Inicializa o sensor BH1750
void bh1750_init(i2c_inst_t *i2c);

// Lê luminosidade em Lux
float bh1750_read_lux(i2c_inst_t *i2c);

#endif