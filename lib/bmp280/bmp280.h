#pragma once

#include <stdio.h>
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_i2c.h"
#include "../printf/retarget.h"
#include <math.h>

uint8_t init_bmp280(I2C_HandleTypeDef *i2c_handle_temp);
float bmp280_get_pressure_hPa();
float bmp280_get_temperature_celsius();
float bmp280_get_height_meters_above_sea_level(float pressure_sea_level_hpa, float temperature_sea_level);
float bmp280_get_height_meters_from_reference(uint8_t reset_reference);
float bmp280_get_height_centimeters_from_reference(uint8_t reset_reference);
void bmp280_set_reference_pressure_from_number_of_samples(uint16_t sample_size);