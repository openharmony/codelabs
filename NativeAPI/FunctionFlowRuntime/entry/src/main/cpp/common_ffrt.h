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
#ifndef OHOS_COMMON_FFRT_H
#define OHOS_COMMON_FFRT_H

#include "ffrt/ffrt.h"

typedef struct {
    FfrtFunctionHeader header;
    FfrtFunction func;
    FfrtFunction afterFunc;
    void* arg;
} c_function_t;

// 封装提交队列任务函数
ffrt_task_handle_t commitRequest(ffrt_queue_t bank, void (*func)(void *), const char *name, void *arg,
    ffrt_queue_priority_t level, int delay);
void waitForRequest(ffrt_task_handle_t task);

// type: 0代表并发调度，非0代表串行调度
ffrt_queue_t create_bank_system(const char *name, int concurrency, int type);
void destroy_bank_system(ffrt_queue_t queue_handle);

#endif