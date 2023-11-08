#pragma once
#include <stdio.h>
#include "pico/stdlib.h"

#include "hardware/rtc.h"
#include "pico/util/datetime.h"

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
        void run(displayDirection);
    private:
        void (SM_Display:: *next_function)(displayDirection);
        void next(void (SM_Display::*funcptr)(displayDirection));
        void sm_state_init(displayDirection direction);
        void sm_state_idle(displayDirection direction);
        void sm_state_toggle_alarm(displayDirection direction);
        void sm_state_set_alarm(displayDirection direction); void sm_state_alarm_hour(displayDirection directon); void sm_state_alarm_min(displayDirection direction);
        void sm_state_set_clock(displayDirection direction); void sm_state_clock_hour(displayDirection direction); void sm_state_clock_min(displayDirection direction);
        void sm_state_set_alarmtone(displayDirection direction); void sm_state_adjust_tone(displayDirection direction);
        void sm_state_set_pre_runtime(displayDirection direction); void sm_state_adjust_time(displayDirection direction);

        void sm_state_on(displayDirection direction); //deprecated
        void sm_state_off(displayDirection direction); //deprecated
        void test_print(void){printf("Test Print sm_display\n");return;}; 
};  


