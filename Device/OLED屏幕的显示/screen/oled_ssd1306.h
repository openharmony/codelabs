/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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
 

#ifndef __SSD1306_H__
#define __SSD1306_H__

#include <stddef.h>
#include <_ansi.h>

_BEGIN_STD_C

#include "oled_ssd1306_conf.h"
#include "oled_ssd1306_fonts.h"

#define IOT_USE_I2C
#define IOT_I2C_ADDER        (0x3C << 1)

#define ADC_TEST_LENGTH  64
#define KEY_EVENT_NONE 0
#define KEY_EVENT_S1 1
#define KEY_EVENT_S2 2
#define SLEEP_TIME (3 * 1000 * 1000)
#define CONTROL_TIME 100
#define ONE_THOUSAND 1000
#define TWO 2
#define FOUR 0.4
#define SIX 0.6
#define EIGHT 0.8
#define ONE 1.1
#define THREE 3.0
#define SSD1306_I2C_PORT        hi2c1
#define IOT_HEIGHT 64
#define IOT_WIDTH           128
#define IOT_BUFFER_SIZE   (IOT_WIDTH * IOT_HEIGHT / 8)
#define MEMORY 0x20
#define HORIZONTAL 0x00
#define PAGE 0xB0

#define COM 0xC8
#define HIGH 0x10
#define LINE 0x40
#define CONTRAST 0xFF

#define SEGMENT 0xA1

#define NORMAL 0xA6
#define MULTIPLEX 0xA8
#define SEEMS 0x3F
#define FOLLOWS 0xA4
#define OFFSET 0xD3
#define FREQUENCY 0xD5
#define RATIO 0xF0
#define PERIOD 0xD9
#define DEOI 0x11
#define PINS 0xDA
#define HIS 0x12
#define VCOMH 0xDB
#define VLSD 0x30
#define DCDC 0x8D
#define XSDL 0x14
#define QES 0x8000
#define FSC 0xAF
#define FSD 0xAE
typedef enum {
    BLACK = 0x00,
    WHITE = 0x01
} IOT_COLOR;

typedef enum {
    IOT_OK = 0x00,
    IOT_ERR = 0x01
} IoTErrorT;

typedef struct {
    int16_t currentX;
    int16_t currentY;
    int8_t inverted;
    int8_t initialized;
    int8_t displayOn;
} IotT;

void IotInit(void);
void IotFill(IOT_COLOR color);
void IotSetCursor(int8_t x, int8_t y);
void IotChangeScreen(void);
char IotShowChar(char ch, FontDef font, IOT_COLOR color);
char IotShowString(char* str, FontDef font, IOT_COLOR color);
void IotShowPixel(int8_t x, int8_t y, IOT_COLOR color);
void IotShowRegion(int8_t x, int8_t y, int8_t w, int8_t h, const int8_t* data, int32_t size, int32_t stride);
void IotSetContrast(const int8_t value);
void IotSetDisplayOn(const int8_t on);
int8_t IotGetDisplayOn(void);
void HalDelay(int32_t ms);
void IotReset(void);
void IotWriteCmd(int8_t byte);
void IotWriteData(int8_t* buffer, size_t buffSize);
IoTErrorT IotFillBuffer(int8_t* buf, int32_t len);

_END_STD_C

#endif
