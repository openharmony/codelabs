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
#include <string.h>
#include <unistd.h>

#include "ohos_init.h"
#include "cmsis_os2.h"
#include "iot_gpio.h"
#include "iot_pwm.h"
#include "hi_io.h"
#include "hi_pwm.h"

#define TASK_STACK_SIZE 512
// PWM输出占空比
#define PVM_OUT_DUTY 50
// PWM输出频率
#define PVM_OUT_FREQ 10000
#define TASK_SLEEP_TIME (0.03 * 1000 * 1000)

static void PWMLedTask(void *arg)
{
    (void)arg;

    while (1) {
        for (int i = 0; i < PVM_OUT_DUTY; i++) {
            // 启动PWM信号输出
            IoTPwmStart(HI_PWM_PORT_PWM1, i, PVM_OUT_FREQ);
            usleep(TASK_SLEEP_TIME);
            // 停止PWM信号输出
            IoTPwmStop(HI_PWM_PORT_PWM1);
        }

        for (int i = PVM_OUT_DUTY; i > 0; i--) {
            // 启动PWM信号输出
            IoTPwmStart(HI_PWM_PORT_PWM1, i, PVM_OUT_FREQ);
            usleep(TASK_SLEEP_TIME);
            // 停止PWM信号输出
            IoTPwmStop(HI_PWM_PORT_PWM1);
        }
    }
}

static void PWMLedExample(void)
{
    osThreadAttr_t attr;

    // 初始化10号管脚（红色led灯）
    IoTGpioInit(HI_IO_NAME_GPIO_10);

    // 将10号管脚设置为PWM功能
    hi_io_set_func(HI_IO_NAME_GPIO_10, HI_IO_FUNC_GPIO_10_PWM1_OUT);

    // 初始化PWM设备
    IoTPwmInit(HI_PWM_PORT_PWM1);

    attr.name = "PWMLedTask";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TASK_STACK_SIZE;
    attr.priority = osPriorityNormal;

    if (osThreadNew(PWMLedTask, NULL, &attr) == NULL) {
        printf("[PWMLedExample] Failed to create PWMLedTask!\n");
    }
}

APP_FEATURE_INIT(PWMLedExample);