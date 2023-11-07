
#include <stdio.h>
#include "pico/stdlib.h"

#include "sm_display.h"

//Konstruktor
SM_Display::SM_Display(void){
    next_function = nullptr;
    next(&SM_Display::sm_state_on);
    test_print();
}

void SM_Display::run(displayDirection direction){
    if(next_function != nullptr){
        (this->*next_function)(direction);
    }
    return;
}
void SM_Display::next(void (SM_Display::*funcptr)(displayDirection)){
    next_function = funcptr;
}

void SM_Display::sm_state_on(displayDirection direction){
    printf("on\n");
    switch(direction){
        case dLeft: next(&SM_Display::sm_state_off);
                    printf("next: off\n");
                    break;
        default: next(&SM_Display::sm_state_on);
                printf("next: stay on \n");
    }
    return;
}

void SM_Display::sm_state_off(displayDirection direction){
    printf("off\n");
    switch(direction){
        case dRight: next(&SM_Display::sm_state_on);
                     printf("next: on\n");
                     break;
        default: next(&SM_Display::sm_state_off);
                printf("next: stay off\n");
    }
    return;
}