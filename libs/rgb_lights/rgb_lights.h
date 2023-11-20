#ifndef RGB_LIGHTS_H
#define RGB_LIGHTS_H

#include <stdio.h>
#include <stdlib.h>

#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "pico/multicore.h"

#include "ws2812.pio.h"


#define STRING_LEN 16


void setup_pio(void);
void set_ring(uint8_t r, uint8_t g, uint8_t b);
static inline void put_pixel(uint32_t pixel_grb);
static inline uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b);

#endif