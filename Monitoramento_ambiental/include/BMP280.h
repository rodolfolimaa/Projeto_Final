#ifndef BMP280_H
#define BMP280_H

#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include <stdint.h>
#include <stdbool.h>

// Endereço I2C padrão do BMP280
#define BMP280_ADDR 0x76

// Registradores importantes
#define BMP280_REG_ID          0xD0
#define BMP280_REG_RESET       0xE0
#define BMP280_REG_STATUS      0xF3
#define BMP280_REG_CTRL_MEAS   0xF4
#define BMP280_REG_CONFIG      0xF5
#define BMP280_REG_PRESS_MSB   0xF7

// Estrutura para guardar coeficientes de calibração
typedef struct {
    uint16_t dig_T1;
    int16_t  dig_T2;
    int16_t  dig_T3;
    uint16_t dig_P1;
    int16_t  dig_P2;
    int16_t  dig_P3;
    int16_t  dig_P4;
    int16_t  dig_P5;
    int16_t  dig_P6;
    int16_t  dig_P7;
    int16_t  dig_P8;
    int16_t  dig_P9;
} bmp280_calib_data_t;

// Estrutura para resultados
typedef struct {
    float temperature;
    float pressure;
} bmp280_data_t;

// Funções públicas
bool bmp280_init(i2c_inst_t *i2c);
bool bmp280_read_data(i2c_inst_t *i2c, bmp280_data_t *data);

#endif