#pragma once

#include "dfplayer.h"
#include "pico/mutex.h"


#define BOARD_TYPE 0 //0 = Pico W
#define ONLINE false


typedef struct {
    uint32_t pressure;
    uint32_t temperature;
    uint32_t humidity;
}sens_data_t;

/**
 * @brief typedef for shared ram
 * 
 */
typedef struct 
{
    sens_data_t env_data;
}sram_t;

/**
 * @brief global variables declaration
 * 
 */
extern sram_t env_data;
extern mutex_t mut_sram;
extern DFP dfp;