/*
 * Copyright (c) 2020 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * @addtogroup IotHardware
 * @{
 *
 * @brief Provides APIs for operating devices,
 * including flash, GPIO, I2C, PWM, UART,ADC, and watchdog APIs.
 *
 *
 *
 * @since 2.2
 * @version 2.2
 */

/**
 * @file iot_adc.h
 *
 * @brief Declares the ADC interface functions for you to read data.
 *
 * @since 1.0
 * @version 1.1.0
 */

#ifndef IOT_ADC_H
#define IOT_ADC_H


/**
 * @brief Enumerates analog power control modes.
 */
typedef enum {
    /** Automatic control */
    IOT_ADC_CUR_BAIS_DEFAULT,
    /** Automatic control */
    IOT_ADC_CUR_BAIS_AUTO,
    /** Manual control (AVDD = 1.8 V) */
    IOT_ADC_CUR_BAIS_1P8V,
    /** Manual control (AVDD = 3.3 V) */
    IOT_ADC_CUR_BAIS_3P3V,
    /** Button value */
    IOT_ADC_CUR_BAIS_BUTT,
} IotAdcCurBais;

/**
 * @brief Enumerates equation models.
 */
typedef enum {
    /** One-equation model */
    IOT_ADC_EQU_MODEL_1,
    /** Two-equation model */
    IOT_ADC_EQU_MODEL_2,
    /** Four-equation model */
    IOT_ADC_EQU_MODEL_4,
    /** Eight-equation model */
    IOT_ADC_EQU_MODEL_8,
    /** Button value */
    IOT_ADC_EQU_MODEL_BUTT,
} IotAdcEquModelSel;

/**
 * @brief Reads a piece of sampled data from a specified ADC channel based on the input parameters.
 *
 *
 *
 * @param channel Indicates the ADC channel index.
 * @param data Indicates the pointer to the address for storing the read data.
 * @param equModel Indicates the equation model.
 * @param curBais Indicates the analog power control mode.
 * @param rstCnt Indicates the count of the time from reset to conversion start.
 *               One count is equal to 334 ns. The value must range from 0 to 0xFF0.
 * @return Returns {@link IOT_SUCCESS} if the PWM signal output is stopped;
 * returns {@link IOT_FAILURE} otherwise. For details about other return values, see the chip description.
 * @since 1.0
 * @version 1.0
 */
unsigned int IoTAdcRead(unsigned int channel, unsigned short *data, IotAdcEquModelSel equModel,
                     IotAdcCurBais curBais, unsigned short rstCnt);

#endif
/** @} */