#pragma once
#include <stdio.h>
#include "pico/stdlib.h"

#include "hardware/rtc.h"
#include "pico/util/datetime.h"

#include "ss_oled.hpp"
#include "globals.h"

//Defines OLED for pico ss oled
#define SDA_PIN 6
#define SCL_PIN 7
#define PICO_I2C i2c1
#define I2C_SPEED 100 * 1000

#define OLED_WIDTH 128
#define OLED_HEIGHT 64

typedef struct{
    int x;
    int y;
}coordinate_t;

typedef struct{
    coordinate_t p1;
    coordinate_t p2;
    coordinate_t p3;
}triangle_t;

enum displayDirection {
    dNone,
    dUp,
    dDown,
    dLeft,
    dRight,
    dEnter,
    dTimeout
};

class SM_Display{
    public:
        SM_Display();
        int sm_set_callback(void* callback);
        uint16_t get_duration_min(void);
        datetime_t get_alarm_time(void);
        void run(displayDirection);
    private:
        datetime_t m_dt;
        datetime_t m_alarm_time;
        uint16_t duration_min = 0;
        bool m_alarm_active=false; //false when alarm is inactive, true when active
        void* ptr_alarm_callback;
        picoSSOLED m_Oled{OLED_128x64, 0x3c, 0, 0, PICO_I2C, SDA_PIN, SCL_PIN, I2C_SPEED};
        triangle_t tri_hour_top{
            .p1={16,0},
            .p2={100,30},
            .p3={16,30}
        };
        triangle_t tri_min_top; //TODO: enable display of shapes


        void (SM_Display:: *next_function)(displayDirection);
        void next(void (SM_Display::*funcptr)(displayDirection));
        void sm_state_init(displayDirection direction);
        void sm_state_idle(displayDirection direction);
        void sm_state_toggle_alarm(displayDirection direction);
        void sm_state_set_alarm(displayDirection direction); void sm_state_alarm_hour(displayDirection directon); void sm_state_alarm_min(displayDirection direction);
        void sm_state_set_clock(displayDirection direction); void sm_state_clock_hour(displayDirection direction); void sm_state_clock_min(displayDirection direction);
        void sm_state_set_alarmtone(displayDirection direction); void sm_state_adjust_tone(displayDirection direction);
        void sm_state_set_pre_runtime(displayDirection direction); void sm_state_adjust_time(displayDirection direction);
        void sm_m_state_adjust_brightness(displayDirection direction); void sm_state_adjust_brightness(displayDirection direction);

        void sm_state_on(displayDirection direction); //deprecated
        void sm_state_off(displayDirection direction); //deprecated
        void test_print(void){printf("Test Print sm_display\n");return;}; 

        void draw_triangle(triangle_t triangle);
};  


