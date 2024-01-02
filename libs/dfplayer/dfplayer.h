#pragma once
#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/uart.h"

#include "dfplayer_defines.h"

class DFP{
    public:
    DFP(void);
    /**
     * @brief initializes UART and DFP
     * 
     * @return int8_t 0: success
     */
    int8_t init(uart_inst_t* uart_inst = uart1,int tx_pin=8,int rx_pin=9,int baudrate = 9600);

    /**
     * @brief plays song from sd-card
     * 
     * @param no number of song
     * @return int8_t 0: success
     * @return int8_t -1: failed to send: load track
     * @return int8_t -2: failed to send: play sound
     */
    int8_t play(int16_t no);

    /**
     * @brief pauses audio
     * 
     * @return int8_t 0: success
     * @return int8_t -1: failed to send: pause
     */
    int8_t pause(void);

    /**
     * @brief Set playback volume
     * 
     * @param volume 0-30
     * @return int8_t 0: success
     * @return int8_t -1: failed to send: adj Volume
     */
    int8_t set_volume(uint8_t volume);

    private:
    uint8_t tx_buffer[DFP_DEFAULT_MSG_LEN];
    uint8_t rx_buffer[DFP_DEFAULT_MSG_LEN];
    uint8_t data_len;
    int baudrate;

    uart_inst_t* m_uart_inst;
    /**
     * @brief calculates checksum and writes to tx_buffer
     * 
     * @return int8_t 0: success
     */
    int8_t m_calculate_checksum(void);
    
    /**
     * @brief sends command to dfp
     * 
     * @return int8_t 0: success
     */
    int8_t m_send_cmd(void);
};