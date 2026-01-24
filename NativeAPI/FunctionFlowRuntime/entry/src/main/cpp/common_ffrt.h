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
#ifndef OHOS_COMMON_FFRT_H
#define OHOS_COMMON_FFRT_H

#include "ffrt/ffrt.h"

typedef struct {
    ffrt_function_header_t header;
    ffrt_function_t func;
    ffrt_function_t afterFunc;
    void* arg;
} CFunction;

typedef struct {
    char *name;
    void *arg;
} CRequest;

// Queue type constants
#define TYPE_CONCURRENT 0
#define TYPE_SERIAL 1

// 封装提交队列任务函数
ffrt_task_handle_t CommitRequest(ffrt_queue_t bank, void (*func)(void *), CRequest request,
    ffrt_queue_priority_t level, int delay);
void WaitForRequest(ffrt_task_handle_t task);

// type: 0代表并发调度，非0代表串行调度
ffrt_queue_t CreateBankSystem(const char *name, int concurrency, int type);
void DestroyBankSystem(ffrt_queue_t queueHandle);

#endif