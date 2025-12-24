
#include "BankQueueSystem.h"
#include "native_log_wrapper.h"

BankQueueSystem::BankQueueSystem(int type, const char *name, int concurrency) {
    if (type == 0) {
        queue_ =
            std::make_unique<ffrt::queue>(ffrt::queue_concurrent, name, ffrt::queue_attr().max_concurrency(concurrency));
    } else {
        queue_ =
            std::make_unique<ffrt::queue>(ffrt::queue_serial, name, ffrt::queue_attr().max_concurrency(concurrency));
    }
    LOGI("bank system has been initialized");
}

BankQueueSystem::~BankQueueSystem() {
    queue_ = nullptr;
    LOGI("bank system has been destroyed");
}

// 开始排队，即提交队列任务
ffrt::task_handle BankQueueSystem::Enter(const std::function<void()> &func, const char *name, ffrt_queue_priority_t level, int delay) {
    return queue_->submit_h(func, ffrt::task_attr().name(name).priority(level).delay(delay));
}

// 退出排队，即取消队列任务
int BankQueueSystem::Exit(const ffrt::task_handle &t) {
    return queue_->cancel(t); 
}

// 等待排队，即等待队列任务
void BankQueueSystem::Wait(const ffrt::task_handle &handle) {
    queue_->wait(handle); 
}
