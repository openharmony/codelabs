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
#include "common_ffrt.h"
#include "native_log_wrapper.h"

static inline void ffrt_exec_function_wrapper(void* t)
{
    c_function_t* f = (c_function_t *)t;
    if (f->func) {
        f->func(f->arg);
    }
}

static inline void ffrt_destroy_function_wrapper(void* t)
{
    c_function_t* f = (c_function_t *)t;
    if (f->after_func) {
        f->after_func(f->arg);
    }
}

#define FFRT_STATIC_ASSERT(cond, msg) int x(int static_assertion_##msg[(cond) ? 1 : -1])
static inline ffrt_function_header_t *ffrt_create_function_wrapper(const ffrt_function_t func,
    const ffrt_function_t after_func, void *arg)
{
    FFRT_STATIC_ASSERT(sizeof(c_function_t) <= ffrt_auto_managed_function_storage_size,
        size_of_function_must_be_less_than_ffrt_auto_managed_function_storage_size);

    c_function_t* f = (c_function_t *)ffrt_alloc_auto_managed_function_storage_base(ffrt_function_kind_queue);
    f->header.exec = ffrt_exec_function_wrapper;
    f->header.destroy = ffrt_destroy_function_wrapper;
    f->func = func;
    f->after_func = after_func;
    f->arg = arg;
    return (ffrt_function_header_t *)f;
}

ffrt_queue_t create_bank_system(const char *name, int concurrency, int type)
{
    ffrt_queue_attr_t queue_attr;
    (void)ffrt_queue_attr_init(&queue_attr);
    ffrt_queue_attr_set_max_concurrency(&queue_attr, concurrency);

    // 创建一个并发/串行 队列
    ffrt_queue_t queue;
    if (type == 0) {
        queue = ffrt_queue_create(ffrt_queue_concurrent, name, &queue_attr);
    } else {
        queue = ffrt_queue_create(ffrt_queue_serial, name, &queue_attr);
    }

    // 队列创建完后需要销毁队列属性
    ffrt_queue_attr_destroy(&queue_attr);
    if (!queue) {
        LOGE("create queue failed");
        return NULL;
    }

    LOGI("create bank system successfully");
    return queue;
}

void destroy_bank_system(ffrt_queue_t queue_handle)
{
    ffrt_queue_destroy(queue_handle);
    LOGI("destroy bank system successfully");
}

// 封装提交队列任务函数
ffrt_task_handle_t commitRequest(ffrt_queue_t bank, void (*func)(void *), const char *name, void *arg,
    ffrt_queue_priority_t level, int delay)
{
    ffrt_task_attr_t task_attr;
    (void)ffrt_task_attr_init(&task_attr);
    ffrt_task_attr_set_name(&task_attr, name);
    ffrt_task_attr_set_queue_priority(&task_attr, level);
    ffrt_task_attr_set_delay(&task_attr, delay);

    return ffrt_queue_submit_h(bank, ffrt_create_function_wrapper(func, NULL, arg), &task_attr);
}

// 封装等待队列任务函数
void waitForRequest(ffrt_task_handle_t task)
{
    ffrt_queue_wait(task);
}
