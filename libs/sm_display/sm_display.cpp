
#include <stdio.h>
#include "pico/stdlib.h"

#include "sm_display.h"

// Konstruktor
SM_Display::SM_Display(void)
{
    next_function = nullptr;
    next(&SM_Display::sm_state_init);
    test_print();
}

void SM_Display::run(displayDirection direction)
{
    if (next_function != nullptr)
    {
        (this->*next_function)(direction);
    }
    return;
}
void SM_Display::next(void (SM_Display::*funcptr)(displayDirection))
{
    next_function = funcptr;
}

void SM_Display::sm_state_init(displayDirection directeion)
{
    next(&SM_Display::sm_state_idle);
    return;
}

void SM_Display::sm_state_idle(displayDirection direction)
{
    static datetime_t dt;
    if (rtc_running())
    {
        printf("State: Idle \n");
        rtc_get_datetime(&dt);
        printf("%d:%d:%d\n", dt.hour, dt.min, dt.sec);
    }
    switch (direction)
    {
    case dUp:
        next(&SM_Display::sm_state_set_pre_runtime);
        break;
    case dEnter:
    case dDown:
        next(&SM_Display::sm_state_toggle_alarm);
        break;
    default:
        next(&SM_Display::sm_state_idle);
    }
}

void SM_Display::sm_state_toggle_alarm(displayDirection direction)
{
    printf("State: toggle Alarm \n");
    printf("____________\n");
    printf("Vorlaufzeit\n");
    printf(">Alarm (de)aktivieren\n");
    printf("Wecker stellen\n");
    printf("Uhrzeit einstellen\n");
    printf("____________\n");
    switch (direction)
    {
    case dEnter:
        printf("Alarm de(aktiviert)\n");
        next(&SM_Display::sm_state_idle);
        break;
    case dTimeout:
    case dLeft:
        next(&SM_Display::sm_state_idle);
        break;
    case dUp:
        next(&SM_Display::sm_state_set_pre_runtime);
        break;
    case dDown:
        next(&SM_Display::sm_state_set_alarm);
        break;
    default:
        next(&SM_Display::sm_state_toggle_alarm);
    }
}

void SM_Display::sm_state_set_alarm(displayDirection direction)
{
    printf("State: set_alarm\n");
    printf("____________\n");
    printf("Alarm (de)aktivieren\n");
    printf(">Wecker stellen\n");
    printf("Uhrzeit einstellen\n");
    printf("Weckton aussuchen\n");
    printf("____________\n");
    switch (direction)
    {
    case dTimeout:
    case dLeft:
        next(&SM_Display::sm_state_idle);
        break;
    case dEnter:
    case dRight:
        next(&SM_Display::sm_state_alarm_hour);
        break;
    case dUp:
        next(&SM_Display::sm_state_toggle_alarm);
        break;
    case dDown:
        next(&SM_Display::sm_state_set_clock);
        break;
    default:
        next(&SM_Display::sm_state_set_alarm);
    }
}

void SM_Display::sm_state_alarm_hour(displayDirection direction)
{
    static int temp_h = 6;
    printf("state: set alarm_h = %d\n", temp_h);

    switch (direction)
    {
    case dTimeout:
        next(&SM_Display::sm_state_idle);
        break;

    case dEnter:
        next(&SM_Display::sm_state_idle);
        break;
    case dUp:
        temp_h = (temp_h + 1) % 24;
        next(&SM_Display::sm_state_alarm_hour);
        break;
    case dDown:
        if (temp_h == 0)
        {
            temp_h = 23;
        }
        else
        {
            temp_h -= 1;
        }
        next(&SM_Display::sm_state_alarm_hour);
        break;
    case dLeft:
        next(&SM_Display::sm_state_set_alarm);
        break;
    case dRight:
        next(&SM_Display::sm_state_alarm_min);
        break;
    default:
        next(&SM_Display::sm_state_alarm_hour);
    }
}

void SM_Display::sm_state_alarm_min(displayDirection direction)
{
    static int temp_min = 6;
    printf("state: set alarm_min = %d\n", temp_min);
    switch (direction)
    {
    case dTimeout:
        next(&SM_Display::sm_state_idle);
        break;

    case dEnter:
        next(&SM_Display::sm_state_idle);
        break;
    case dUp:
        temp_min = (temp_min + 1) % 60;
        next(&SM_Display::sm_state_alarm_min);
        break;
    case dDown:
        if (temp_min == 0)
        {
            temp_min = 59;
        }
        else
        {
            temp_min -= 1;
        }
        next(&SM_Display::sm_state_alarm_min);
        break;
    case dLeft:
        next(&SM_Display::sm_state_alarm_hour);
        break;
    default:
        next(&SM_Display::sm_state_alarm_min);
    }
}

void SM_Display::sm_state_set_clock(displayDirection direction)
{
    printf("State: set_clock\n");
    printf("____________\n");
    printf("Wecker stellen\n");
    printf(">Uhrzeit einstellen\n");
    printf("Weckton aussuchen\n");
    printf("Vorlaufzeit einstellen\n");
    printf("____________\n");
    switch (direction)
    {
    case dTimeout:
    case dLeft:
        next(&SM_Display::sm_state_idle);
        break;
    case dEnter:
    case dRight:
        next(&SM_Display::sm_state_clock_hour);
        break;
    case dUp:
        next(&SM_Display::sm_state_set_alarm);
        break;
    case dDown:
        next(&SM_Display::sm_state_set_alarmtone);
        break;
    default:
        next(&SM_Display::sm_state_set_clock);
    }
}

void SM_Display::sm_state_clock_hour(displayDirection direction)
{
    static int temp_h = 6;
    printf("state: set clock_h = %d\n", temp_h);

    switch (direction)
    {
    case dTimeout:
        next(&SM_Display::sm_state_idle);
        break;

    case dEnter:
        next(&SM_Display::sm_state_idle);
        break;
    case dUp:
        temp_h = (temp_h + 1) % 24;
        next(&SM_Display::sm_state_clock_hour);
        break;
    case dDown:
        if (temp_h == 0)
        {
            temp_h = 23;
        }
        else
        {
            temp_h -= 1;
        }
        next(&SM_Display::sm_state_clock_hour);
        break;
    case dLeft:
        next(&SM_Display::sm_state_set_clock);
        break;
    case dRight:
        next(&SM_Display::sm_state_clock_min);
        break;
    default:
        next(&SM_Display::sm_state_clock_hour);
    }
}

void SM_Display::sm_state_clock_min(displayDirection direction)
{
    static int temp_min = 6;
    printf("state: set clock_min = %d\n", temp_min);
    switch (direction)
    {
    case dTimeout:
        next(&SM_Display::sm_state_idle);
        break;

    case dEnter:
        next(&SM_Display::sm_state_idle);
        break;
    case dUp:
        temp_min = (temp_min + 1) % 60;
        next(&SM_Display::sm_state_clock_min);
        break;
    case dDown:
        if (temp_min == 0)
        {
            temp_min = 59;
        }
        else
        {
            temp_min -= 1;
        }
        next(&SM_Display::sm_state_clock_min);
        break;
    case dLeft:
        next(&SM_Display::sm_state_clock_hour);
        break;
    default:
        next(&SM_Display::sm_state_clock_min);
    }
}

void SM_Display::sm_state_set_alarmtone(displayDirection direction)
{
    printf("State: set_alarmtone\n");
    printf("____________\n");
    printf("Uhrzeit einstellen\n");
    printf(">Weckton aussuchen\n");
    printf("Vorlaufzeit einstellen\n");
    printf("Wecker (de)aktivieren\n");
    printf("____________\n");
    switch (direction)
    {
    case dTimeout:
    case dLeft:
        next(&SM_Display::sm_state_idle);
        break;
    case dEnter:
    case dRight:
        next(&SM_Display::sm_state_adjust_tone);
        break;
    case dUp:
        next(&SM_Display::sm_state_set_clock);
        break;
    case dDown:
        next(&SM_Display::sm_state_set_pre_runtime);
        break;
    default:
        next(&SM_Display::sm_state_set_alarmtone);
    }
}

void SM_Display::sm_state_adjust_tone(displayDirection direction)
{
    static int tone_nr = 0;
    printf("Sound %d\n", tone_nr);
    switch (direction)
    {
    case dTimeout:
        next(&SM_Display::sm_state_idle);
        break;
    case dEnter:
        next(&SM_Display::sm_state_idle);
        break;
    case dLeft:
        next(&SM_Display::sm_state_set_alarmtone);
        break;
    case dUp:
        tone_nr++;
        next(&SM_Display::sm_state_adjust_tone);
        break;
    case dDown:
        tone_nr--;
        next(&SM_Display::sm_state_adjust_tone);
        break;
    default:
        next(&SM_Display::sm_state_adjust_tone);
    }
}

void SM_Display::sm_state_set_pre_runtime(displayDirection direction)
{
    printf("State: set_pre_runtime\n");
    printf("____________\n");
    printf("Weckton aussuchen\n");
    printf(">Vorlaufzeit einstellen\n");
    printf("Wecker (de)aktivieren\n");
    printf("Wecker stellen\n");
    printf("____________\n");
    switch (direction)
    {
    case dTimeout:
    case dLeft:
        next(&SM_Display::sm_state_idle);
        break;
    case dEnter:
    case dRight:
        next(&SM_Display::sm_state_adjust_time);
        break;
    case dUp:
        next(&SM_Display::sm_state_set_alarmtone);
        break;
    case dDown:
        next(&SM_Display::sm_state_toggle_alarm);
        break;
    default:
        next(&SM_Display::sm_state_set_pre_runtime);
    }
}

void SM_Display::sm_state_adjust_time(displayDirection direction)
{
    static int duration_min = 0;
    printf(" %d min\n", duration_min);
    switch (direction)
    {
    case dTimeout:
        next(&SM_Display::sm_state_idle);
        break;
    case dEnter:
        next(&SM_Display::sm_state_idle);
        break;
    case dLeft:
        next(&SM_Display::sm_state_set_pre_runtime);
        break;
    case dUp:
        duration_min++;
        next(&SM_Display::sm_state_adjust_time);
        break;
    case dDown:
        duration_min--;
        next(&SM_Display::sm_state_adjust_time);
        break;
    default:
        next(&SM_Display::sm_state_adjust_time);
    }
}

void SM_Display::sm_state_on(displayDirection direction)
{
    printf("on\n");
    switch (direction)
    {
    case dLeft:
        next(&SM_Display::sm_state_off);
        printf("next: off\n");
        break;
    default:
        next(&SM_Display::sm_state_on);
        printf("next: stay on \n");
    }
    return;
}

void SM_Display::sm_state_off(displayDirection direction)
{
    printf("off\n");
    switch (direction)
    {
    case dRight:
        next(&SM_Display::sm_state_on);
        printf("next: on\n");
        break;
    default:
        next(&SM_Display::sm_state_off);
        printf("next: stay off\n");
    }
    return;
}