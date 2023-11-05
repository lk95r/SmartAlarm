#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/pio.h"
#include "hardware/interp.h"
#include "hardware/timer.h"
#include "hardware/watchdog.h"
#include "hardware/clocks.h"
#include "hardware/rtc.h"
#include "pico/util/datetime.h"


#include "pico/util/queue.h"
#include "pico/multicore.h"
#include "core_1/core_1.h"
#define FLAG_VALUE 123



extern queue_t q_date_time;
datetime_t dt;
bool error;
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
void check_rtc(){
    if(rtc_running()){
        printf("c0: rtc läuft\n");
        rtc_get_datetime(&dt);
        printf("%d:%d:%d\n",dt.hour,dt.min,dt.sec);
        return;
    }
    /* template for future queue implementation
    }else if(queue_is_full(&q_date_time)){
        printf("c0: waiting for data in q\n");
        queue_remove_blocking(&q_date_time,&dt);
        printf("c0: got data\n");
        printf("%d:%d:%d\n",dt.hour,dt.min,dt.sec);
        rtc_init();
        error = rtc_set_datetime(&dt);
        error? printf("set time successfull\n"):printf("error\n");
        return;
    } */
    return;
}

#define TEST_NUM 10
int main()
{

    stdio_init_all();
    printf("c0: started\n");
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
    /**template for future queue implementation*/
    //queue_init(&q_date_time,sizeof(dt),1);
    sleep_ms(5000);
    multicore_launch_core1(core1_entry);
    printf("c0: started core1\n");
    while(1){
        printf("c0: alive\n");
        sleep_ms(1000);
        check_rtc();
    }
    return 0;
}
