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

#define LED_INTERVAL_TIME_US (1 * 1000000)
#define LED_TASK_STACK_SIZE 512
#define LED_TEST_GPIO_9 9

// 小灯泡每隔1秒闪烁一次
static void SparkTask(void)
{
    IoTGpioInit(LED_TEST_GPIO_9);
    hi_io_set_func(LED_TEST_GPIO_9, 0);
    IoTGpioSetDir(LED_TEST_GPIO_9, IOT_GPIO_DIR_OUT);

    while (1) {
        IoTGpioSetOutputVal(LED_TEST_GPIO_9, 1);
        usleep(LED_INTERVAL_TIME_US);
        IoTGpioSetOutputVal(LED_TEST_GPIO_9, 0);
        usleep(LED_INTERVAL_TIME_US);
    }
}

static void Main(void)
{
    // 指定线程的属性
    osThreadAttr_t attr;
    attr.name = "SparkTask";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = LED_TASK_STACK_SIZE;
    attr.priority = osPriorityNormal;

    if (osThreadNew((osThreadFunc_t)SparkTask, NULL, &attr) == NULL) {
        printf("Failed to create SparkTask!\r\n");
    }

}

SYS_RUN(Main);
