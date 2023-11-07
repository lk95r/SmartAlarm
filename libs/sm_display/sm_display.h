#pragma once
#include <stdio.h>
#include "pico/stdlib.h"

enum displayDirection {
    dNone,
    dUp,
    dDown,
    dLeft,
    dRight,
    dEnter
};

class SM_Display{
    public:
        SM_Display();
        void run(displayDirection);
    private:
        void (SM_Display:: *next_function)(displayDirection);
        void next(void (SM_Display::*funcptr)(displayDirection));
        void sm_state_on(displayDirection direction);
        void sm_state_off(displayDirection direction);
        void test_print(void){printf("Test Print sm_display\n");return;}; 
};  


