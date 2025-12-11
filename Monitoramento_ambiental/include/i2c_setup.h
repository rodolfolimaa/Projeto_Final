#ifndef I2C_SETUP_H
#define I2C_SETUP_H

#include "hardware/i2c.h"

// Estrutura para definir uma interface I2C
typedef struct {
    i2c_inst_t *i2c;
    uint sda_pin;
    uint scl_pin;
} I2C_interface_t;

// Interfaces globais (declaradas em i2c_setup.c)
extern const I2C_interface_t i2c_hw_iface0;
extern const I2C_interface_t i2c_hw_iface1;

// Função de inicialização
void setup_i2c(const I2C_interface_t *iface);

#endif
