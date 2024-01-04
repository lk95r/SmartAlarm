
#include "hardware/spi.h"
#include "hardware/pio.h"
#include "hardware/interp.h"
#include "hardware/timer.h"
#include "hardware/watchdog.h"
#include "hardware/clocks.h"
#include "hardware/rtc.h"
#include "hardware/adc.h"
#include "hardware/gpio.h"
#include "pico/util/datetime.h"

#include "libs/sm_display/sm_display.h"
#include "libs/rgb_lights/rgb_lights.h"
#include "bme_280.h"
#include "globals.h"

#include "pico/util/queue.h"
#include "pico/mutex.h"
#include "pico/multicore.h"
#include "core_1/core_1.h"
#define FLAG_VALUE 123
#define JOYSTICK true
#define DEBOUNCE_DELAY_MS 600
#define TIMEOUT_DELAY_MS 20 * 60
#define PIN_JOYSTICK_X 26
#define PIN_JOYSTICK_Y 27
#define PIN_JOYSTICK_SW 22

extern queue_t q_date_time;
sram_t sram;
mutex_t mut_sram;
datetime_t dt;
bool error;
SM_Display fsm_display;
volatile bool sw_triggered = false;
BME280 bme;
DFP dfp;

int g_sens_temperature;

displayDirection e_menu_instruction;
// SPI Defines
// We are going to use SPI 0, and allocate it to the following GPIO pins
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define SPI_PORT spi0
#define PIN_MISO 16
#define PIN_CS 17
#define PIN_SCK 18
#define PIN_MOSI 19

void alarm_callback(void)
{
    static uint8_t alarm_state = 0;
    datetime_t alarmtime;
    uint16_t duration;
    switch (alarm_state)
    {
    case 0:// start route duration before alarm clock -> set dim lights
        alarmtime = fsm_display.get_alarm_time();
        duration = (uint16_t)(fsm_display.get_duration_min() / 2);
        if (alarmtime.min < duration)
        {
            if (alarmtime.hour >= (uint8_t)(duration / 60))
            { // not 0:00
                alarmtime.hour = alarmtime.hour - ((uint8_t)duration / 60) - 1;
                alarmtime.min = alarmtime.min + 60 - duration;
            }
            else
            {
                alarmtime.hour = alarmtime.hour - ((uint8_t)duration / 60) - 1 + 24;
                alarmtime.min = alarmtime.min + 60 - duration;
            }
        }
        else
        {
            alarmtime.min = alarmtime.min - duration;
        }
        rtc_set_alarm(&alarmtime, (rtc_callback_t)alarm_callback);
        set_ring(10, 10, 10); // low level red-yellow
        alarm_state = 1;
        break;
    case 1: // halfway throug runtime: lights get brighter and sound starts playing
        alarmtime = fsm_display.get_alarm_time();
        rtc_set_alarm(&alarmtime, (rtc_callback_t)alarm_callback);
        alarm_state = 2;
        set_ring(50, 50, 50); //
        dfp.set_volume(10);
        sleep_ms(2);
        dfp.play();
        break;
    case 2: // alarm time: lights full blast, sound gets louder
        alarmtime = fsm_display.get_alarm_time();
        alarmtime.hour = alarmtime.hour + (uint8_t)((alarmtime.min + 15) / 60);
        alarmtime.min = (alarmtime.min + 15 % 60);
        rtc_set_alarm(&alarmtime, (rtc_callback_t)alarm_callback);
        set_ring(255, 255, 255);
        dfp.set_volume(20);
        sleep_ms(2);
        dfp.play();
        alarm_state = 3;
        break;
    case 3: // 15 min after alarm: stop sound
        alarmtime = fsm_display.get_alarm_time();
        alarmtime.hour = alarmtime.hour + (uint8_t)((alarmtime.min + 30) / 60);
        alarmtime.min = (alarmtime.min + 30 % 60);
        rtc_set_alarm(&alarmtime, (rtc_callback_t)alarm_callback);
        dfp.pause();
        alarm_state = 4;
        break;
    case 4: // 30 min after alarm: set lights to 0
        set_ring(0, 0, 0);
        alarm_state = 0;
        break;
    default:
        alarm_state = 0;
        dfp.pause();
    }
}



void gpio_sw_callback(uint gpio, uint32_t events)
{
    static uint64_t u64_ts_last_input = 0;
    if (to_ms_since_boot(get_absolute_time()) - u64_ts_last_input > DEBOUNCE_DELAY_MS)
    {
        u64_ts_last_input = to_ms_since_boot(get_absolute_time());
        printf("ENTER\n");
        fsm_display.run(dEnter);
    }
}

/**
 * @brief Get Input from Joystick
 *
 * @return displayDirection
 */
displayDirection getInput(void)
{
    char c_user_input;
    displayDirection temp_insutruction = dNone;
    static displayDirection last_instruction = dNone;
    static uint64_t u64_ts_last_input = to_ms_since_boot(get_absolute_time());
#if JOYSTICK
    adc_select_input(0);
    uint adc_x = adc_read();
    adc_select_input(1);
    uint adc_y = adc_read();
    printf("x:%d\t",adc_x);
    printf("y:%d\n",adc_y);
    adc_x /= 500;
    adc_y /= 500;
    if (adc_x < 3)
    {
        temp_insutruction = dLeft;
        // DEBUG
        // printf("Left,%d\n",adc_x);
    }
    else if (adc_x > 5)
    {
        temp_insutruction = dRight;
        // DEBUG
        // printf("Right,%d",adc_x);
    }
    if (adc_y < 3)
    {
        temp_insutruction = dUp;
        // DEBUG
        // printf("Up,%d",adc_y);
    }
    else if (adc_y > 6)
    {
        temp_insutruction = dDown;
        // DEBUG
        // printf("Down,%d",adc_y);
    }
    if (to_ms_since_boot(get_absolute_time()) - u64_ts_last_input > DEBOUNCE_DELAY_MS ||
        last_instruction != temp_insutruction)
    { // button is debounced
        last_instruction = temp_insutruction;
        u64_ts_last_input = to_ms_since_boot(get_absolute_time());
        return temp_insutruction;
    }
    if (to_ms_since_boot(get_absolute_time()) - u64_ts_last_input > TIMEOUT_DELAY_MS && last_instruction == temp_insutruction)
    { // Timout //TODO: fix Timeout
        temp_insutruction = dTimeout;
        last_instruction = dTimeout;
        printf("Timeout...\n");
        return dTimeout;
    }
    else
    { // nothing
        temp_insutruction = dNone;
        return temp_insutruction;
    }
// DEBUG
// if(temp_insutruction == dNone)printf("None x:%i, y:%i",adc_x,adc_y);
// printf("\n");
#else
    c_user_input = getchar();
    printf("Input: %c\n", c_user_input);
    if (c_user_input == 'l' || c_user_input == 'L')
    {
        temp_insutruction = dLeft;
    }
    else if (c_user_input == 'r' || c_user_input == 'R')
    {
        temp_insutruction = dRight;
    }
    else if (c_user_input == 'u' || c_user_input == 'U')
    {
        temp_insutruction = dUp;
    }
    else if (c_user_input == 'd' || c_user_input == 'D')
    {
        temp_insutruction = dDown;
    }
    else if (c_user_input == 'e' || c_user_input == 'E')
    {
        temp_insutruction = dEnter;
    }
    else
    {
        temp_insutruction = dNone;
    }
#endif
    return temp_insutruction;
}

void core1_entry()
{
    core_1_main();
}
void check_rtc()
{
    if (rtc_running())
    {
        printf("c0: rtc running\n");
        rtc_get_datetime(&dt);
        printf("%d:%d:%d\n", dt.hour, dt.min, dt.sec);
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
    adc_init();
    setup_pio();
    adc_gpio_init(PIN_JOYSTICK_X);
    adc_gpio_init(PIN_JOYSTICK_Y);
    gpio_set_dir(PIN_JOYSTICK_SW, false);
    gpio_set_input_enabled(PIN_JOYSTICK_SW, true);
    gpio_set_pulls(PIN_JOYSTICK_SW, true, false);
    gpio_set_irq_enabled_with_callback(PIN_JOYSTICK_SW, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &gpio_sw_callback);
    sleep_ms(20);
    bme.init((void *)i2c0, 100 * 1000, BME280_I2C_ADDR_PRIM, 21, 20); // BME280_I2C_ADDR_PRIM
    // bme.init((void *) i2c1, 100*1000, BME280_I2C_ADDR_SEC,19, 18);
    bme.test_device_id();
    sleep_ms(500);
    g_sens_temperature = bme.get_temperature();
    printf("c0: started\n");
    // SPI initialisation. This example will use SPI at 1MHz.
    // spi_init(SPI_PORT, 1000*1000);
    // gpio_set_function(PIN_MISO, GPIO_FUNC_SPI);
    // gpio_set_function(PIN_CS,   GPIO_FUNC_SIO);
    // gpio_set_function(PIN_SCK,  GPIO_FUNC_SPI);
    // gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);
    //
    //// Chip select is active-low, so we'll initialise it to a driven-high state
    // gpio_set_dir(PIN_CS, GPIO_OUT);
    // gpio_put(PIN_CS, 1);
    /**template for future queue implementation*/
    // queue_init(&q_date_time,sizeof(dt),1);
    sleep_ms(500);
    if (fsm_display.sm_set_callback((void *)&alarm_callback) != 0)
        printf("cant set Nullptr \n");
    multicore_launch_core1(core1_entry);
    printf("c0: started core1\n");
    while (1)
    {
        sleep_ms(100);
        bme.test_device_id();
        e_menu_instruction = getInput();
        g_sens_temperature=bme.get_temperature();
        printf("%.2fC\n",(float)g_sens_temperature/100);
        e_menu_instruction = getDirection();
        fsm_display.run(e_menu_instruction);
        set_ring(00, 56, 78);
        // check_rtc();
    }
    return 0;
}
