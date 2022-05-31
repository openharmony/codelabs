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


#include <stdio.h>
#include <unistd.h>
#include "ohos_init.h"
#include "cmsis_os2.h"
#include "hi_io.h"
#include "hi_adc.h"
#include "oled_ssd1306.h"

#define LED_I2C_BARRAGE (400 * 1000)
#define TASK_STACK_SIZE 4096
void IotShowChinese(void)
{
    const int32_t w = 16;
    const int32_t h = 16;
    int8_t fonts[][32] = {
        {
            /* -- ID:0,字符:"你",ASCII编码:C4E3,对应字:宽x高=16x16,画布:宽W=16 高H=16,共32字节 */
            0x11, 0x00, 0x11, 0x00, 0x11, 0x00, 0x23, 0xFC, 0x22, 0x04, 0x64, 0x08, 0xA8, 0x40, 0x20, 0x40,
            0x21, 0x50, 0x21, 0x48, 0x22, 0x4C, 0x24, 0x44, 0x20, 0x40, 0x20, 0x40, 0x21, 0x40, 0x20, 0x80,
        },
        {
            /* -- ID:1,字符:"好",ASCII编码:BAC3,对应字:宽x高=16x16,画布:宽W=16 高H=16,共32字节 */
            0x10, 0x00, 0x11, 0xFC, 0x10, 0x04, 0x10, 0x08, 0xFC, 0x10, 0x24, 0x20, 0x24, 0x24, 0x27, 0xFE,
            0x24, 0x20, 0x44, 0x20, 0x28, 0x20, 0x10, 0x20, 0x28, 0x20, 0x44, 0x20, 0x84, 0xA0, 0x00, 0x40,
        },
        {
            /* -- ID:2,字符:"世",ASCII编码:CAC0,对应字:宽x高=16x16,画布:宽W=16 高H=16,共32字节 */
            0x01, 0x10, 0x11, 0x10, 0x11, 0x10, 0x11, 0x10, 0x11, 0x14, 0xFF, 0xFE, 0x11, 0x10, 0x11, 0x10,
            0x11, 0x10, 0x11, 0x10, 0x11, 0xF0, 0x11, 0x10, 0x10, 0x00, 0x10, 0x08, 0x1F, 0xFC, 0x00, 0x00,
        },  
        {
            /* -- ID:3,字符:"界",ASCII编码:BDE7,对应字:宽x高=16x16,画布:宽W=16 高H=16,共32字节 */
            0x00, 0x10, 0x1F, 0xF8, 0x11, 0x10, 0x11, 0x10, 0x1F, 0xF0, 0x11, 0x10, 0x11, 0x10, 0x1F, 0xF0,
            0x02, 0x80, 0x04, 0x60, 0x0C, 0x50, 0x34, 0x4E, 0xC4, 0x44, 0x04, 0x40, 0x08, 0x40, 0x10, 0x40,
        }
    };
    IotFill(BLACK);
    for (size_t i = 0; i < sizeof(fonts) / sizeof(fonts[0]); i++) {
        IotShowRegion(i * w, 0, w, h, fonts[i], sizeof(fonts[0]), h);
    }
    IotChangeScreen();
}

hi_u16 g_adc_buf[ADC_TEST_LENGTH] = { 0 };
int g_keyStatus = KEY_EVENT_NONE;
char g_keyFlg = 0;

int GetKeyEvent(void)
{
    int tmp = g_keyStatus;
    g_keyStatus = KEY_EVENT_NONE;
    return tmp;
}

hi_void convert_to_voltage(hi_u32 data_len)
{
    hi_u32 i;
    float vltMax = 0;
    float vltMin = 100;
    float vltVal;
    hi_u16 vlt;
    for (i = 0; i < data_len; i++) {
        vlt = g_adc_buf[i];
        float voltage = (float)vlt * 1.8 * 4 / 4096.0;  /* vlt * 1.8 * 4 / 4096.0: Convert code into voltage */
        vltMax = (voltage > vltMax) ? voltage : vltMax;
        vltMin = (voltage < vltMin) ? voltage : vltMin;
    }
    printf("vltMin:%.3f, vltMax:%.3f \n", vltMin, vltMax);
    vltVal = (vltMin + vltMax) / TWO;
    // 按键1被按下
    if ((vltVal > FOUR) && (vltVal < SIX)) {
        if (g_keyFlg == 0) {
            g_keyFlg = 1;
            g_keyStatus = KEY_EVENT_S1;
        }
    }
    // 按键2被按下
    if ((vltVal > EIGHT) && (vltVal < ONE)) {
        if (g_keyFlg == 0) {
            g_keyFlg = 1;
            g_keyStatus = KEY_EVENT_S2;
        }
    }
    // 没有按键按下
    if (vltVal > THREE) {
        g_keyFlg = 0;
        g_keyStatus = KEY_EVENT_NONE;
    }
}

void ReadAdc(void)
{
    hi_u32 ret, i;
    hi_u16 data;  /* 10 */
    printf("ADC Start\n");

    memset_s(g_adc_buf, sizeof(g_adc_buf), 0x0, sizeof(g_adc_buf));
    
        for (i = 0; i < ADC_TEST_LENGTH; i++) {
            ret = hi_adc_read((hi_adc_channel_index)HI_ADC_CHANNEL_2,
            &data, HI_ADC_EQU_MODEL_1, HI_ADC_CUR_BAIS_DEFAULT, 0);
            if (ret != HI_ERR_SUCCESS) {
                printf("ADC Read Fail\n");
                return;
            }
            g_adc_buf[i] = data;
        }
    convert_to_voltage(ADC_TEST_LENGTH);
}

static void OLEDTask(void const *arg)
{
    (void)arg;
    
    // 指定波特率初始化I2C设备
    IoTI2cInit(0, LED_I2C_BARRAGE);
    // 初始化
    IotInit();
    
    while (1) {
        // 读取ADC值
        ReadAdc();
    switch (GetKeyEvent()) {
        case KEY_EVENT_NONE:
            {
                // Clear screen
                IotFill(BLACK);
                IotChangeScreen();
            }
            break;

        case KEY_EVENT_S1:
            {
                // 清屏
                IotFill(BLACK);
                // 设置光标位置
                IotSetCursor(0, 0);
                // 显示内容
                IotShowString("**OpenHarmony!**", g_font710, WHITE);
                IotChangeScreen();
                usleep(SLEEP_TIME);
            }
            break;

        case KEY_EVENT_S2:
            {
                // 显示中文内容
                IotShowChinese();
                usleep(SLEEP_TIME);
            }
            break;

        default:
            {
            }
            break;
        }
         // 轮询时间控制
        usleep(CONTROL_TIME);
    }
}

static void OLEDDemo(void)
{
    osThreadAttr_t attr;

    IoTGpioInit(HI_IO_NAME_GPIO_13);
    IoTGpioInit(HI_IO_NAME_GPIO_14);
    // 将引脚功能设置为I2C引脚
    hi_io_set_func(HI_IO_NAME_GPIO_13, HI_IO_FUNC_GPIO_13_I2C0_SDA);
    hi_io_set_func(HI_IO_NAME_GPIO_14, HI_IO_FUNC_GPIO_14_I2C0_SCL);

    attr.name = "OLEDTask";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TASK_STACK_SIZE;
    attr.priority = osPriorityNormal;

    if (osThreadNew(OLEDTask, NULL, &attr) == NULL) {
        printf("[OLEDDemo] Failed to create OLEDTask!\n");
    }
}
SYS_RUN(OLEDDemo);
