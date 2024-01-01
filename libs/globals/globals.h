#pragma once

#include "libs/bme_280/bme_280.h"
#include "pico/mutex.h"

#define BOARD_TYPE 0 //0 = Pico W
#define ONLINE false


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