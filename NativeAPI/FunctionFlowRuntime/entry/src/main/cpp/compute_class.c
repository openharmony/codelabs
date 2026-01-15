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
#define ONE  1
#define TWO  2
#define THREE  3
#define FIVE  5
#define  RET_SUCCESS_5  5
struct ParaStruct {
    int a;
    int b;
};

struct ParaStruct g_para1;
struct ParaStruct g_para2;

void Add(void *arg)
{
    struct ParaStruct *para1 = (struct ParaStruct *)arg;
    int a = para1->a;
    int b = para1->b;
}

void Sub(void *arg)
{
    struct ParaStruct *para2 = (struct ParaStruct *)arg;
    int a = para2->a;
    int b = para2->b;
}

int ComputeFfrtQueue()
{
    // 并行调度
    ffrt_queue_t bank = create_bank_system("Bank", 2, 0);
    if (!bank) {
        LOGE("create bank system failed");
        return -1;
    }
   
    g_para1.a = ONE;
    g_para1.b = TWO;

    g_para2.a = FIVE;
    g_para2.b = THREE;

    CRequest request1;
    request1.name = "customer1";
    request1.arg = &g_para1;
    
    CRequest request2;
    request2.name = "customer2";
    request2.arg = &g_para2;
    
    // VIP享受更优先的服务
    ffrt_task_handle_t task1 = commitRequest(bank, Add, request1, ffrt_queue_priority_low, 0);
    ffrt_task_handle_t task2 = commitRequest(bank, Sub, request2, ffrt_queue_priority_low, 0);

    // 等待所有的客户服务完成
    waitForRequest(task1);
    waitForRequest(task2);

    destroy_bank_system(bank);

    ffrt_task_handle_destroy(task1);
    ffrt_task_handle_destroy(task2);
    LOGI("FfrtQueue results ");
    return RET_SUCCESS_5;
}
