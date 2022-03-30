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

// 重复执行的定时器
static void TimerRepeatCallback(void *arg)
{
    (void)arg;
    printf("[TimerRepeatCallback] timer repeat callback!\r\n");
}

// 只执行一次的定时器
static void TimerOnceCallback(void *arg)
{
    (void)arg;
    printf("[TimerOnceCallback] timer once callback!\r\n");
}

static void TimerTest(void)
{
    osTimerId_t id1, id2;
    uint32_t timerDelay;
    osStatus_t status;

    // 启动第一个定时器：每隔一秒打印一次
    id1 = osTimerNew(TimerRepeatCallback, osTimerPeriodic, NULL, NULL);
    if (id1 != NULL) {
        // Hi3861 1U=10ms,100U=1S
        timerDelay = 100U;

        status = osTimerStart(id1, timerDelay);
        if (status != osOK) {
            // Timer could not be started
            printf("timer repeat start failed\r\n");
        }
    }

    // 启动第二个定时器：第10秒的时候打印
    id2 = osTimerNew(TimerOnceCallback, osTimerOnce, NULL, NULL);
    if (id2 != NULL) {
        // Hi3861 1U=10ms,1000U=10S
        timerDelay = 1000U;

        status = osTimerStart(id2, timerDelay);
        if (status != osOK) {
            // Timer could not be started
            printf("timer once start failed\r\n");
        }
    }
}

SYS_RUN(TimerTest);
