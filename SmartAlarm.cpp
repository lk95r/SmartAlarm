#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/pio.h"
#include "hardware/interp.h"
#include "hardware/timer.h"
#include "hardware/watchdog.h"
#include "hardware/clocks.h"
#include "pico/cyw43_arch.h"


#include "pico/util/queue.h"
#include "pico/multicore.h"

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


int64_t alarm_callback(alarm_id_t id, void *user_data) {
    // Put your timeout handler code in here
    return 0;
}


void core1_entry() {
    while (1) {
        // Function pointer is passed to us via the queue_entry_t which also
        // contains the function parameter.
        // We provide an int32_t return value by simply pushing it back on the
        // return queue which also indicates the result is ready.

        queue_entry_t entry;

        queue_remove_blocking(&call_queue, &entry);

        int32_t result = entry.func(entry.data);

        queue_add_blocking(&results_queue, &result);
    }
}

int32_t factorial(int32_t n) {
    int32_t f = 1;
    for (int i = 2; i <= n; i++) {
        f *= i;
    }
    return f;
}

int32_t fibonacci(int32_t n) {
    if (n == 0) return 0;
    if (n == 1) return 1;

    int n1 = 0, n2 = 1, n3 = 0;

    for (int i = 2; i <= n; i++) {
        n3 = n1 + n2;
        n1 = n2;
        n2 = n3;
    }
    return n3;
}

#define TEST_NUM 10
int main()
{
    stdio_init_all();

    // SPI initialisation. This example will use SPI at 1MHz.
    spi_init(SPI_PORT, 1000*1000);
    gpio_set_function(PIN_MISO, GPIO_FUNC_SPI);
    gpio_set_function(PIN_CS,   GPIO_FUNC_SIO);
    gpio_set_function(PIN_SCK,  GPIO_FUNC_SPI);
    gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);
    
    // Chip select is active-low, so we'll initialise it to a driven-high state
    gpio_set_dir(PIN_CS, GPIO_OUT);
    gpio_put(PIN_CS, 1);
    


    // Interpolator example code
    interp_config cfg = interp_default_config();
    // Now use the various interpolator library functions for your use case
    // e.g. interp_config_clamp(&cfg, true);
    //      interp_config_shift(&cfg, 2);
    // Then set the config 
    interp_set_config(interp0, 0, &cfg);

    // Timer example code - This example fires off the callback after 2000ms
    add_alarm_in_ms(2000, alarm_callback, NULL, false);
    int32_t res;

    printf("Hello, multicore_runner using queues!\n");

    // This example dispatches arbitrary functions to run on the second core
    // To do this we run a dispatcher on the second core that accepts a function
    // pointer and runs it. The data is passed over using the queue library from
    // pico_utils

    queue_init(&call_queue, sizeof(queue_entry_t), 2);
    queue_init(&results_queue, sizeof(int32_t), 2);

    multicore_launch_core1(core1_entry);

    queue_entry_t entry = {factorial, TEST_NUM};
    queue_add_blocking(&call_queue, &entry);

    // We could now do a load of stuff on core 0 and get our result later

    queue_remove_blocking(&results_queue, &res);

    printf("Factorial %d is %d\n", TEST_NUM, res);

    // Now try a different function
    entry.func = fibonacci;
    queue_add_blocking(&call_queue, &entry);

    queue_remove_blocking(&results_queue, &res);

    printf("Fibonacci %d is %d\n", TEST_NUM, res);




    puts("Hello, world!");

    return 0;
}
