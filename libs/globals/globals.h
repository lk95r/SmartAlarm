#pragma once

#include "libs/bme_280/bme_280.h"

#define BOARD_TYPE 0
#define ONLINE false


/**
 * @brief typedef for shared ram
 * 
 */
typedef struct sram_t
{
    sens_data_t env_data;
};

extern sram_t env_data;
