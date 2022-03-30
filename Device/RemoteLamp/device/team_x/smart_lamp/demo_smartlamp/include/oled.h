/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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


#ifndef __WINDER_OLED_H__
#define __WINDER_OLED_H__

#include "ohos_types.h"

#define WIFI_IOT_OLED_I2C_IDX_0           0
#define WIFI_IOT_IO_FUNC_GPIO_13_I2C0_SDA 6
#define WIFI_IOT_IO_FUNC_GPIO_14_I2C0_SCL 6

#define I2C_REG_ARRAY_LEN           (64)
#define BH_SEND_BUFF                (1)
#define OLED_SEND_BUFF_LEN          (28)
#define OLED_SEND_BUFF_LEN2         (25)
#define OLED_SEND_BUFF_LEN3         (27)
#define OLED_SEND_BUFF_LEN4         (29)
#define Max_Column                  (128)
#define OLED_DEMO_TASK_STAK_SIZE    (4096)
#define OLED_DEMO_TASK_PRIORITY     (25)

#define OLED_ADDRESS                0x78
#define OLED_ADDRESS_WRITE_CMD      0x00
#define OLED_ADDRESS_WRITE_DATA     0x40

#define OLED_CLEAN_SCREEN           ((uint8)0x00)

#define SLEEP_20_MS                 20000
#define SLEEP_100_MS                100000

#define DELAY_10_MS                 1
#define DELAY_100_MS                10
#define DELAY_200_MS                20
#define DELAY_250_MS                25
#define DELAY_500_MS                50
/* ssd1306 register cmd */
#define DISPLAY_OFF                 0xAE
#define SET_LOW_COLUMN_ADDRESS      0x00
#define SET_HIGH_COLUMN_ADDRESS     0x10
#define SET_START_LINE_ADDRESS      0x40
#define SET_PAGE_ADDRESS            0xB0
#define CONTRACT_CONTROL            0x81
#define FULL_SCREEN                 0xFF
#define SET_SEGMENT_REMAP           0xA1
#define NORMAL                      0xA6
#define SET_MULTIPLEX               0xA8
#define DUTY                        0x3F
#define SCAN_DIRECTION              0xC8
#define DISPLAY_OFFSET              0xD3
#define DISPLAY_TYPE                0x00
#define OSC_DIVISION                0xD5
#define DIVISION                    0x80
#define COLOR_MODE_OFF              0xD8
#define COLOR                       0x05
#define PRE_CHARGE_PERIOD           0xD9
#define PERIOD                      0xF1
#define PIN_CONFIGUARTION           0xDA
#define CONFIGUARTION               0x12
#define SET_VCOMH                   0xDB
#define VCOMH                       0x30
#define SET_CHARGE_PUMP_ENABLE      0x8D
#define PUMP_ENABLE                 0x14
#define TURN_ON_OLED_PANEL          0xAF
#define CMD_LENGTH                  27

#define OLED_I2C_WRITE_CMD_SEND_LEN         2

#define HORIZONTAL_COORDINATE_OF_SLOGANS    2
#define VERTICAL_COORDINATE_OF_SLOGANS      3
#define CHAR_SIZE_OF_LANTTICE_8_16         16
#define HORI_PIXEL_OF_LANTTICE_8_16         8

#define CHAR_SIZE_OF_LANTTICE_HZ_16_16     32
#define HORI_PIXEL_OF_LANTTICE_HZ_16_16    16

#define CHAR_SIZE_OF_LANTTICE_HZ_16_16_1   48
#define HORI_PIXEL_OF_LANTTICE_HZ_16_16_1  16
#define HORI_PIXEL_OF_LANTTICE_6_8          6

#define LIMIT_HORI_PIXEL                  120
#define MAX_LINE_OF_LANTTICE                8
#define SET_POSITION_OFFSET                 4
#define MAX_OLED_INIT_CYCLE_TIMES           5


/**
 * @brief Oled Init Operations
 *
 * @since 1.0
 * @version 1.0
 */
int OledInit(void);

/**
 * @brief Oled set oled ON or OFF
 * @param value 0 set oled OFF, others set the oled ON
 *
 * @since 1.0
 * @version 1.0
 */
void OledSetOnOff(int value);

/**
 * @brief Oled clear
 *
 * @since 1.0
 * @version 1.0
 */
void OledClear(void);


/**
 * @brief Oled Show str
 *
 * @param
 *        str The str that you want show
 *        length The length of the str that you want show
 *        x The offset of X axis
 *        y The offset of Y axis
 *        charSize The CharSize (eg: 8*6)
 * @since 1.0
 * @version 1.0
 */
void OledShowStr(char *str, int length, uint8 x, uint8 y, uint8 charSize);

#endif  /* __OLED_H__ */
