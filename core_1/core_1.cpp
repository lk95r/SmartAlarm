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
    return 0;
}