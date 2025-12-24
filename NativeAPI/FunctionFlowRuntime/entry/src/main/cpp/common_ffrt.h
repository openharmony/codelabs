#ifndef OHOS_COMMON_FFRT_H
#define OHOS_COMMON_FFRT_H

#include "ffrt/ffrt.h"

typedef struct {
    ffrt_function_header_t header;
    ffrt_function_t func;
    ffrt_function_t after_func;
    void* arg;
} c_function_t;

// 封装提交队列任务函数
ffrt_task_handle_t commit_request(ffrt_queue_t bank, void (*func)(void *), const char *name, void *arg,
    ffrt_queue_priority_t level, int delay);
void wait_for_request(ffrt_task_handle_t task);

// type: 0代表并发调度，非0代表串行调度
ffrt_queue_t create_bank_system(const char *name, int concurrency, int type);
void destroy_bank_system(ffrt_queue_t queue_handle);

#endif