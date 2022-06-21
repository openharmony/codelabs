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
 
 
#include "oled_ssd1306.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include <stdio.h>
#include <unistd.h>

#include "iot_i2c.h"
#include "iot_errno.h"

#if defined(IOT_USE_I2C)

#define IOT_I2C_IDX     0

#define IOT_CTRL_CMD 0x00
#define IOT_CTRL_DATA 0x40
#define IOT_MASK_CONT (0x1<<7)

void IotReset(void) {
}

void HalDelay(int32_t ms)
{
    int32_t msPerTick = 1000 / osKernelGetTickFreq();
    if (ms >= msPerTick) {
        osDelay(ms / msPerTick);
    }

    int32_t restMs = ms % msPerTick;
    if (restMs > 0) {
        usleep(restMs * ONE_THOUSAND);
    }
}

static int32_t IotSendData(const int8_t* data, size_t size)
{
    int id = IOT_I2C_IDX;

    return IoTI2cWrite(id, IOT_I2C_ADDER, data, size);
}

static int32_t IotWriteByte(int8_t regAdder, int8_t byte)
{
    int8_t buffer[] = {regAdder, byte};
    return IotSendData(buffer, sizeof(buffer));
}

void IotWriteCmd(int8_t byte)
{
    IotWriteByte(IOT_CTRL_CMD, byte);
}

void IotWriteData(int8_t* buffer, size_t buffSize)
{
    int8_t data[IOT_WIDTH * TWO] = {0};
    for (size_t i = 0; i < buffSize; i++) {
        data[i * TWO] = IOT_CTRL_DATA | IOT_MASK_CONT;
        data[i * TWO + 1] = buffer[i];
    }
    data[(buffSize - 1) * TWO] = IOT_CTRL_DATA;
    IotSendData(data, sizeof(data));
}
#endif

static int8_t g_iotBuffer[IOT_BUFFER_SIZE];

static IotT g_iot;

IoTErrorT IotFillBuffer(int8_t* buf, int32_t len)
{
    IoTErrorT ret = IOT_ERR;
    if (len <= IOT_BUFFER_SIZE) {
        memcpy(g_iotBuffer, buf, len);
        ret = IOT_OK;
    }
    return ret;
}

void IotInit(void)
{
    // 复位
    IotReset();

    // 等待屏幕启动
    HalDelay(CONTROL_TIME);

    // 初始化
    IotSetDisplayOn(0);

    IotWriteCmd(MEMORY);
    IotWriteCmd(HORIZONTAL);
    IotWriteCmd(PAGE);
    IotWriteCmd(COM);
    IotWriteCmd(HORIZONTAL);
    IotWriteCmd(HIGH);
    IotWriteCmd(LINE);

    // 设置对比度
    IotSetContrast(CONTRAST);

    IotWriteCmd(SEGMENT);
    IotWriteCmd(NORMAL);
    IotWriteCmd(MULTIPLEX);
    IotWriteCmd(SEEMS);
    IotWriteCmd(FOLLOWS);
    IotWriteCmd(OFFSET);
    IotWriteCmd(HORIZONTAL);
    IotWriteCmd(FREQUENCY);
    IotWriteCmd(RATIO);
    IotWriteCmd(PERIOD);
    IotWriteCmd(DEOI);
    IotWriteCmd(PINS);
    IotWriteCmd(HIS);
    IotWriteCmd(VCOMH);
    IotWriteCmd(VLSD);
    IotWriteCmd(DCDC);
    IotWriteCmd(XSDL);
    IotSetDisplayOn(1);

    // 清除屏幕显示
    IotFill(BLACK);

    // 将更改后的屏幕缓冲区写入屏幕
    IotChangeScreen();

    // 设置屏幕
    g_iot.currentX = 0;
    g_iot.currentY = 0;

    g_iot.initialized = 1;
}

// 用给定的颜色填充屏幕
void IotFill(IOT_COLOR color)
{
    int32_t i;
    for (i = 0; i < sizeof(g_iotBuffer); i++) {
        g_iotBuffer[i] = (color == BLACK) ? HORIZONTAL : CONTRAST;
    }
}

void IotChangeScreen(void)
{
    const unsigned int two = 2;
    int8_t cmd[] = {
        0X21,   // 设置列起始和结束地址
        0X00,   // 列起始地址 0
        0X7F,   // 列终止地址 127
        0X22,   // 设置页起始和结束地址
        0X00,   // 页起始地址 0
        0X07,   // 页终止地址 7
    };
    int32_t count = 0;
    int8_t data[sizeof(cmd) * TWO + IOT_BUFFER_SIZE + 1] = {};

    for (int32_t i = 0; i < sizeof(cmd) / sizeof(cmd[0]); i++) {
        data[count++] = IOT_CTRL_CMD | IOT_MASK_CONT;
        data[count++] = cmd[i];
    }

    data[count++] = IOT_CTRL_DATA;
    memcpy(&data[count], g_iotBuffer, sizeof(g_iotBuffer));
    count += sizeof(g_iotBuffer);

    int32_t ret = IotSendData(data, count);
    if (ret != IOT_SUCCESS) {
        printf("IotChangeScreen send frame data filed: %d!\r\n", ret);
    }
}

void IotShowPixel(int8_t x, int8_t y, IOT_COLOR color)
{
    const unsigned int eight = 8;
    if (x >= IOT_WIDTH || y >= IOT_HEIGHT) {
        return;
    }

    if (g_iot.inverted) {
        color = (IOT_COLOR)!color;
    }

    if (color == WHITE) {
        g_iotBuffer[x + (y / eight) * IOT_WIDTH] |= 1 << (y % eight);
    } else {
        g_iotBuffer[x + (y / eight) * IOT_WIDTH] &= ~(1 << (y % eight));
    }
}

char IotShowChar(char ch, FontDef font, IOT_COLOR color)
{
    const unsigned int thirtyTwo = 32;
    const unsigned int oHAndTwentySix = 126;
    int32_t i, b, j;

    if (ch < thirtyTwo || ch > oHAndTwentySix) {
        return 0;
    }

    if (IOT_WIDTH < (g_iot.currentX + font.fontWidth) ||
        IOT_HEIGHT < (g_iot.currentY + font.fontHeight)) {
        return 0;
    }

    for (i = 0; i < font.fontHeight; i++) {
        b = font.data[(ch - thirtyTwo) * font.fontHeight + i];
        for (j = 0; j < font.fontWidth; j++) {
            if ((b << j) & QES)  {
                IotShowPixel(g_iot.currentX + j, (g_iot.currentY + i), (IOT_COLOR) color);
            } else {
                IotShowPixel(g_iot.currentX + j, (g_iot.currentY + i), (IOT_COLOR)!color);
            }
        }
    }

    g_iot.currentX += font.fontWidth;

    return ch;
}

char IotShowString(char* str, FontDef font, IOT_COLOR color)
{

    while (*str) {
        if (IotShowChar(*str, font, color) != *str) {
            return *str;
        }

        str++;
    }

    return *str;
}

void IotSetCursor(int8_t x, int8_t y)
{
    g_iot.currentX = x;
    g_iot.currentY = y;
}

void IotShowRegion(int8_t x, int8_t y, int8_t w, int8_t h, const int8_t* data, int32_t size, int32_t stride)
{
    if (x + w > IOT_WIDTH || y + h > IOT_HEIGHT || w * h == 0) {
        printf("%dx%d @ %d,%d out of range or invalid!\r\n", w, h, x, y);
        return;
    }

    w = (w <= IOT_WIDTH ? w : IOT_WIDTH);
    h = (h <= IOT_HEIGHT ? h : IOT_HEIGHT);
    stride = (stride == 0 ? w : stride);

    int8_t rows = size * 8 / stride;
    for (int8_t i = 0; i < rows; i++) {
        int32_t base = i * stride / 8;
        for (int8_t j = 0; j < w; j++) {
            int32_t idx = base + (j / 8);
            int8_t byte = idx < size ? data[idx] : 0;
            int8_t bit  = byte & (0x80 >> (j % 8));
            IotShowPixel(x + j, y + i, bit ? WHITE : BLACK);
        }
    }
}

void IotSetContrast(const int8_t value)
{
    const int8_t kSetContrastControlRegister = 0x81;
    IotWriteCmd(kSetContrastControlRegister);
    IotWriteCmd(value);
}

void IotSetDisplayOn(const int8_t on)
{
    int8_t value;
    if (on) {
        value = FSC;
        g_iot.displayOn = 1;
    } else {
        value = FSD;
        g_iot.displayOn = 0;
    }
    IotWriteCmd(value);
}

int8_t IotGetDisplayOn()
{
    return g_iot.displayOn;
}
