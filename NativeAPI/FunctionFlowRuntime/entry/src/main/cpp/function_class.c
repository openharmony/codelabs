#include "function_class.h"
#include "common_ffrt.h"
#include "native_log_wrapper.h"
#include <string.h>
#include <unistd.h>

void BankBusiness(void *arg)
{
    usleep(100 * 1000);
    LOGI("saving or withdraw ordinary customer");
}

void BankBusinessVIP(void *arg)
{
    usleep(100 * 1000);
    LOGI("saving or withdraw VIP");
}

void ProcessFfrtQueue() {
    // 串行调度
    ffrt_queue_t bank = create_bank_system("Bank", 2,1);
    if (!bank) {
        LOGE("create bank system failed");
        return;
    }

    // VIP享受更优先的服务
    ffrt_task_handle_t task1 = commit_request(bank, BankBusiness, "customer1", NULL, ffrt_queue_priority_low, 0);
    ffrt_task_handle_t task2 = commit_request(bank, BankBusinessVIP, "customer2", NULL, ffrt_queue_priority_low, 0);

    // 等待所有的客户服务完成
    wait_for_request(task1);
    wait_for_request(task2);

    destroy_bank_system(bank);

    ffrt_task_handle_destroy(task1);
    ffrt_task_handle_destroy(task2);
    LOGI("FfrtQueue results ");
}
