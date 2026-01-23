/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.All Rights Reserved.
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
#define  RET_SUCCESS_4  4
#define SLEEP_DURATION_MS 100
#define THOUSAND 1000
/* 定义三个全局变量返回值 */
static int g_bankBusinessRet = -1;
static int g_bankBusinessVipRet = -1;
void BankBusiness(void *arg)
{
    usleep(SLEEP_DURATION_MS * THOUSAND);
    LOGI("saving or withdraw ordinary customer");
    g_bankBusinessRet = 0;
}

void BankBusinessVIP(void *arg)
{
    usleep(SLEEP_DURATION_MS * THOUSAND);
    LOGI("saving or withdraw VIP");
    g_bankBusinessVipRet = 0;
}

int ProcessFfrtQueue()
{
    // 串行调度
    ffrt_queue_t bank = CreateBankSystem("Bank", 2, 1);
    if (!bank) {
        LOGE("create bank system failed");
        return -1;
    }
    
    CRequest request1;
    request1.name = "customer1";
    request1.arg = NULL;
    
    CRequest request2;
    request2.name = "customer2";
    request2.arg = NULL;
    
    // VIP享受更优先的服务
    ffrt_task_handle_t task1 = CommitRequest(bank, BankBusiness, request1, ffrt_queue_priority_low, 0);
    ffrt_task_handle_t task2 = CommitRequest(bank, BankBusinessVIP, request2, ffrt_queue_priority_low, 0);

    // 等待所有的客户服务完成
    WaitForRequest(task1);
    WaitForRequest(task2);

    DestroyBankSystem(bank);

    ffrt_task_handle_destroy(task1);
    ffrt_task_handle_destroy(task2);
    LOGI("FfrtQueue results ");
    if (g_bankBusinessRet == 0 && g_bankBusinessVipRet == 0) {
    return RET_SUCCESS_4;
    } else {
        return -1;
    }
}