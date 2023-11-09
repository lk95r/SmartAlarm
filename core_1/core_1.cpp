/**
 * Copyright (c) 2022 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include <string.h>
#include <time.h>

#include "pico/stdlib.h"

#include "hardware/rtc.h"
#include "hardware/adc.h"
#include "pico/util/datetime.h"
#include "pico/util/queue.h"
#include "pico/multicore.h"


#include "core_1.h"

queue_t q_date_time;
bool date_received = false;


// Called with results of operation
static void set_clock(void) {
    datetime_t dt;

    dt.year = 2023;
    dt.month = 11;
    dt.day = 11;
    dt.dotw = 6;
    dt.hour = 11;
    dt.min = 11;
    dt.sec = 11;

    rtc_init();
    if(rtc_set_datetime(&dt)){
        date_received = true;
    }
    printf("c1: uhr läuft");


}


int core_1_main(void){
    printf("c1: cor1main accessed\n");
    set_clock();
    #if 0
    while(1){
         adc_select_input(0);
        uint adc_x_raw = adc_read();
        adc_select_input(1);
        uint adc_y_raw = adc_read();

        // Display the joystick position something like this:
        // X: [            o             ]  Y: [              o         ]
        const uint bar_width = 40;
        const uint adc_max = (1 << 12) - 1;
        uint bar_x_pos = adc_x_raw * bar_width / adc_max;
        uint bar_y_pos = adc_y_raw * bar_width / adc_max;
        printf("x: %d\ty: %d\n",adc_x_raw, adc_y_raw);
        /**
        printf("\rX: [");
        for (int i = 0; i < bar_width; ++i)
            putchar( i == bar_x_pos ? 'o' : ' ');
        printf("]  Y: [");
        for (int i = 0; i < bar_width; ++i)
            putchar( i == bar_y_pos ? 'o' : ' ');
        printf("]\n"); */
        sleep_ms(500);
    }
    #endif
    return 0;
}