#include "include/BMP280.h"
#include <stdio.h>

// Variável global para calibração
static bmp280_calib_data_t calib_data;
static int32_t t_fine;

static uint8_t bmp280_read8(i2c_inst_t *i2c, uint8_t reg) {
    uint8_t val;
    i2c_write_blocking(i2c, BMP280_ADDR, &reg, 1, true);
    i2c_read_blocking(i2c, BMP280_ADDR, &val, 1, false);
    return val;
}

static void bmp280_read_calibration(i2c_inst_t *i2c) {
    uint8_t reg = 0x88;
    uint8_t buf[24];
    i2c_write_blocking(i2c, BMP280_ADDR, &reg, 1, true);
    i2c_read_blocking(i2c, BMP280_ADDR, buf, 24, false);

    calib_data.dig_T1 = (buf[1] << 8) | buf[0];
    calib_data.dig_T2 = (buf[3] << 8) | buf[2];
    calib_data.dig_T3 = (buf[5] << 8) | buf[4];
    calib_data.dig_P1 = (buf[7] << 8) | buf[6];
    calib_data.dig_P2 = (buf[9] << 8) | buf[8];
    calib_data.dig_P3 = (buf[11] << 8) | buf[10];
    calib_data.dig_P4 = (buf[13] << 8) | buf[12];
    calib_data.dig_P5 = (buf[15] << 8) | buf[14];
    calib_data.dig_P6 = (buf[17] << 8) | buf[16];
    calib_data.dig_P7 = (buf[19] << 8) | buf[18];
    calib_data.dig_P8 = (buf[21] << 8) | buf[20];
    calib_data.dig_P9 = (buf[23] << 8) | buf[22];
}

bool bmp280_init(i2c_inst_t *i2c) {
    uint8_t id = bmp280_read8(i2c, BMP280_REG_ID);
    if (id != 0x58) {
        printf("BMP280 não encontrado! ID: 0x%X\n", id);
        return false;
    }

    bmp280_read_calibration(i2c);

    // Configura modo normal, oversampling x1
    uint8_t ctrl_meas[2] = {BMP280_REG_CTRL_MEAS, 0x27}; // Temp/Press oversampling x1, modo normal
    i2c_write_blocking(i2c, BMP280_ADDR, ctrl_meas, 2, false);

    uint8_t config[2] = {BMP280_REG_CONFIG, 0xA0}; // Standby 1000ms, filtro off
    i2c_write_blocking(i2c, BMP280_ADDR, config, 2, false);

    return true;
}

bool bmp280_read_data(i2c_inst_t *i2c, bmp280_data_t *data) {
    uint8_t reg = BMP280_REG_PRESS_MSB;
    uint8_t buf[6];
    i2c_write_blocking(i2c, BMP280_ADDR, &reg, 1, true);
    i2c_read_blocking(i2c, BMP280_ADDR, buf, 6, false);

    int32_t adc_P = ((int32_t)buf[0] << 12) | ((int32_t)buf[1] << 4) | (buf[2] >> 4);
    int32_t adc_T = ((int32_t)buf[3] << 12) | ((int32_t)buf[4] << 4) | (buf[5] >> 4);

    // --- Compensação de temperatura ---
    int32_t var1, var2;
    var1 = ((((adc_T >> 3) - ((int32_t)calib_data.dig_T1 << 1))) * ((int32_t)calib_data.dig_T2)) >> 11;
    var2 = (((((adc_T >> 4) - ((int32_t)calib_data.dig_T1)) * ((adc_T >> 4) - ((int32_t)calib_data.dig_T1))) >> 12) *
           ((int32_t)calib_data.dig_T3)) >> 14;
    t_fine = var1 + var2;
    data->temperature = (t_fine * 5 + 128) >> 8;
    data->temperature /= 100.0f;

    // --- Compensação de pressão ---
    int64_t p;
    int64_t var1_p, var2_p;
    var1_p = ((int64_t)t_fine) - 128000;
    var2_p = var1_p * var1_p * (int64_t)calib_data.dig_P6;
    var2_p = var2_p + ((var1_p * (int64_t)calib_data.dig_P5) << 17);
    var2_p = var2_p + (((int64_t)calib_data.dig_P4) << 35);
    var1_p = ((var1_p * var1_p * (int64_t)calib_data.dig_P3) >> 8) + ((var1_p * (int64_t)calib_data.dig_P2) << 12);
    var1_p = (((((int64_t)1) << 47) + var1_p)) * ((int64_t)calib_data.dig_P1) >> 33;
    if (var1_p == 0) return false;
    p = 1048576 - adc_P;
    p = (((p << 31) - var2_p) * 3125) / var1_p;
    var1_p = (((int64_t)calib_data.dig_P9) * (p >> 13) * (p >> 13)) >> 25;
    var2_p = (((int64_t)calib_data.dig_P8) * p) >> 19;
    p = ((p + var1_p + var2_p) >> 8) + (((int64_t)calib_data.dig_P7) << 4);

    data->pressure = (float)p / 256.0f; // em Pa
    return true;
}