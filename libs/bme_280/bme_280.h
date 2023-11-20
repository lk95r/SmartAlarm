#pragma once

#include <stdio.h>
#include <stdlib.h>

#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/gpio.h"
#include "bme_280_defines.h"

#define DEFAULT_BAUDRATE 100*1000

class BME280{
    public:
    BME280(void);
    uint init(void* i2c, uint baudrate, uint i2c_address,uint pin_scl, uint pin_sda);
    uint test_device_id(void);
    private:
    void* m_i2c_inst;
    uint m_baudrate;
    uint m_i2c_address;
    uint m_pin_scl;
    uint m_pin_sda;
};