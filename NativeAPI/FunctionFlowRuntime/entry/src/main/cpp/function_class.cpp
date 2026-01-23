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
#include "function_class_cpp.h"
#include "bank_queue_system.h"
#include "native_log_wrapper.h"
#include <string>
#include <unistd.h>
#include "database_ops.h"
#define SLEEP_DURATION_MS 100
#define THOUSAND 1000
#define RET_SUCCESS_1  1
#define RET_SUCCESS_2  2
#define TYPE_CONCURRENT  0
#define TYPE_SERIAL 1
#define TWO  2
#define THREE  3

/* 定义三个全局变量返回值 */
static int g_bankBusiness1Ret = -1;
static int g_bankBusinessVip1Ret = -1;
static int g_bankBusiness2Ret = -1;
static int g_bankBusinessVip2Ret = -1;
static int g_bankBusinessNewRet = -1;

FunctionClass::FunctionClass(uint32_t version)
{
    LOGI("FunctionClass::FunctionClass %d", version);
}

FunctionClass::~FunctionClass()
{
    LOGI("FunctionClass::~FunctionClass");
}

class Test {
public:
    int Add(int a, int b)
    {
        return a + b;
    }
};

void BankBusiness1()
{
    usleep(SLEEP_DURATION_MS * THOUSAND);
    LOGI("saving or withdraw ordinary customer");
    g_bankBusiness1Ret = 0;
}

void BankBusinessVip1()
{
    usleep(SLEEP_DURATION_MS * THOUSAND);
    LOGI("saving or withdraw VIP");
    g_bankBusinessVip1Ret = 0;
}

void BankBusiness2()
{
    usleep(SLEEP_DURATION_MS * THOUSAND);
    LOGI("saving or withdraw ordinary customer");
    g_bankBusiness2Ret = 0;
}

void BankBusinessVip2()
{
    usleep(SLEEP_DURATION_MS * THOUSAND);
    LOGI("saving or withdraw VIP");
    g_bankBusinessVip2Ret = 0;
}

void BankBusinessNew()
{
    DatabaseOpsDemo();
    LOGI("saving or withdraw VIP");
    g_bankBusinessNewRet = 0;
}
int FunctionClass::FfrtSerialQueue()
{
    // type传1，代表串行调度
    LOGI("FfrtQueue start ");
    BankQueueSystem bankQueue(TYPE_SERIAL, "Bank", TWO);

    auto task1 = bankQueue.Enter(BankBusiness1, "customer1", ffrt_queue_priority_low, 0);
    auto task2 = bankQueue.Enter(BankBusiness1, "customer2", ffrt_queue_priority_low, 0);
    // VIP享受更优先的服务
    auto task3 = bankQueue.Enter(BankBusinessVip1, "customer3 vip", ffrt_queue_priority_high, 0);
    auto task4 = bankQueue.Enter(BankBusiness1, "customer4", ffrt_queue_priority_low, 0);
    auto task5 = bankQueue.Enter(BankBusiness1, "customer5", ffrt_queue_priority_low, 0);

    // 取消客户4的服务
    bankQueue.Exit(task4);

    // 等待所有的客户服务完成
    bankQueue.Wait(task5);
    LOGI("FfrtQueue results ");
    if (g_bankBusiness1Ret == 0 && g_bankBusinessVip1Ret == 0) {
        return RET_SUCCESS_1;
    } else {
        return -1;
    }
}

int FunctionClass::FfrtConcurrentQueue()
{
    // type传0，代表并发调度
    BankQueueSystem bankQueue(TYPE_CONCURRENT, "Bank", THREE);

    auto task1 = bankQueue.Enter(BankBusiness2, "customer1", ffrt_queue_priority_low, 0);
    // VIP享受更优先的服务
    auto task2 = bankQueue.Enter(BankBusinessVip2, "customer3 vip", ffrt_queue_priority_high, 0);
    auto task3 = bankQueue.Enter(BankBusinessNew, "customer3 new", ffrt_queue_priority_high, 0);
    // 取消客户4的服务

    // 等待所有的客户服务完成
    bankQueue.Wait(task1);
    bankQueue.Wait(task2);
    bankQueue.Wait(task3);
    LOGI("FfrtQueue results ");
    if (g_bankBusiness2Ret == 0 && g_bankBusinessVip2Ret == 0 && g_bankBusinessNewRet == 0) {
        return RET_SUCCESS_2;
    } else {
        return -1;
    }
}
