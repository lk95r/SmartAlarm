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
    int8_t init(void);
    private:
};