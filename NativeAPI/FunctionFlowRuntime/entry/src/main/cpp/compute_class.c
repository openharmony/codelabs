#include "function_class.h"
#include "common_ffrt.h"
#include "native_log_wrapper.h"
#include <string.h>
#include <unistd.h>

struct ParaStruct {
    int a;
    int b;
};

struct ParaStruct g_para1;
struct ParaStruct g_para2;

void add(void *arg) {
    struct ParaStruct *para1 = (struct ParaStruct *)arg;
    int a = para1->a;
    int b = para1->b;
}

void sub(void *arg) {
    struct ParaStruct *para2 = (struct ParaStruct *)arg;
    int a = para2->a;
    int b = para2->b;
}

void ComputeFfrtQueue() {
    // 并行调度
    ffrt_queue_t bank = create_bank_system("Bank", 2, 0);
    if (!bank) {
        LOGE("create bank system failed");
        return;
    }

    g_para1.a = 1;
    g_para1.b = 2;

    g_para2.a = 5;
    g_para2.b = 3;

    // VIP享受更优先的服务
    ffrt_task_handle_t task1 = commit_request(bank, add, "customer1", &g_para1, ffrt_queue_priority_low, 0);
    ffrt_task_handle_t task2 = commit_request(bank, sub, "customer2", &g_para2, ffrt_queue_priority_low, 0);

    // 等待所有的客户服务完成
    wait_for_request(task1);
    wait_for_request(task2);

    destroy_bank_system(bank);

    ffrt_task_handle_destroy(task1);
    ffrt_task_handle_destroy(task2);
    LOGI("FfrtQueue results ");
}
