#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/pio.h"
#include "hardware/interp.h"
#include "hardware/timer.h"
#include "hardware/watchdog.h"
#include "hardware/clocks.h"
#include "pico/cyw43_arch.h"
#include "lwipopts.h"


#include "pico/util/queue.h"
#include "pico/multicore.h"
#include "core_1/core_1.h"
#define FLAG_VALUE 123

typedef struct
{
    int32_t (*func)(int32_t);
    int32_t data;
} queue_entry_t;

queue_t call_queue;
queue_t results_queue;


// SPI Defines
// We are going to use SPI 0, and allocate it to the following GPIO pins
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define SPI_PORT spi0
#define PIN_MISO 16
#define PIN_CS   17
#define PIN_SCK  18
#define PIN_MOSI 19



void core1_entry() {
    core_1_main();
}


#define TEST_NUM 10
int main()
{
    stdio_init_all();

    // SPI initialisation. This example will use SPI at 1MHz.
    //spi_init(SPI_PORT, 1000*1000);
    //gpio_set_function(PIN_MISO, GPIO_FUNC_SPI);
    //gpio_set_function(PIN_CS,   GPIO_FUNC_SIO);
    //gpio_set_function(PIN_SCK,  GPIO_FUNC_SPI);
    //gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);
    //
    //// Chip select is active-low, so we'll initialise it to a driven-high state
    //gpio_set_dir(PIN_CS, GPIO_OUT);
    //gpio_put(PIN_CS, 1);

    multicore_launch_core1(core1_entry);

    return 0;
}
