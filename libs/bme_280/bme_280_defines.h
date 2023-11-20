#pragma once
/**
* Copyright (c) 2020 Bosch Sensortec GmbH. All rights reserved.
*
* BSD-3-Clause
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
* 1. Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the following disclaimer.
*
* 2. Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the
*    documentation and/or other materials provided with the distribution.
*
* 3. Neither the name of the copyright holder nor the names of its
*    contributors may be used to endorse or promote products derived from
*    this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
* "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
* FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
* COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
* IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*
* @file       bme280_defs.h
* @date       2020-12-17
* @version    v3.5.1
*
*/
/*! @name BME280 chip identifier */
#define BME280_CHIP_ID                            UINT8_C(0x60)

/*! @name I2C addresses */
#define BME280_I2C_ADDR_PRIM                      UINT8_C(0x76)
#define BME280_I2C_ADDR_SEC                       UINT8_C(0x77)

/*! @name Register Address */
#define BME280_REG_CHIP_ID                        UINT8_C(0xD0)
#define BME280_REG_RESET                          UINT8_C(0xE0)
#define BME280_REG_TEMP_PRESS_CALIB_DATA          UINT8_C(0x88)
#define BME280_REG_HUMIDITY_CALIB_DATA            UINT8_C(0xE1)
#define BME280_REG_CTRL_HUM                       UINT8_C(0xF2)
#define BME280_REG_STATUS                         UINT8_C(0xF3)
#define BME280_REG_PWR_CTRL                       UINT8_C(0xF4)
#define BME280_REG_CTRL_MEAS                      UINT8_C(0xF4)
#define BME280_REG_CONFIG                         UINT8_C(0xF5)
#define BME280_REG_DATA                           UINT8_C(0xF7)

/*! @name Macros related to size */
#define BME280_LEN_TEMP_PRESS_CALIB_DATA          UINT8_C(26)
#define BME280_LEN_HUMIDITY_CALIB_DATA            UINT8_C(7)
#define BME280_LEN_P_T_H_DATA                     UINT8_C(8)

/*! @name Sensor power modes */
#define BME280_POWERMODE_SLEEP                    UINT8_C(0x00)
#define BME280_POWERMODE_FORCED                   UINT8_C(0x01)
#define BME280_POWERMODE_NORMAL                   UINT8_C(0x03)

#define BME280_SENSOR_MODE_MSK                    UINT8_C(0x03)
#define BME280_SENSOR_MODE_POS                    UINT8_C(0x00)

/*! @name Soft reset command */
#define BME280_SOFT_RESET_COMMAND                 UINT8_C(0xB6)

#define BME280_STATUS_IM_UPDATE                   UINT8_C(0x01)
#define BME280_STATUS_MEAS_DONE                   UINT8_C(0x08)