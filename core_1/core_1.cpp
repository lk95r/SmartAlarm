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
#include "ss_oled.hpp"

#include "core_1.h"

queue_t q_date_time;
bool date_received = false;

#define DISPLAY_TEST 0
#define ADC_TEST 0
#if DISPLAY_TEST
// RPI Pico
#define SDA_PIN 6
#define SCL_PIN 7
#define PICO_I2C i2c1
#define I2C_SPEED 100 * 1000

#define OLED_WIDTH 128
#define OLED_HEIGHT 64
#endif

// Called with results of operation
static void set_clock(void)
{
  datetime_t dt;

  dt.year = 2023;
  dt.month = 11;
  dt.day = 11;
  dt.dotw = 6;
  dt.hour = 11;
  dt.min = 11;
  dt.sec = 11;

  rtc_init();
  if (rtc_set_datetime(&dt))
  {
    date_received = true;
  }
  printf("c1: uhr läuft");
}

int core_1_main(void)
{
  printf("c1: cor1main accessed\n");
  set_clock();
#if ADC_TEST
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
#if DISPLAY_TEST
  static uint8_t ucBuffer[1024];
  uint8_t uc[8];
  int i, j, rc;
  char szTemp[32];
  picoSSOLED myOled(OLED_128x64, 0x3c, 0, 0, PICO_I2C, SDA_PIN, SCL_PIN, I2C_SPEED);

  rc = myOled.init();
  myOled.set_back_buffer(ucBuffer);
  myOled.fill(0,1);
  while (1)
  {
    /**
     * "Alarm (de)aktivieren\n");
        ">Wecker stellen\n");
        "Uhrzeit einstellen\n");
        "Weckton aussuchen\n");
     * 
     */
    if (rc != OLED_NOT_FOUND)
    {
      //myOled.fill(0, 1);
      myOled.set_contrast(50);
      myOled.write_string(0, 0, 0, (char *)"*************",FONT_8x8, 0,1);
      myOled.write_string(0, 0, 1, (char *)"Toggle Alarm", FONT_8x8, 0, 1);
      myOled.write_string(0, 0, 2, (char *)">Set Alarm", FONT_8x8, 0, 1);
      myOled.write_string(0, 0, 3, (char *)"Set Clock", FONT_8x8, 0, 1);
      myOled.write_string(0, 0, 4, (char *)"Set Tone", FONT_8x8, 0, 1);
      myOled.write_string(0, 0, 5, (char *)"Set Runtime",FONT_8x8,0,1);
      myOled.write_string(0, 0, 6, (char *)"Set Bringhtness",FONT_8x8, 0, 1);
      myOled.write_string(0, 0, 7, (char *)"*************",FONT_8x8,0, 1);

    }
    /**
    myOled.fill(0, 1);
    myOled.write_string(0, 0, 0, (char *)"Now with 5 font sizes", FONT_6x8, 0, 1);
    myOled.write_string(0, 0, 1, (char *)"6x8 8x8 16x16", FONT_8x8, 0, 1);
    myOled.write_string(0, 0, 2, (char *)"16x32 and a new", FONT_8x8, 0, 1);
    myOled.write_string(0, 0, 3, (char *)"Stretched", FONT_12x16, 0, 1);
    myOled.write_string(0, 0, 5, (char *)"from 6x8", FONT_12x16, 0, 1);
    sleep_ms(5000); 

    int x, y;
    myOled.fill(0, 1);
    myOled.write_string(0, 0, 0, (char *)"Backbuffer Test", FONT_NORMAL, 0, 1);
    myOled.write_string(0, 0, 1, (char *)"96 lines", FONT_NORMAL, 0, 1);
    sleep_ms(2000);
    for (x = 0; x < OLED_WIDTH - 1; x += 2)
    {
      myOled.draw_line(x, 0, OLED_WIDTH - x, OLED_HEIGHT - 1, 1);
    };
    for (y = 0; y < OLED_HEIGHT - 1; y += 2)
    {
      myOled.draw_line(OLED_WIDTH - 1, y, 0, OLED_HEIGHT - 1 - y, 1);
    };
    myOled.write_string(0, 0, 1, (char *)"Without backbuffer", FONT_SMALL, 0, 1);
    sleep_ms(2000);
    myOled.fill(0, 1);
    for (x = 0; x < OLED_WIDTH - 1; x += 2)
    {
      myOled.draw_line(x, 0, OLED_WIDTH - 1 - x, OLED_HEIGHT - 1, 0);
    }
    for (y = 0; y < OLED_HEIGHT - 1; y += 2)
    {
      myOled.draw_line(OLED_WIDTH - 1, y, 0, OLED_HEIGHT - 1 - y, 0);
    }
    myOled.dump_buffer(ucBuffer);
    myOled.write_string(0, 0, 1, (char *)"With backbuffer", FONT_SMALL, 0, 1);
    sleep_ms(2000); */
  }
#endif
  return 0;
}