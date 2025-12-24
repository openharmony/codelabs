
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
    ffrt::task_handle Enter(const std::function<void()>& func, const char *name, ffrt_queue_priority_t level, int delay);

    // 退出排队，即取消队列任务
    int Exit(const ffrt::task_handle &t);

    // 等待排队，即等待队列任务
    void Wait(const ffrt::task_handle& handle);
};

#endif //OHOS_SIMPLE_BANKQUEUESYSTEM_H
