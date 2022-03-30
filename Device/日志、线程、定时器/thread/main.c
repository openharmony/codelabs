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

#define TASK_STACK_SIZE (1024 * 4)
#define TASK_SLEEP_TIME1 (1 * 1000 * 1000)
#define TASK_SLEEP_TIME2 (2 * 1000 * 1000)

static void Task1(void)
{
    int count = 0;
    while (1) {
        printf("Task1----%d\r\n", count++);
        usleep(TASK_SLEEP_TIME1);
    }
}

static void Task2(void)
{
    int count = 0;
    while (1) {
        printf("Task2----%d\r\n", count++);
        usleep(TASK_SLEEP_TIME2);
    }
}

static void ThreadTest(void)
{
    // 指定线程的属性
    osThreadAttr_t attr;
    attr.name = "Thread_1";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TASK_STACK_SIZE;
    attr.priority = osPriorityNormal;

    if (osThreadNew((osThreadFunc_t)Task1, NULL, &attr) == NULL) {
        printf("Failed to create Task1!\r\n");
    }

    attr.name = "Thread_2";

    if (osThreadNew((osThreadFunc_t)Task2, NULL, &attr) == NULL) {
        printf("Failed to create Task2!\n\n");
    }

}

SYS_RUN(ThreadTest);
