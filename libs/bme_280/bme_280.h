#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <cstring>

#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/gpio.h"
#include "bme_280_defines.h"

#define DEFAULT_BAUDRATE 100*1000

typedef struct {
    uint32_t pressure;
    int32_t temperature;
    uint32_t humidity;
}sens_data_t;

class BME280{
    public:
    BME280(void);
    uint init(void* i2c, uint baudrate, uint i2c_address,uint pin_scl, uint pin_sda);
    uint test_device_id(void);
    int32_t get_temperature(void);
    
    sens_data_t data;
    
    private:
    void* m_i2c_inst;
    uint m_baudrate;
    uint m_i2c_address;
    uint m_pin_scl;
    uint m_pin_sda;
    sens_data m_comp_data;
    sens_data m_uncomp_data;

    bme280_calib_data m_cal_values;
    int32_t compensate_temperature(void);
    void read_data(void);
    void read_sensor_status(void);
    //int32_t compensate_pressure(void);
    //int32_t compensate_humidity(void);
    void get_calib_data(void);
    void parse_temp_pres_calib_data(uint8_t * calib_data,uint8_t size);
    void parse_hum_calib_data(uint8_t* calib_data,uint8_t size);
};