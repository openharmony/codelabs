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

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "ohos_init.h"
#include "cmsis_os2.h"
#include "iot_gpio.h"

#define TASK_STACK_SIZE (1024 * 4)
#define TASK_SLEEP_TIME (1 * 1000 * 1000)

#define LED_TEST_GPIO_10 10
#define LED_TEST_GPIO_11 11
#define LED_TEST_GPIO_12 12

// 初始化红绿灯
static void InitTrafficLight(void)
{
    IoTGpioInit(LED_TEST_GPIO_10);
    hi_io_set_func(LED_TEST_GPIO_10, 0);
    IoTGpioSetDir(LED_TEST_GPIO_10, IOT_GPIO_DIR_OUT);

    IoTGpioInit(LED_TEST_GPIO_11);
    hi_io_set_func(LED_TEST_GPIO_11, 0);
    IoTGpioSetDir(LED_TEST_GPIO_11, IOT_GPIO_DIR_OUT);

    IoTGpioInit(LED_TEST_GPIO_12);
    hi_io_set_func(LED_TEST_GPIO_12, 0);
    IoTGpioSetDir(LED_TEST_GPIO_12, IOT_GPIO_DIR_OUT);
}

static void TrafficLightTask(void)
{
    InitTrafficLight();

    while (1) {
        IoTGpioSetOutputVal(LED_TEST_GPIO_10, 1);
        IoTGpioSetOutputVal(LED_TEST_GPIO_11, 0);
        IoTGpioSetOutputVal(LED_TEST_GPIO_12, 0);
        usleep(TASK_SLEEP_TIME);

        IoTGpioSetOutputVal(LED_TEST_GPIO_10, 0);
        IoTGpioSetOutputVal(LED_TEST_GPIO_11, 1);
        IoTGpioSetOutputVal(LED_TEST_GPIO_12, 0);
        usleep(TASK_SLEEP_TIME);

        IoTGpioSetOutputVal(LED_TEST_GPIO_10, 0);
        IoTGpioSetOutputVal(LED_TEST_GPIO_11, 0);
        IoTGpioSetOutputVal(LED_TEST_GPIO_12, 1);
        usleep(TASK_SLEEP_TIME);
    }
}

static void Main(void)
{
    // 指定线程的属性
    osThreadAttr_t attr;
    attr.name = "TrafficLightTask";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TASK_STACK_SIZE;
    attr.priority = osPriorityNormal;

    if (osThreadNew((osThreadFunc_t)TrafficLightTask, NULL, &attr) == NULL) {
        printf("Failed to create TrafficLightTask!\r\n");
    }
}

SYS_RUN(Main);
