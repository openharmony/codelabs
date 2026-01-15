/*
 * Copyright (c) 2025 Beijing Institude of Technology.
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
#include "function_class.h"
#include "common_ffrt.h"
#include "native_log_wrapper.h"
#include <string.h>
#include <unistd.h>
#define  RET_SUCCESS_4 = 4;
void BankBusiness(void *arg)
{
    usleep(100 * 1000);
    LOGI("saving or withdraw ordinary customer");
}

void BankBusinessVIP(void *arg)
{
    usleep(100 * 1000);
    LOGI("saving or withdraw VIP");
}

int ProcessFfrtQueue() 
{
    // 串行调度
    ffrt_queue_t bank = create_bank_system("Bank", 2, 1);
    if (!bank) {
        LOGE("create bank system failed");
        return -1;
    }

    // VIP享受更优先的服务
    ffrt_task_handle_t task1 = commitRequest(bank, BankBusiness, "customer1", NULL, ffrt_queue_priority_low, 0);
    ffrt_task_handle_t task2 = commitRequest(bank, BankBusinessVIP, "customer2", NULL, ffrt_queue_priority_low, 0);

    // 等待所有的客户服务完成
    waitForRequest(task1);
    waitForRequest(task2);

    destroy_bank_system(bank);

    ffrt_task_handle_destroy(task1);
    ffrt_task_handle_destroy(task2);
    LOGI("FfrtQueue results ");
    return RET_SUCCESS_4;
}