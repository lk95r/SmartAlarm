#include "rgb_lights.h"
#include <stdio.h>
#include <stdlib.h>

#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "hardware/gpio.h"
#include "pico/multicore.h"
#include "ws2812.pio.h"

uint32_t pixelsb[STRING_LEN];
uint32_t pixelsr[STRING_LEN];
uint32_t pixelsg[STRING_LEN];

int bit_depth = 12;
const int PIN_TX = 2;

static inline void put_pixel(uint32_t pixel_grb)
{
    pio_sm_put_blocking(pio0, 0, pixel_grb << 8u);
}

static inline uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b)
{
    return ((uint32_t)(r) << 8) |
           ((uint32_t)(g) << 16) |
           (uint32_t)(b);
}

void setup_pio(void)
{
    PIO pio = pio0;
    int sm = 0;
    uint offset = pio_add_program(pio, &ws2812_program);
    ws2812_program_init(pio, sm, offset, PIN_TX, 800000, false);
}

void set_ring(void)
{
    for(int j = 0;j<25;j++)
    {
    for (int i = 0; i < STRING_LEN; i++)
    {
        put_pixel(urgb_u32(10*j, 2*j, 5*j));
        
    }
        sleep_ms(200);
    }
}