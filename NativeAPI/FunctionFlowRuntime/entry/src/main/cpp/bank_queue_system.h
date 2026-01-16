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
#ifndef OHOS_SIMPLE_BANKQUEUESYSTEM_H
#define OHOS_SIMPLE_BANKQUEUESYSTEM_H

#include <iostream>
#include <unistd.h>
#include "ffrt/ffrt.h"

class BankQueueSystem {
private:
    std::unique_ptr<ffrt::queue> queue_;

public:
    // type: 0代表并发调度，非0代表串行调度
    BankQueueSystem(int type, const char *name, int concurrency);

    ~BankQueueSystem();

    // 开始排队，即提交队列任务
    ffrt::task_handle Enter(const std::function<void()>& func,
                            const char *name,
                            ffrt_queue_priority_t level,
                            int delay);

    // 退出排队，即取消队列任务
    int Exit(const ffrt::task_handle &t);

    // 等待排队，即等待队列任务
    void Wait(const ffrt::task_handle& handle);
};

#endif //OHOS_SIMPLE_BANKQUEUESYSTEM_H
