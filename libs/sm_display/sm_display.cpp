
#include <stdio.h>
#include "pico/stdlib.h"

#include "sm_display.h"


/**
 * @brief Construct a new sm display::sm display object as a state machine
 * and initializes m_Oled object
 */
SM_Display::SM_Display(void): m_Oled(OLED_128x64, 0x3c, 0, 0, PICO_I2C, SDA_PIN, SCL_PIN, I2C_SPEED)
{
    next_function = nullptr;
    this->m_alarm_time={
        .year=-1,
        .month=-1,
        .day=-1,
        .dotw=-1,
        .hour=0,
        .min=0,
        .sec=0,
    };
    m_Oled.init();
    m_Oled.fill(0,1);
    next(&SM_Display::sm_state_init);
}

/**
 * @brief runs next iteration of State machine
 * 
 * @param direction input to statemachine
 */
void SM_Display::run(displayDirection direction)
{
    if (next_function != nullptr)
    {
        (this->*next_function)(direction);
    }
    return;
}

/**
 * @brief set callback function to be called for alarm
 * 
 * @param callback function pointer to callback function
 * @return int -1 when nullptr is passed
 * @return int 0 success
 */
int SM_Display::sm_set_callback(void* callback){
    if(callback == nullptr){
        return -1;
    }
    else{
        this->ptr_alarm_callback = callback; 
        return 0;
    }
    
}

/**
 * @brief set next function to be called by state machine needs to be a function from fsm
 * 
 * @param funcptr 
 */
void SM_Display::next(void (SM_Display::*funcptr)(displayDirection))
{
    next_function = funcptr;
}

/**
 * @brief init states -> idle
 * 
 * @param direction 
 */
void SM_Display::sm_state_init(displayDirection direction)
{
    next(&SM_Display::sm_state_idle);
    return;
}

/**
 * @brief idle state displays clock and temperatur
 * 
 * Next states: Menu
 * 
 * @param direction input to statemachine possible to open menu
 */
void SM_Display::sm_state_idle(displayDirection direction)
{

    if (rtc_running())
    {
        printf("State: Idle \n");
        rtc_get_datetime(&m_dt);
        printf("%d:%d:%d\n", m_dt.hour, m_dt.min, m_dt.sec);
    }
    switch (direction)
    {
    case dUp:
        next(&SM_Display::sm_m_state_adjust_brightness);
        break;
    case dEnter:
    case dDown:
        next(&SM_Display::sm_state_toggle_alarm);
        break;
    default:
        next(&SM_Display::sm_state_idle);
    }
}

/**
 * @brief displays menu and toggles alarm when enter is pressed
 * 
 * @param direction menu control/ dEnter -> Toggle alarm, idle / dUp/dDown scroll Menu / dLeft -> idle
 */
void SM_Display::sm_state_toggle_alarm(displayDirection direction){
    printf("State: toggle Alarm \n");
    printf("____________\n");
    printf("Vorlaufzeit einstellen\n");
    printf(">Alarm (de)aktivieren\n");
    printf("Wecker stellen\n");
    printf("Uhrzeit einstellen\n");
    printf("____________\n");
    //myOled.set_contrast(50);
    m_Oled.write_string(0, 0, 0, (char *)"*************",FONT_8x8, 0,1);
    m_Oled.write_string(0, 0, 1, (char *)">Toggle Alarm", FONT_8x8, 0, 1);
    m_Oled.write_string(0, 0, 2, (char *)"Set Alarm", FONT_8x8, 0, 1);
    m_Oled.write_string(0, 0, 3, (char *)"Set Clock", FONT_8x8, 0, 1);
    m_Oled.write_string(0, 0, 4, (char *)"Set Tone", FONT_8x8, 0, 1);
    m_Oled.write_string(0, 0, 5, (char *)"Set Runtime",FONT_8x8,0,1);
    m_Oled.write_string(0, 0, 6, (char *)"Set Bringhtness",FONT_8x8, 0, 1);
    m_Oled.write_string(0, 0, 7, (char *)"*************",FONT_8x8,0, 1);
    switch (direction)
    {
    case dEnter:
        m_Oled.fill(0,1);
        printf("Alarm de(aktiviert)\n");
        if(m_alarm_active==false){
            rtc_enable_alarm();
            m_alarm_active = true;
            //DEBUG
            printf("Alarm enabled");
        }else if(m_alarm_active==true){
            rtc_disable_alarm();
            m_alarm_active = false;
            //DEBUG
            printf("Alarm disabled");
        }
        next(&SM_Display::sm_state_idle);
        break;
    case dTimeout:
    case dLeft:
        m_Oled.write_string(0, 0, 1, (char *)"Toggle Alarm    ", FONT_8x8, 0, 1);
        next(&SM_Display::sm_state_idle);
        break;
    case dUp:
        m_Oled.write_string(0, 0, 1, (char *)"Toggle Alarm    ", FONT_8x8, 0, 1);
        next(&SM_Display::sm_m_state_adjust_brightness);
        break;
    case dDown:
        m_Oled.write_string(0, 0, 1, (char *)"Toggle Alarm    ", FONT_8x8, 0, 1);
        next(&SM_Display::sm_state_set_alarm);
        break;
    default:
        next(&SM_Display::sm_state_toggle_alarm);
    }
}

/**
 * @brief menu entry to set alarm
 * 
 * @param direction control menu, dRight / dEnter -> set hour/ dUp/dDown -> next menu entry / dLeft -> back to idle
 */
void SM_Display::sm_state_set_alarm(displayDirection direction)
{
    printf("State: set_alarm\n");
    printf("____________\n");
    printf("Alarm (de)aktivieren\n");
    printf(">Wecker stellen\n");
    printf("Uhrzeit einstellen\n");
    printf("Weckton aussuchen\n");
    printf("____________\n");

    m_Oled.write_string(0, 0, 0, (char *)"*************",FONT_8x8, 0,1);
    m_Oled.write_string(0, 0, 1, (char *)"Toggle Alarm", FONT_8x8, 0, 1);
    m_Oled.write_string(0, 0, 2, (char *)">Set Alarm", FONT_8x8, 0, 1);
    m_Oled.write_string(0, 0, 3, (char *)"Set Clock", FONT_8x8, 0, 1);
    m_Oled.write_string(0, 0, 4, (char *)"Set Tone", FONT_8x8, 0, 1);
    m_Oled.write_string(0, 0, 5, (char *)"Set Runtime",FONT_8x8,0,1);
    m_Oled.write_string(0, 0, 6, (char *)"Set Bringhtness",FONT_8x8, 0, 1);
    m_Oled.write_string(0, 0, 7, (char *)"*************",FONT_8x8,0, 1);

    switch (direction)
    {
    case dTimeout:
    case dLeft:
        m_Oled.fill(0,1);
        next(&SM_Display::sm_state_idle);
        break;
    case dEnter:
    case dRight:
        m_Oled.fill(0,1);
        next(&SM_Display::sm_state_alarm_hour);
        break;
    case dUp:
        m_Oled.write_string(0, 0, 2, (char *)"Set Alarm  ", FONT_8x8, 0, 1);
        next(&SM_Display::sm_state_toggle_alarm);
        break;
    case dDown:
        m_Oled.write_string(0, 0, 2, (char *)"Set Alarm  ", FONT_8x8, 0, 1);
        next(&SM_Display::sm_state_set_clock);
        break;
    default:
        next(&SM_Display::sm_state_set_alarm);
    }
}
/**
 * @brief set hour for alarm
 * 
 * @param direction conrol dRight -> set min / dEnter -> save hour / dLeft -> back to menu
 */
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
        m_alarm_time.hour = temp_h;
        rtc_set_alarm(&m_alarm_time,(rtc_callback_t )ptr_alarm_callback); //TODO: incorporate pre runtime
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
        m_alarm_time.hour = temp_h;
        next(&SM_Display::sm_state_alarm_min);
        break;
    default:
        next(&SM_Display::sm_state_alarm_hour);
    }
}

/**
 * @brief set min for alarm
 * 
 * @param direction dEnter -> confirm, idle / dLeft -> set Hour / dUp/dDown -> set min
 */
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
        m_alarm_time.min = temp_min;
        rtc_set_alarm(&m_alarm_time,(rtc_callback_t)ptr_alarm_callback); //TODO: incorporate pre runtime
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
        m_alarm_time.min = temp_min;
        next(&SM_Display::sm_state_alarm_hour);
        break;
    default:
        next(&SM_Display::sm_state_alarm_min);
    }
}

/**
 * @brief menu entry to set clock
 * 
 * @param direction control menu, dRight / dEnter -> set hour/ dUp/dDown -> next menu entry / dLeft -> back to idle
 */
void SM_Display::sm_state_set_clock(displayDirection direction)
{
    printf("State: set_clock\n");
    printf("____________\n");
    printf("Wecker stellen\n");
    printf(">Uhrzeit einstellen\n");
    printf("Weckton aussuchen\n");
    printf("Vorlaufzeit einstellen\n");
    printf("____________\n");
    m_Oled.write_string(0, 0, 0, (char *)"*************",FONT_8x8, 0,1);
    m_Oled.write_string(0, 0, 1, (char *)"Toggle Alarm", FONT_8x8, 0, 1);
    m_Oled.write_string(0, 0, 2, (char *)"Set Alarm", FONT_8x8, 0, 1);
    m_Oled.write_string(0, 0, 3, (char *)">Set Clock", FONT_8x8, 0, 1);
    m_Oled.write_string(0, 0, 4, (char *)"Set Tone", FONT_8x8, 0, 1);
    m_Oled.write_string(0, 0, 5, (char *)"Set Runtime",FONT_8x8,0,1);
    m_Oled.write_string(0, 0, 6, (char *)"Set Bringhtness",FONT_8x8, 0, 1);
    m_Oled.write_string(0, 0, 7, (char *)"*************",FONT_8x8,0, 1);
    switch (direction)
    {
    case dTimeout:
    case dLeft:
        m_Oled.fill(0,1);
        next(&SM_Display::sm_state_idle);
        break;
    case dEnter:
    case dRight:
        m_Oled.fill(0,1);
        next(&SM_Display::sm_state_clock_hour);
        break;
    case dUp:
        m_Oled.write_string(0, 0, 3, (char *)"Set Clock  ", FONT_8x8, 0, 1);
        next(&SM_Display::sm_state_set_alarm);
        break;
    case dDown:
        m_Oled.write_string(0, 0, 3, (char *)"Set Clock  ", FONT_8x8, 0, 1);
        next(&SM_Display::sm_state_set_alarmtone);
        break;
    default:
        next(&SM_Display::sm_state_set_clock);
    }
}

/**
 * @brief set hour for clock
 * 
 * @param direction conrol dRight -> set min / dEnter -> save hour / dLeft -> back to menu
 */
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
        if (rtc_running())
        {
            rtc_get_datetime(&m_dt);
        }
        m_dt.hour = temp_h;
        rtc_set_datetime(&m_dt);
        printf("Clock set");
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
        m_dt.hour = temp_h;
        next(&SM_Display::sm_state_set_clock);
        break;
    case dRight:
        if (rtc_running())
        {
            rtc_get_datetime(&m_dt);
        }
        m_dt.hour = temp_h;
        next(&SM_Display::sm_state_clock_min);
        break;
    default:
        next(&SM_Display::sm_state_clock_hour);
    }
}

/**
 * @brief set min for clock
 * 
 * @param direction dEnter -> confirm, idle / dLeft -> set Hour / dUp/dDown -> set min
 */
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
        m_dt.min = temp_min;
        rtc_set_datetime(&m_dt);
        printf("Clock set");
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

/**
 * @brief menu entry to select alarm tone
 * 
 * @param direction dLeft -> back to idle / dEnter/dRight -> select tone
 */
void SM_Display::sm_state_set_alarmtone(displayDirection direction)
{
    printf("State: set_alarmtone\n");
    printf("____________\n");
    printf("Uhrzeit einstellen\n");
    printf(">Weckton aussuchen\n");
    printf("Vorlaufzeit einstellen\n");
    printf("Wecker (de)aktivieren\n");
    printf("____________\n");
    m_Oled.write_string(0, 0, 0, (char *)"*************",FONT_8x8, 0,1);
    m_Oled.write_string(0, 0, 1, (char *)"Toggle Alarm", FONT_8x8, 0, 1);
    m_Oled.write_string(0, 0, 2, (char *)"Set Alarm", FONT_8x8, 0, 1);
    m_Oled.write_string(0, 0, 3, (char *)"Set Clock", FONT_8x8, 0, 1);
    m_Oled.write_string(0, 0, 4, (char *)">Set Tone", FONT_8x8, 0, 1);
    m_Oled.write_string(0, 0, 5, (char *)"Set Runtime",FONT_8x8,0,1);
    m_Oled.write_string(0, 0, 6, (char *)"Set Bringhtness",FONT_8x8, 0, 1);
    m_Oled.write_string(0, 0, 7, (char *)"*************",FONT_8x8,0, 1);
    switch (direction)
    {
    case dTimeout:
    case dLeft:
        m_Oled.fill(0,1);
        next(&SM_Display::sm_state_idle);
        break;
    case dEnter:
    case dRight:
        m_Oled.fill(0,1);
        next(&SM_Display::sm_state_adjust_tone);
        break;
    case dUp:
        m_Oled.write_string(0, 0, 4, (char *)"Set Tone  ", FONT_8x8, 0, 1);
        next(&SM_Display::sm_state_set_clock);
        break;
    case dDown:
        m_Oled.write_string(0, 0, 4, (char *)"Set Tone  ", FONT_8x8, 0, 1);
        next(&SM_Display::sm_state_set_pre_runtime);
        break;
    default:
        next(&SM_Display::sm_state_set_alarmtone);
    }
}

/**
 * @brief scroll trough saved alarm tones
 * 
 * @param direction dLeft -> back to Menu / dEnter -> confirm, back to idle / dUp/dDown -> scroll through tones
 */
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

/**
 * @brief menu entry to set runtume
 * 
 * @param direction dLeft -> back to idle / dEnter/dRight -> set time
 */
void SM_Display::sm_state_set_pre_runtime(displayDirection direction)
{
    printf("State: set_pre_runtime\n");
    printf("____________\n");
    printf("Weckton aussuchen\n");
    printf(">Vorlaufzeit einstellen\n");
    printf("Wecker (de)aktivieren\n");
    printf("Wecker stellen\n");
    printf("____________\n");
    m_Oled.write_string(0, 0, 0, (char *)"*************",FONT_8x8, 0,1);
    m_Oled.write_string(0, 0, 1, (char *)"Toggle Alarm", FONT_8x8, 0, 1);
    m_Oled.write_string(0, 0, 2, (char *)"Set Alarm", FONT_8x8, 0, 1);
    m_Oled.write_string(0, 0, 3, (char *)"Set Clock", FONT_8x8, 0, 1);
    m_Oled.write_string(0, 0, 4, (char *)"Set Tone", FONT_8x8, 0, 1);
    m_Oled.write_string(0, 0, 5, (char *)">Set Runtime",FONT_8x8,0,1);
    m_Oled.write_string(0, 0, 6, (char *)"Set Bringhtness",FONT_8x8, 0, 1);
    m_Oled.write_string(0, 0, 7, (char *)"*************",FONT_8x8,0, 1);
    switch (direction)
    {
    case dTimeout:
    case dLeft:
        m_Oled.fill(0,1);
        next(&SM_Display::sm_state_idle);
        break;
    case dEnter:
    case dRight:
        m_Oled.fill(0,1);
        next(&SM_Display::sm_state_adjust_time);
        break;
    case dUp:
        m_Oled.write_string(0, 0, 5, (char *)"Set Runtime ",FONT_8x8,0,1);
        next(&SM_Display::sm_state_set_alarmtone);
        break;
    case dDown:
        m_Oled.write_string(0, 0, 5, (char *)"Set Runtime ",FONT_8x8,0,1);
        next(&SM_Display::sm_m_state_adjust_brightness);
        break;
    default:
        next(&SM_Display::sm_state_set_pre_runtime);
    }
}

/**
 * @brief set runtime
 * 
 * @param direction dLeft -> back to Menu / dEnter -> confirm, back to idle / dUp/dDown -> set time (mins)
 */
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

/**
 * @brief menu entry to control brightness
 * 
 * @param direction dLeft -> back to idle / dEnter/dRight -> control brightness
 */
void SM_Display::sm_m_state_adjust_brightness(displayDirection direction){
    m_Oled.write_string(0, 0, 0, (char *)"*************",FONT_8x8, 0,1);
    m_Oled.write_string(0, 0, 1, (char *)"Toggle Alarm", FONT_8x8, 0, 1);
    m_Oled.write_string(0, 0, 2, (char *)"Set Alarm", FONT_8x8, 0, 1);
    m_Oled.write_string(0, 0, 3, (char *)"Set Clock", FONT_8x8, 0, 1);
    m_Oled.write_string(0, 0, 4, (char *)"Set Tone", FONT_8x8, 0, 1);
    m_Oled.write_string(0, 0, 5, (char *)"Set Runtime",FONT_8x8,0,1);
    m_Oled.write_string(0, 0, 6, (char *)">Set Bringhtness",FONT_8x8, 0, 1);
    m_Oled.write_string(0, 0, 7, (char *)"*************",FONT_8x8,0, 1);
    switch(direction){
        case dTimeout:
        case dLeft:
            m_Oled.fill(0,1);
            next(&SM_Display::sm_state_idle);
            break;
        case dEnter:
        case dRight:
            m_Oled.fill(0,1);
            next(&SM_Display::sm_state_adjust_brightness);
            break;
        case dUp:
            m_Oled.write_string(0, 0, 6, (char *)"Set Bringhtness ",FONT_8x8,0,1);
            next(&SM_Display::sm_state_set_pre_runtime);
            break;
        case dDown:
            m_Oled.write_string(0, 0, 6, (char *)"Set Bringhtness ",FONT_8x8,0,1);
            next(&SM_Display::sm_state_toggle_alarm);
            break;
        default:
            next(&SM_Display::sm_m_state_adjust_brightness);

    }
} 

/**
 * @brief adjust brighntess 
 * 
 * @param direction dLeft -> back to Menu / dEnter -> confirm, back to idle / dUp/dDown -> adjust brightness
 */
void SM_Display::sm_state_adjust_brightness(displayDirection direction){
    static int brighntess = 50;
    m_Oled.write_string(0, 0, 1, (char *)"Toggle Alarm    ", FONT_8x8, 0, 1);
    switch(direction){
        case dEnter:
        case dTimeout:
            m_Oled.fill(0,1);
            break;
        case dUp:
            if(brighntess<204)brighntess+=50;
            m_Oled.set_contrast(brighntess);
            next(&SM_Display::sm_state_adjust_brightness);
            break;
        case dDown:
            if(brighntess>51)brighntess-=50;
            m_Oled.set_contrast(brighntess);
            next(&SM_Display::sm_state_adjust_brightness);
            break;
        case dLeft:
            next(&SM_Display::sm_m_state_adjust_brightness);
            break;
        default:
            next(&SM_Display::sm_state_adjust_brightness);
        
    }
}


/**
 * @brief deprecated -> for debugging purposes
 * 
 * @param direction 
 */
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

/**
 * @brief deprecated -> for debugging pruposes
 * 
 * @param direction 
 */
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